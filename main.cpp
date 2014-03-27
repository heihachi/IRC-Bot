#include "inc/config.h"
#include "inc/bot.h"

// set the global config variable
string config[8][2] = { };

int main()
{
    configuration(config);
    bot(config);
    return 0;
}

