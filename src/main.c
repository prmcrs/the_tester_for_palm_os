/*
 * The Tester for Palm OS - early native 68K prototype.
 * (c) 2026 Pablo Goitisolo
 */
#include <PalmOS.h>
#include "tester.h"

#define LOG_LINES 11
#define LINE_CHARS 62

static Char gLines[LOG_LINES][LINE_CHARS];
static UInt16 gLineCount = 0;
static Boolean gNeedsRedraw = true;

static void drawLine(Coord x, Coord y, const Char *s)
{
    WinDrawChars(s, StrLen(s), x, y);
}

static void redraw(void)
{
    UInt16 i;
    Coord y = 8;
    RectangleType bounds;
    bounds.topLeft.x = 0;
    bounds.topLeft.y = 0;
    bounds.extent.x = 160;
    bounds.extent.y = 160;
    WinEraseRectangle(&bounds, 0);
    FntSetFont(boldFont);
    drawLine(4, y, "<3 THE TESTER / PALM");
    y += 16;
    FntSetFont(stdFont);
    drawLine(4, y, "H/V/M: health | R: TCP/DNS");
    y += 12;
    drawLine(4, y, "T: raw HTTP | Y: HTTP");
    y += 12;
    drawLine(4, y, "C: clear | ?: help");
    y += 15;
    drawLine(4, y, "--- DEBUG OUTPUT ---");
    y += 12;
    for (i = 0; i < gLineCount && y < 155; ++i) {
        drawLine(4, y, gLines[i]);
        y += 11;
    }
    gNeedsRedraw = false;
}

void TesterLog(const Char *message)
{
    UInt16 i;
    if (gLineCount == LOG_LINES) {
        for (i = 1; i < LOG_LINES; ++i)
            StrCopy(gLines[i - 1], gLines[i]);
        gLineCount--;
    }
    StrNCopy(gLines[gLineCount], message, LINE_CHARS - 1);
    gLines[gLineCount][LINE_CHARS - 1] = 0;
    gLineCount++;
    gNeedsRedraw = true;
    redraw();
}

static void clearLog(void)
{
    gLineCount = 0;
    TesterLog("READY.");
}

static void handleKey(WChar chr)
{
    switch (chr) {
    case 'h': case 'H': TesterFetch("/health.txt", "HEALTH"); break;
    case 'v': case 'V': TesterFetch("/server.txt", "SERVER"); break;
    case 'm': case 'M': TesterFetch("/services.txt", "SERVICES"); break;
    case 'r': case 'R': TesterTcpDns(); break;
    case 't': case 'T': TesterFetch("/", "RAW HTTP"); break;
    case 'y': case 'Y': TesterFetch("/", "HTTP CONNECTION"); break;
    case 'c': case 'C': clearLog(); break;
    case '?':
        TesterLog("H/V/M health; R DNS/TCP");
        TesterLog("T/Y HTTP; C clear");
        break;
    default: break;
    }
}

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
    EventType event;
    if (cmd != sysAppLaunchCmdNormalLaunch)
        return 0;
    clearLog();
    do {
        EvtGetEvent(&event, evtWaitForever);
        if (event.eType == keyDownEvent)
            handleKey(event.data.keyDown.chr);
        if (event.eType == winEnterEvent || event.eType == appUpdateEvent)
            gNeedsRedraw = true;
        if (!SysHandleEvent(&event) && !MenuHandleEvent(0, &event, 0))
            ; /* No forms are used in this prototype. */
        if (gNeedsRedraw)
            redraw();
    } while (event.eType != appStopEvent);
    return 0;
}
