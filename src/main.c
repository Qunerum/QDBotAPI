#include "../include/qdbot.h"

void initFn() {
	setPresence(2, "Made by Qunerum!", 3);
}
void _help(const char* channelId) { sendEmbed(channelId, "Help", "Description", 0, 155, 0); }

int main() {
	setLogs(1);
	setCmdPrefix('$');
	addCommand("help", _help);
	madeBot("MTUxNzIxMTkyNDU1Mzc5NzgzMw.GQ54WB.4WPuZhri2tXTZ4xCgdThFB9Gh4A2VTGI6thWh4", initFn);
	return 0;
}
