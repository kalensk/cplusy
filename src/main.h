#ifndef MAIN
#define MAIN

typedef struct {
    int fd;
    const char* name;
    const char* port;
    const char* channel;
} ServerInfo;

typedef struct {
    const char* nick;
    const char* realName;
} NameInfo;

typedef struct {
    ServerInfo* server;
    NameInfo* name;
    FILE* file_ptr;
} Irc;

typedef enum {
    ADMIN,
    AWAY,
    CNOTICE,
    CPRIVMSG,
    CONNECT,
    DIE,
    ENCAP,
    ERROR,
    HELP,
    INFO,
    INVITE,
    ISON,
    JOIN,
    KICK,
    KILL,
    KNOCK,
    LINKS,
    LIST,
    LUSERS,
    MODE,
    MOTD,
    NAMES,
    NAMESX,
    NICK,
    NOTICE,
    OPER,
    PART,
    PASS,
    PING,
    PONG,
    PRIVMSG,
    QUIT,
    REHASH,
    RESTART,
    RULES,
    SERVER,
    SERVICE,
    SERVLIST,
    SQUERY,
    SQUIT,
    SETNAME,
    SILENCE,
    STATS,
    SUMMON,
    TIME,
    TOPIC,
    TRACE,
    UHNAMES,
    USER,
    USERHOST,
    USERIP,
    USERS,
    VERSION,
    WALLOPS,
    WATCH,
    WHO,
    WHOIS,
    WHOWAS
} Command;

NameInfo* create_nameInfo(const char* nick, const char* realName);
ServerInfo* create_serverInfo(const char* server, const char* port);
void create_connection(Irc* irc);
void startup_sequence(Irc* irc);
void send_command(Irc* irc, const char* command);
void send_command_with_arg(Irc* irc, const char* command, const char* arg);

#endif // main.h
