#include "../include/qdbot.h"

void initFn() {
	setPresence(2, "Made by Qunerum!", 3);
}
void _help(const char* channelId) { sendEmbed(channelId, "Help", "Description", 0, 155, 0); }

int main() {
	setLogs(1);
	setCmdPrefix('$');
	addCommand("help", _help);
	madeBot("YOUR_DISCORD_BOT_TOKEN", initFn);
	return 0;
}
