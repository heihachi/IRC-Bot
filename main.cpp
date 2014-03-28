#include "inc/config.h"
#include "inc/bot.h"

// set the global config variable
struct config
{
    // using
    string server = "";
    int    port = 0;
    string userName = "";
    string nickName = "";
    string pass = "";
    string channelPass = "";
    string serverPass = "";
};

int main()
{
    configuration(config);
    bot(config);
    return 0;
}

