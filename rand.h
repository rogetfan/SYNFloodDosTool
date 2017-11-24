/* File: rand.h
 * Auth: isinstance
 * Date: 20171031
 * Mail: no
 */

#ifndef RAND_H
#define RAND_H

#define MY_RAND_MAX_USERNAME_LENGTH 1024
#define MY_RAND_MAX_PASSWORD_LENGTH 1024

struct RAND_INPUT
{
    int Flag;
    int NumLoop;
    int Seed;
    int DebugMode;
};

void rand_string(char *rebuf, const struct RAND_INPUT *input);

#endif