#ifndef QDBOT_H
#define QDBOT_H

#define MAX_COMMANDS 128

void setLogs(int state);
void madeBot(char* token, void (*initFunc)());
void setPresence(int status, const char *activity_name, int type);

void sendMsg(const char* channel_id, const char *text);
void sendEmbed(const char *channel_id, const char *title, const char *description, int r, int g, int b);

void setCmdPrefix(char prefix);
void addCommand(char* command, void (*function)(const char* channelId));

static inline int clamp(int value, int min, int max) { return value > max ? max : value < min ? min : value; }

#endif
