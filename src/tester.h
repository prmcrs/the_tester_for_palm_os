#ifndef TESTER_H
#define TESTER_H
#include <PalmOS.h>
void TesterLog(const Char *message);
void TesterTcpDns(void);
void TesterFetch(const Char *path, const Char *label);
#endif
