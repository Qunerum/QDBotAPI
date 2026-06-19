#include "../include/qdbot.h"

void _help() {

}

int main() {
	setLogs(1);
	setCmdPrefix('!');
	addCommand("help", _help);
	madeBot("YOUR_DISCORD_BOT_TOKEN");
	return 0;
}
