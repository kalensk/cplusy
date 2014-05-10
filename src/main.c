#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <errno.h>
#include <arpa/inet.h>

#include "main.h"


NameInfo* create_nameInfo(const char* nick, const char* realName) {
    NameInfo* name = (NameInfo*) malloc(sizeof(NameInfo));

    name->nick = nick;
    name->realName = realName;

    return name;
}

ServerInfo* create_serverInfo(const char* server, const char* port) {
    ServerInfo* serverObj = (ServerInfo*) malloc(sizeof(ServerInfo));

    serverObj->name = server;
    serverObj->port = port;

    return serverObj;
}

void create_connection(Irc* irc) {
    // getaddrinfo()
    struct addrinfo *resolved_addr = NULL;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(irc->server->name, irc->server->port, &hints, &resolved_addr);
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
        int fd = irc->server->fd = socket(addr->ai_family, SOCK_STREAM, 0);
        if (fd < 0) {
            fprintf(stderr, "socket() error: %s\n", gai_strerror(fd));
            exit(1);
        }

        // connect()
        printf("status before connect() = %d\n", status);
        printf("socket fd = %d\n", fd);
        status = connect(irc->server->fd, addr->ai_addr, addr->ai_addrlen);
        if (status < 0) {
            printf("status after connect() = %d with errorno => %s\n", status, strerror(errno));
            fprintf(stderr, "connect() error: %s\n", gai_strerror(status));
            shutdown(fd, 2);
            printf("close socket fd = %d\n", fd);
            continue;
        }
        startup_sequence(irc);
    }

    freeaddrinfo(resolved_addr);


}

void startup_sequence(Irc* irc) {
    char buffer[512] = {0};

    send_command_with_arg(irc, "NICK", irc->name->nick);

    sprintf(buffer, "%s %s %s :%s", irc->name->nick, irc->name->nick, irc->server->name, irc->name->realName);
    send_command_with_arg(irc, "USER", buffer); //USER nick + nick + server + : + realName
}

void send_command(Irc* irc, const char* command) {
    send_command_with_arg(irc, command, NULL);
}

void send_command_with_arg(Irc* irc, const char* command, const char* arg) {
    send(irc->server->fd, command, strlen(command), 0);
    send(irc->server->fd, arg, strlen(arg), 0);
    send(irc->server->fd, "\r\n", 2, 0);
}



int main(int argc, char** argv) {
    NameInfo* name;
    ServerInfo* server;
    Irc* irc;
    
    printf("Hello world\n");

    name = create_nameInfo("plusy", "plus one bot");
    server = create_serverInfo("irc.he.net", "6667");

    irc = (Irc*) malloc(sizeof(Irc));
    irc->server = server;
    irc->name = name;

    create_connection(irc);
}


