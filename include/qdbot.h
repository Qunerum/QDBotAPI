#ifndef QDBOT_H
#define QDBOT_H

#define MAX_COMMANDS 128

void setLogs(int state);
void madeBot(char* token);
void sendMsg(const char *channel_id, const char *text);
void setCmdPrefix(char prefix);
void addCommand(char* command, void (*function)());

#endif
