/* File: rand.c
 * Auth: isinstance
 * Date: 20171031
 * Mail: no
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "rand.h"
#include "debug.h"
#include "http.h"
#include "exploit.h"

#define CAT_STRING "%s%c"
#define CAT_NUM "%d%c"
#define IP_STRING "%s.%d"

#ifndef MY_RAND_MAX_USERNAME_LENGTH
#define MY_RAND_MAX_USERNAME_LENGTH 12
#endif

#ifndef MY_RAND_MAX_USERNAME_LENGTH
#define MY_RAND_MAX_PASSWORD_LENGTH 12
#endif

char *grand_user(struct GINPUT *pinput)
{
    /*
     * Generate the random user name and return
     */

    struct GINPUT *ptmp = (struct GINPUT *)malloc(sizeof(struct GINPUT));
    ptmp = pinput;
    char *lstring;
    int rand_number_0 = -1;
    int rand_number_1 = -1;
    float MAX = ptmp->Max;
    float re_seed = 0;

    lstring = (char *)calloc((MAX + 1), sizeof(char));
    memset(lstring, '\0', strlen(lstring));
    rand_number_0 = 1 + (int)(rand() % ((int)MAX - 1));

    re_seed = ptmp->Seed;
    if (re_seed > 1024)
    {
        re_seed = 0;
    }
    // srand is here
    srand((int)time(0) + re_seed);

    int i;
    for (i = 0; i < rand_number_0; i++)
    {
        // [a, b] random interger
        // [33, 126] except space[32]
        // 92 = 126 - 33 - 1
        rand_number_1 = 33 + (int)(rand() % 92);
        if (isprint(rand_number_1))
        {
            //printf("%d\n", rand_number);
            //printf("%c\n", rand_number);
            sprintf(lstring, CAT_STRING, lstring, rand_number_1);
            //printf("%s\n", lstring);
        }
    }
    //printf("%s\n", lstring);
    free(ptmp);
    return lstring;
}

char *grand_passwd(struct GINPUT *pinput)
{
    /*
     * Generate the random password and return
     */

    struct GINPUT *ptmp = (struct GINPUT *)malloc(sizeof(struct GINPUT));
    ptmp = pinput;
    char *lstring;
    int rand_number_0 = -1;
    int rand_number_1 = -1;
    float MAX = ptmp->Max;
    float re_seed = 0;

    lstring = (char *)calloc((MAX + 1), sizeof(char));
    memset(lstring, '\0', strlen(lstring));
    rand_number_0 = 1 + (int)(rand() % ((int)MAX - 1));

    re_seed = ptmp->Seed;
    if (re_seed > 1024)
    {
        re_seed = 0;
    }
    // srand is here
    srand((int)time(0) + re_seed);

    int i;
    for (i = 0; i < rand_number_0; i++)
    {
        // [a, b] random interger
        // [33, 126] except space[32]
        // 92 = 126 - 33 - 1
        rand_number_1 = 33 + (int)(rand() % 92);
        if (isprint(rand_number_1))
        {
            //printf("%d\n", rand_number);
            //printf("%c\n", rand_number);
            sprintf(lstring, CAT_STRING, lstring, rand_number_1);
            //printf("%s\n", lstring);
        }
    }
    //printf("%s\n", lstring);
    free(ptmp);
    return lstring;
}

void rand_string(char *rebuf, const struct RAND_INPUT *input)
{
    /* NOTE:
     * rebuf        Which to store the data, definition in exploit.c
     * Flag         If flag=0, output the username random string, else output password random string
     * NumLoop      [0, 1024] loop to generate the randome number in one seconds
     * DebugMode    Show more infomation
     */

    int MAX;
    char *strtmp = NULL;

    struct RAND_INPUT *ptmp = (struct RAND_INPUT *)input;
    struct GINPUT *sinput = (struct GINPUT *)malloc(sizeof(struct GINPUT));

    if (ptmp->RandFlag == 0)
    {
        MAX = (int)MY_RAND_MAX_USERNAME_LENGTH;
        strtmp = (char *)calloc((MAX + 1), sizeof(char));
        sinput->Seed = (float)ptmp->NumLoop + ptmp->Seed;
        sinput->Max = (float)MAX;
        strtmp = grand_passwd(sinput);
    }
    else if (ptmp->RandFlag == 1)
    {
        MAX = (int)MY_RAND_MAX_PASSWORD_LENGTH;
        strtmp = (char *)calloc((MAX + 1), sizeof(char));
        sinput->Seed = (float)ptmp->NumLoop + ptmp->Seed;
        sinput->Max = (float)MAX;
        strtmp = grand_passwd(sinput);
    }
    //free(sinput);
    strcpy(rebuf, strtmp);
}

int rand_num(int seed)
{
    /*
     * Return the random number between 1-255
     */

    int rand_number = -1;
    int return_num = -1;

    // srand is here
    srand((int)time(0) + seed);

    // [a, b] random interger
    // [1, 254] except space[32]
    // 252 = 254 - 1 - 1
    rand_number = 1 + (int)(rand() % 252);
    return_num = rand_number;
    return return_num;
}

void rand_ip(char *rebuf)
{
    /*
     * Return the random ip address
     */

    int i;
    int ip_part = 0;
    char *ip_string;

    ip_string = (char *)calloc(20, sizeof(char));
    memset(ip_string, '\0', strlen(ip_string));

    for (i = 0; i < 4; ++i)
    {
        // ip has four num like 192 168 1 1
        ip_part = rand_num(i);
        sprintf(ip_string, IP_STRING, ip_string, ip_part);
    }

    // delete the first '.'
    ip_string = ip_string + 1;
    strcpy(rebuf, ip_string);
}

int rand_port(void)
{
    /*
     * Return randome port
     */

    int rand_number = -1;
    int return_num = -1;

    // srand is here
    srand((int)time(0));

    // [a, b] random interger
    // [1, 9999] except space[32]
    // 9997 = 9999 - 1 - 1
    rand_number = 1 + (int)(rand() % 9997);
    return_num = rand_number;
    return return_num;
}

/*
int main(int argc, char *argv[])
{
    // This code is for test

    char *test_buf;
    int num_loop = 0;
    int control_seed = 0;
    int seed = 0;
    //struct RAND_INPUT *testinput = (struct RAND_INPUT *)malloc(sizeof(struct RAND_INPUT));
    test_buf = (char *)calloc(MY_RAND_MAX_PASSWORD_LENGTH, sizeof(char));
    for (;;)
    {
        // Simulation the muti thread situation
        // In this situaion
        // May thread will have the same time value and seed value
        // This test for envn though we can create the random string

        if (seed >= 1024)
        {
            seed = 0;
        }
        ++seed;
        //if (control_seed == 4)
        //{
        //    ++seed;
        //    control_seed = 0;
        //}
        //++control_seed;

        //testinput->DebugMode = 1;
        //testinput->RandFlag = 1;
        //testinput->NumLoop = num_loop;
        //testinput->Seed = control_seed;

        //rand_string(test_buf, testinput);
        rand_ip(test_buf);
        printf("%s\n", test_buf);
        //++num_loop;
        //if (num_loop >= 1024)
        //{
        //    num_loop = 0;
        //}
        //sleep(1);
    }
    free(test_buf);
    //free(testinput);
    return 0;
}
*/