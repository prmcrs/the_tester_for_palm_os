/*
 * NetLib HTTP/1.0 diagnostic probes.
 * No TLS; Palm OS network connection is managed in system Network prefs.
 */
#include <PalmOS.h>
#include <NetMgr.h>
#include "tester.h"

#define NET_TIMEOUT (10 * sysTicksPerSecond)
#define HOST_NAME "primo-tech.neocities.org"

static Boolean openNetwork(UInt16 *refNum)
{
    Err err;
    UInt16 ifErr;
    err = SysLibFind("Net.lib", refNum);
    if (err != errNone) {
        TesterLog("NetLib not found.");
        return false;
    }
    err = NetLibOpen(*refNum, &ifErr);
    if (err != errNone && err != netErrAlreadyOpen) {
        TesterLog("NetLibOpen failed.");
        return false;
    }
    return true;
}

/* DNS and socket connect are separate steps to isolate failures. */
static NetSocketRef connectHost(UInt16 refNum, const Char *host, UInt16 port)
{
    NetHostInfoBufType hostBuf;
    NetHostInfoType *hostInfo;
    NetSocketAddrINType addr;
    NetSocketRef sock;
    Err err = errNone;
    Int16 result;

    hostInfo = NetLibGetHostByName(refNum, (Char *)host,
                                  &hostBuf, sizeof(hostBuf),
                                  NET_TIMEOUT, &err);
    if (!hostInfo || err != errNone ||
        !hostInfo->addressListP || !hostInfo->addressListP[0]) {
        TesterLog("DNS lookup failed.");
        return -1;
    }
    TesterLog("DNS resolution: OK");
    sock = NetLibSocketOpen(refNum, netSocketAddrINET,
                            netSocketTypeStream, netSocketProtoIPTCP,
                            NET_TIMEOUT, &err);
    if (sock < 0) {
        TesterLog("Socket open failed.");
        return -1;
    }
    MemSet(&addr, sizeof(addr), 0);
    addr.family = netSocketAddrINET;
    addr.port = NetHToNS(port);
    addr.addr = *hostInfo->addressListP[0];
    result = NetLibSocketConnect(refNum, sock, (NetSocketAddrType *)&addr,
                                  sizeof(addr), NET_TIMEOUT, &err);
    if (result < 0) {
        TesterLog("TCP connect failed.");
        NetLibSocketClose(refNum, sock, NET_TIMEOUT, &err);
        return -1;
    }
    TesterLog("TCP connect: OK");
    return sock;
}

void TesterTcpDns(void)
{
    UInt16 refNum;
    NetSocketRef sock;
    Err err = errNone;
    TesterLog("[TCP / DNS]");
    if (!openNetwork(&refNum))
        return;
    sock = connectHost(refNum, "example.com", 80);
    if (sock >= 0)
        NetLibSocketClose(refNum, sock, NET_TIMEOUT, &err);
    NetLibClose(refNum, false);
}

void TesterFetch(const Char *path, const Char *label)
{
    UInt16 refNum;
    NetSocketRef sock;
    Err err = errNone;
    Char request[256];
    Char response[128];
    Int16 count;
    UInt16 i;
    TesterLog(label);
    if (!openNetwork(&refNum))
        return;
    sock = connectHost(refNum, HOST_NAME, 80);
    if (sock < 0) {
        NetLibClose(refNum, false);
        return;
    }
    StrPrintF(request, "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
              path, HOST_NAME);
    count = NetLibSend(refNum, sock, request, StrLen(request),
                       0, 0, 0, NET_TIMEOUT, &err);
    if (count <= 0) {
        TesterLog("HTTP send failed.");
    } else {
        count = NetLibReceive(refNum, sock, response, sizeof(response)-1,
                              0, 0, 0, NET_TIMEOUT, &err);
        if (count > 0) {
            response[count] = 0;
            for (i = 0; i < (UInt16)count; ++i) {
                if (response[i] == '\r' || response[i] == '\n') {
                    response[i] = 0;
                    break;
                }
            }
            TesterLog(response); /* First HTTP status line only. */
        } else {
            TesterLog("HTTP receive failed.");
        }
    }
    NetLibSocketClose(refNum, sock, NET_TIMEOUT, &err);
    NetLibClose(refNum, false);
}
