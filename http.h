/* File : http.h 
 * Auth : sjin 
 * Date : 20141206 
 * Mail : 413977243@qq.com 
 * -----------------------
 * Rewrite
 * Auth: isinstance
 * Date: 20171031
 * Mail: no
 */

#ifndef HTTP_H
#define HTTP_H

#define MY_HTTP_DEFAULT_PORT 80
#define MY_HTTP_DEFAULT_RESPONSE_LENGTH 1024
#define MY_HTTP_DEFAULT_SEND_BUF 10240
//#define MY_HTTP_DEFAULT_POST_LENGTH 8192
// 2017-11-03 add timeout define
//#define MY_HTTP_DEFAULT_TIMEOUT 3

struct HTTP_POST_ARG
{
    char *URL;
    char *PostData;
    char *ReturnStr;
    int DebugMode;
    int NotRecv;
};

#define HTTP_POST "POST /%s HTTP/1.1\r\n"                              \
                  "HOST: %s:%d\r\n"                                    \
                  "Accept: */*\r\n"                                    \
                  "Content-Type:application/x-www-form-urlencoded\r\n" \
                  "Content-Length: %lu\r\n\r\n"                        \
                  "%s"

#define HTTP_GET "GET /%s HTTP/1.1\r\n" \
                 "HOST: %s:%d\r\n"      \
                 "Accept: */*\r\n\r\n"

//char *http_get(const char *url, int debug_mode, int not_recv);
void http_post(const struct HTTP_POST_ARG *input);
int http_judge(const char *inbuf, int debug_mode);

#endif