/* File: rand.h
 * Auth: isinstance
 * Date: 20171031
 * Mail: no
 */

#ifndef RAND_H
#define RAND_H

#define MY_RAND_MAX_USERNAME_LENGTH 128
#define MY_RAND_MAX_PASSWORD_LENGTH 128

struct RAND_INPUT
{
    int RandFlag;
    int NumLoop;
    int Seed;
    int DebugMode;
};

struct GINPUT
{
    float Seed;
    float Max;
};

void rand_string(char *rebuf, const struct RAND_INPUT *input);
void rand_ip(char *rebuf);
int rand_port(void);

#endif