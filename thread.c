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
#include <sys/types.h>
#include <sys/wait.h>

#include "http.h"
#include "debug.h"
#include "rand.h"
#include "exploit.h"

void *exploit(void *input);

void work(struct INPUT_ARG *pinput)
{

    pid_t pid;
    pthread_t tid;
    int process_i;
    int DEBUGMODE = pinput->DebugMode;
    int thread_i;
    int ret;
    char pdebug[BUFFER_SIZE] = {'\0'};

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
                //sprintf(pdebug, "p_i:%d - t_i:%d", process_i, thread_i);
                //debug(DEBUGMODE, pdebug);
                pinput->ThreadSeed = thread_i + process_i;
                ret = pthread_create(&tid, NULL, (void *)exploit, pinput);
                if (ret != 0)
                {
                    printf("Create pthread error!\n");
                }
                //sprintf(pdebug, "End ret thread.c: %d", ret);
                //debug(DEBUGMODE, pdebug);
                pthread_join(tid, NULL);
            }

            //exploit(pinput);
        }
        else if (pid < 0)
        {
            // Error now
            printf("Create process error\n");
        }
        else
        {

            int wait_val;
            int child_id;
            // Father process
            child_id = wait(&wait_val);
            sprintf(pdebug, "Child exit, process id: %d", child_id);
            debug(DEBUGMODE, pdebug);
            if (WIFEXITED(wait_val))
            {
                printf("child exited with code %d\n", WEXITSTATUS(wait_val));
            }
            else
            {
                printf("child exited NOT normally\n");
                // sleep() for test
                //sleep(1);
                work(pinput);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    /*
     * Main function here
     */

    // Mode setting
    // MAX_LOOP_TIME default is '-1'
    int MAXLOOPTIME = MAX_LOOP_TIME_DEFAULT;
    // RAND_USER_NAME_DEFAULT default is '0'(mean do NOT use random user name)
    int RANDFLAG = RAND_USER_NAME_DEFAULT;
    // DEBUG_MODE_DEFAULT default is '0'
    int DEBUGMODE = DEBUG_MODE_DEFAULT;
    // NOT_RECV_DEFAULT is '0'
    int NOTRECV = NOT_RECV_DEFAULT;
    // Accept the parse_input function return value
    struct PARSE_RETURN *parse_result;
    char **pargv = argv;
    int *mvalue;
    char pdebug[BUFFER_SIZE] = {'\0'};

    struct INPUT_ARG *pinput = (struct INPUT_ARG *)malloc(sizeof(struct INPUT_ARG));

    // Store the max_loop_time
    mvalue = (int *)calloc(BUFFER_SIZE, sizeof(int));
    if (argc > 1)
    {
        parse_result = (struct PARSE_RETURN *)parse_input(argc, pargv);
        if (parse_result == NULL)
        {
            printf("Wrrong input option\n");
            return 1;
        }
        if (parse_result->doption == 1)
        {
            DEBUGMODE = 1;
        }
        if (parse_result->noption == 1)
        {
            NOTRECV = 1;
        }
        if (parse_result->roption == 1)
        {
            if (parse_result->moption == 1)
            {
                if (parse_result->mvalue > 1)
                {
                    RANDFLAG = 1;
                    MAXLOOPTIME = parse_result->mvalue;
                }
                else
                {
                    printf("Error, Wrrong max_loop_time value\n");
                    return 1;
                }
            }
        }
    }

    // Init struct
    pinput->RandFlag = RANDFLAG;
    pinput->MaxLoopTime = MAXLOOPTIME;
    pinput->DebugMode = DEBUGMODE;
    pinput->NotRecv = NOTRECV;

    debug(DEBUGMODE, "End parse input");
    sprintf(pdebug, "RandFlag:%d - MaxLoopTime:%d - DebugMode:%d - NotRecv:%d", pinput->RandFlag, pinput->MaxLoopTime, pinput->DebugMode, pinput->NotRecv);
    debug(DEBUGMODE, pdebug);

    work(pinput);

    free(mvalue);
    free(pinput);
    return 0;
}