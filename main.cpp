#include "inc/config.h"

bool bot(string[][2]);

// set the global config variable
string config[8][2] = { };

int main()
{
    configuration(config);
    bot(config);
    return 0;
}

bool bot(string config[8][2])
{
    for(int x = 0;x<8;x++)
        for(int y = 0;y<2;y++)
            printf("%-25s\n", config[x][y].c_str());
    return true;
}

