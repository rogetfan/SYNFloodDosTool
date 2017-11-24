/* File: debug.c 
 * Auth: isinstance
 * Date: 20171102
 * Mail: no
 */
#include <stdio.h>

void debug(int flag, char *pstring)
{
    /*
     * debug mode use
     */
    if (flag == 1)
    {
        printf("DEBUG: [%s]\n", pstring);
    }
}