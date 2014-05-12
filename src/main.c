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

// globals
int fd = 0;
FILE* file_ptr = NULL;

NameInfo* create_nameInfo(const char* nick, const char* realName) {
    NameInfo* name = (NameInfo*) malloc(sizeof(NameInfo));

    name->nick = nick;
    name->realName = realName;

    return name;
}

ServerInfo* create_serverInfo(const char* server, const char* port, const char* channel) {
    ServerInfo* serverObj = (ServerInfo*) malloc(sizeof(ServerInfo));

    serverObj->name = server;
    serverObj->port = port;
    serverObj->channel = channel;

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
        fd = socket(addr->ai_family, SOCK_STREAM, 0);
        if (fd < 0) {
            fprintf(stderr, "socket() error: %s\n", gai_strerror(fd));
            exit(1);
        }

        // connect()
        printf("status before connect() = %d\n", status);
        printf("socket fd = %d\n", fd);
        status = connect(fd, addr->ai_addr, addr->ai_addrlen);
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

    send_command_with_arg("NICK", irc->name->nick);

    sprintf(buffer, "%s 0 * :%s", irc->name->nick, irc->name->realName);
    send_command_with_arg("USER", buffer); //USER nick 0 * : realName

    send_command_with_arg("JOIN", irc->server->channel);
}

void send_command(const char* command) {
    send_command_with_arg(command, NULL);
}

void send_command_with_arg(const char* command, const char* arg) {
    send_command_with_arg_and_colon(command, arg, NULL);
}

void send_command_with_arg_and_colon(const char* command, const char* arg, const char* colon) {
    send(fd, command, strlen(command), 0);
    send(fd, arg, strlen(arg), 0);
    send(fd, colon, strlen(colon), 0);
    send(fd, "\r\n", 2, 0);
}

void read_msg() {
    char buffer[512] = {0};

    recv(fd, buffer, strlen(buffer), 0);

}

char* split_msg(char* msg) {
    char* whitespace = "' '";
    char* token = NULL;
    char* tokens[2] = (char*) malloc(sizeof(char) * 3);
    tokens = {0, 0, 0};
    int i = 0;

    token = strtok(msg, whitespace);

    while(token != NULL) {
        tokens[i] = malloc(strlen(token));
        tokens[i] = token;
        token = strtok(NULL, whitespace);
        i++;
    }

    return tokens;
}


void parse_command(char* tokens) {
    char* command = tokens;
    char* arg = tokens[1];
    char* colon = tokens[2];

    if (strcmp(command, "PING\0")) {
        pong(arg);
    }
}

void pong(char* arg) {
    char msg[512];
    sprintf(msg, ":%s", arg);
    send_command("PONG", msg);
}

void PRIVMSG(char* target, char* msg) {
    // PRIVMSG #Test :hi I am in test channel test test
    send_command_with_arg_and_colon("PRIVMSG", target, msg);
}

void write_to_file(Irc* irc, char* msg) {
    char* file_name;
    sprintf(file_name, "%s.log", irc->server->channel);

    file_ptr = fopen(file_name, "w");
    
    if (file_ptr != NULL) {
        fputs(msg, file_ptr);
    }
}

void close_file() {
    fclose(file_ptr);
}


void write_to_stdout(char* msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
}


int main(int argc, char** argv) {
    NameInfo* name;
    ServerInfo* server;
    Irc* irc;
    
    printf("Hello world\n");

    name = create_nameInfo("plusy", "plus one bot");
    server = create_serverInfo("irc.he.net", "6667",  "#not-world");

    irc = (Irc*) malloc(sizeof(Irc));
    irc->server = server;
    irc->name = name;

    create_connection(irc);

    free(irc) ;
}


