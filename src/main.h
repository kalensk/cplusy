#ifndef MAIN
#define MAIN

typedef struct {
    int fd;
    const char* name;
    const char* port;
} ServerInfo;


typedef struct {
    const char* nick;
    const char* realName;
} NameInfo;

typedef struct {
    ServerInfo* server;
    NameInfo* name;
} Irc;


NameInfo* create_nameInfo(const char* nick, const char* realName);
ServerInfo* create_serverInfo(const char* server, const char* port);
void create_connection(Irc* irc);
void startup_sequence(Irc* irc);
void send_command(Irc* irc, const char* command);
void send_command_with_arg(Irc* irc, const char* command, const char* arg);

#endif // main.h
