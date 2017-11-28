/* File: exploit.h
 * Auth: isinstance
 * Date: 20171124
 * Mail: no
 */

#ifndef AHTTP_H
#define AHTTP_H

struct AHTTP_INPUT
{
    int MaxLoop;
    int DebugMode;
    char *SrcIP;
    char *DstIP;
    int SrcPort;
    int DstPort;
};

void dosattack(const struct AHTTP_INPUT *ainput);

#endif