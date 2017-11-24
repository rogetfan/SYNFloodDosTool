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

#ifndef MY_RAND_MAX_USERNAME_LENGTH
#define MY_RAND_MAX_USERNAME_LENGTH 12
#endif

#ifndef MY_RAND_MAX_USERNAME_LENGTH
#define MY_RAND_MAX_PASSWORD_LENGTH 12
#endif

void rand_string(char *rebuf, const struct RAND_INPUT *input)
{
    /* NOTE:
     * rebuf        Which to store the data, definition in exploit.c
     * Flag         If flag=0, output the username random string, else output password random string
     * NumLoop      [0, 1024] loop to generate the randome number in one seconds
     * DebugMode    Show more infomation
     */

    int rand_number_0 = -1;
    int rand_number_1 = -1;
    int MAX;
    int re_seed = 0;
    char *lstring;
    //char dtmp[BUFFER_SIZE] = {'\0'};

    struct RAND_INPUT *ptmp = (struct RAND_INPUT *)input;

    if (ptmp->Flag == 0)
    {
        MAX = (int)MY_RAND_MAX_USERNAME_LENGTH;
    }
    else if(ptmp->Flag == 1)
    {
        //printf("%d\n", (int)MY_RAND_MAX_PASSWORD_LENGTH);
        MAX = (int)MY_RAND_MAX_PASSWORD_LENGTH;
    }
    lstring = (char *)calloc((MAX + 1), sizeof(char));
    rand_number_0 = 1 + (int)(rand() % (MAX - 1));

    re_seed = ptmp->NumLoop + ptmp->Seed;
    if (re_seed > BUFFER_SIZE)
    {
        re_seed = 0;
    }
    // seand is here
    srand((int)time(0) + re_seed);

    //printf("%d\n", MAX);
    int i;
    for (i = 0; i < rand_number_0; i++)
    {
        // [a, b] random interger
        // [33, 126] except space[32]
        // 92 = 126 - 33 - 1
        rand_number_1 = 33 + (int)(rand() % 92);
        //printf("62\n");
        //printf("%d\n", rand_number);
        if (isprint(rand_number_1))
        {
            //printf("%d\n", rand_number);
            //printf("%c\n", rand_number);
            sprintf(lstring, CAT_STRING, lstring, rand_number_1);
            //printf("%s\n", lstring);
        }
    }
    //sprintf(dtmp, "Password: %s", lstring);
    //debug(ptmp->DebugMode, dtmp);
    //printf("%s\n", lstring);
    strcpy(rebuf, lstring);
    //printf("%s\n", rebuf);
}

/*
int main(int argc, char *argv[])
{
    // This code is for test

    char *test_buf;
    int num_loop = 0;
    int control_seed = 0;
    int seed = 0;
    struct RAND_INPUT *testinput = (struct RAND_INPUT *)malloc(sizeof(struct RAND_INPUT));
    ;
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
        if (control_seed == 4)
        {
            ++seed;
            control_seed = 0;
        }
        ++control_seed;

        testinput->DebugMode = 1;
        testinput->Flag = 1;
        testinput->NumLoop = num_loop;
        testinput->Seed = control_seed;

        rand_string(test_buf, testinput);
        printf("%s\n", test_buf);
        ++num_loop;
        if (num_loop >= 1024)
        {
            num_loop = 0;
        }
        //sleep(1);
    }
    free(test_buf);
    free(testinput);
    return 0;
}
*/