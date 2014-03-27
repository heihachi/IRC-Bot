/*

*/

#include "config.h"

bool configuration(string config[8][2])
{
    // a 2d array containing server info and what they are
    // server
    config[0][0] = "server";
    config[0][1] = "irc.truewow.org";
    // port
    config[1][0] = "port";
    config[1][1] = "6667";
    // channel
    config[2][0] = "channel";
    config[2][1] = "#bot";
    // name
    config[3][0] = "name";
    config[3][1] = "Bot";
    // nick
    config[4][0] = "nick";
    config[4][1] = "TestBot";
    // nick password doesnt need to be set
    config[5][0] = "pass";
    config[5][1] = "";
    // channel password usually not set
    config[6][0] = "chanpass";
    config[6][1] = "";
    // server password
    config[7][0] = "serverpass";
    config[7][1] = "";
    return true;
}
