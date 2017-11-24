/* File : thread.c
 * Auth: isinstance
 * Date: 20171102
 * Mail: no
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
//#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "http.h"
#include "debug.h"
#include "rand.h"
#include "exploit.h"

//void *exploit(void *input);

void work(struct INPUT_ARG *pinput)
{

    pid_t pid;
    pthread_t tid;
    int process_i;
    int debug_mode = pinput->DebugMode;
    int thread_i;
    int ret;

    // Create the process and thread here
    for (process_i = 0; process_i < MAX_PROCESS_NUM; process_i++)
    {
        // [1, MAX_PROCESS_NUM] process
        pid = fork();
        if (pid == 0)
        {
            // Child process
            for (thread_i = 0; thread_i < MAX_THREAD_NUM; thread_i++)
            {
                // [1, MAX_THREAD_NUM] thread
                pinput->ThreadSeed = thread_i + process_i;
                ret = pthread_create(&tid, NULL, (void *)exploit, pinput);
                if (ret != 0)
                {
                    debug(debug_mode, 1, "Create pthread error...");
                }
                pthread_join(tid, NULL);
            }

            //exploit(pinput);
        }
        else if (pid < 0)
        {
            // Error now
            debug(debug_mode, 1, "Create process error\n");
        }
        else
        {

            int wait_val;
            int child_id;
            // Father process
            child_id = wait(&wait_val);
            debug(debug_mode, 2, "Child exit, process id: %d", child_id);
            if (WIFEXITED(wait_val))
            {
                debug(debug_mode, 2, "child exited with code %d", WEXITSTATUS(wait_val));
                if (debug_mode != 2)
                {
                    work(pinput);
                }
            }
            else
            {
                debug(debug_mode, 2, "child exited NOT normally");
                // sleep() for test
                //sleep(1);
                if (debug_mode != 2)
                {
                    work(pinput);
                }
            }
        }
    }
}

struct PARSE_RETURN *parse_input(const int iargc, char **pargv)
{
    /*
     * Parse the input from user
     * There is someting should explain
     * Use the struct PARSE_RETURN to return vaule
     */
    int i;
    char *ptmp = NULL;

    struct PARSE_RETURN *prs = NULL;
    struct PARSE_RETURN return_struct;
    prs = &return_struct;

    for (i = 1; i < iargc; i++)
    {
        ptmp = (char *)strstr(pargv[i], "-");
        if (!ptmp)
        {
            printf("Illegal input\n");
            return NULL;
        }
        switch (*(ptmp + 1))
        {
        case 't':
            prs->toption = 1;
            break;
        case 'r':
            prs->roption = 1;
            break;
        case 'd':
            prs->doption = 1;
            break;
        case 'D':
            prs->doption = 2;
            break;
        default:
            printf("Not have this option\n");
            printf(USAGE);
            return NULL;
        }
    }
    return prs;
}

int main(int argc, char *argv[])
{
    /*
     * Main function here
     */

    // Mode setting
    /// ATTACL_MODE_DEFAULT default is '0'(mean do NOT use the attack mode)
    int attack = ATTACK_MODE_DEFAULT;
    // RAND_USER_NAME_DEFAULT default is '0'(mean do NOT use random user name)
    int rand_flag = RAND_USER_NAME_DEFAULT;
    // DEBUG_MODE_DEFAULT default is '0'
    int debug_mode = DEBUG_MODE_DEFAULT;
    // Accept the parse_input function return value
    struct PARSE_RETURN *parse_result;
    char **pargv = argv;
    int *mvalue;

    struct INPUT_ARG *pinput = (struct INPUT_ARG *)malloc(sizeof(struct INPUT_ARG));

    // Store the max_loop_time
    mvalue = (int *)calloc(BUFFER_SIZE, sizeof(int));
    if (argc > 1)
    {
        parse_result = (struct PARSE_RETURN *)parse_input(argc, pargv);
        if (parse_result == NULL)
        {
            printf("Wrrong input option\n");
            return -1;
        }
        if (parse_result->toption != 0)
        {
            attack = parse_result->toption;
        }
        if (parse_result->roption != 0)
        {
            rand_flag = parse_result->roption;
        }
        if (parse_result->doption != 0)
        {
            debug_mode = parse_result->doption;
        }
    }

    // Init struct
    if (rand_flag < 0)
    {
        rand_flag = 0;
    }
    if (debug_mode < 0)
    {
        debug_mode = 0;
    }
    if (attack < 0)
    {
        attack = 0;
    }
    pinput->RandFlag = rand_flag;
    pinput->DebugMode = debug_mode;
    pinput->Attack = attack;

    debug(debug_mode, 1, "End parse input");
    debug(debug_mode, 2, "Started nonsense mode");
    debug(debug_mode, 2, "RandFlag:%d - DebugMode:%d - Attack:%d", pinput->RandFlag, pinput->DebugMode, pinput->Attack);
    if (debug_mode == 2)
    {
        debug(debug_mode, 2, "Sleep 2s");
        sleep(2);
    }

    work(pinput);

    free(mvalue);
    free(pinput);
    return 0;
}