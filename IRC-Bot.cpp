#include "inc/bot.h"

int main()
{
	Config config;
	IrcBot bot = IrcBot(config);
	bot.bot();

    return 0;
}

