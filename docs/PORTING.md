# Porting notes (BlackBerry -> Palm OS)

Reference: Pablo Goitisolo's `TheTester.java` (BlackBerry JDE 4.6.1, The Tester 1.6).

| BlackBerry action | Palm OS implementation |
| --- | --- |
| H / V / M health endpoints | HTTP/1.0 GET to health.txt / server.txt / services.txt |
| R TCP socket / DNS | NetLibGetHostByName + TCP connect to example.com:80 |
| T raw HTTP | Plain HTTP/1.0 request |
| Y HttpConnection | Same socket implementation for now; separate HTTP abstraction planned |
| U HTTPBingo (HTTPS) | Not implemented: needs TLS or proxy |
| I numeric-IP probes | Planned |
| O TCP 53 | Planned, not a DNS-resolution test |
| P / L WAP and WML | Planned; plain HTTP only |
| N RadioInfo | Requires Treo-specific research |
| S CoverageInfo | No direct equivalent |
| APN/deviceSide | BlackBerry-specific; configure network in Palm OS Preferences |

## Prototype limitations

- UI is keyboard-driven using the legacy 160x160 logical coordinate system; no resource-driven menus/forms yet.
- NetLib calls currently run on the UI event loop, so a timed-out request temporarily blocks input.
- HTTP reads only the *first received packet*, extracts the HTTP status line, and **does not yet render the body**.
- Calls are synchronous and HTTP only; redirects, chunked transfer, and HTTPS are out of scope.
- No APN passwords are included in the port.
- This code has **not been compiled against an actual Palm OS SDK** or tested on a Treo 650. API signature and linker validation are the next required step.
- No `.prc` binary is committed until the SDK build and device/simulator test pass.

## Milestone sequence

1. Validate toolchain, compile PRC, launch in POSE/Garnet Simulator.
2. Verify keys and drawing on physical Treo 650.
3. Improve asynchronous UI, log scrolling, HTTP status/body parsing.
4. Add remaining connectivity probes, preferences, diagnostic output.
