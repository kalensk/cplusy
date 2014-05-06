#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <arpa/inet.h>



void create_connection(const char* irc_server, const char* port) {

    // getaddrinfo()
    struct addrinfo *resolved_addr = NULL;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(irc_server, port, &hints, &resolved_addr);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }


    for (struct addrinfo *addr = resolved_addr; addr != NULL; addr = addr->ai_next) {

        // printout ip's returned by getaddrinfo
        char ipstr[INET6_ADDRSTRLEN];
        void *addrstuffs;
        char *ipver;
        if (addr->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr->ai_addr;
            addrstuffs = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)addr->ai_addr;
            addrstuffs = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(addr->ai_family, addrstuffs, ipstr, sizeof ipstr);
        printf("  %s: %s\n", ipver, ipstr);





        // socket()
        int fd = socket(addr->ai_family, SOCK_STREAM, 0);
        if (fd < 0) {
            fprintf(stderr, "socket() error: %s\n", gai_strerror(fd));
        }

        // connect()
        status = connect(fd, addr->ai_addr, addr->ai_addrlen);
        if (status < 0) {
            fprintf(stderr, "connect() error: %s\n", gai_strerror(status));
        }
    }

    freeaddrinfo(resolved_addr);


}

int main(int argc, char** argv) {



    printf("Hello world\n");
    create_connection("irc.mzima.net", "6665");
}


