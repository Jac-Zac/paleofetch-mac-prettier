#include <stdio.h>

int main()
{
        char *logo[] =
        {
        "\e[38;5;76;1m                    'c.        ",
        "                 ,xNMM.        ",
        "               .OMMMMo         ",
        "               OMMM0,          ",
        "     .;loddo:' loolloddol;.    ",
        "   cKMMMMMMMMMMNWMMMMMMMMMM0:  ",
        " \e[38;5;184m.KMMMMMMMMMMMMMMMMMMMMMMMWd.  ",
        " XMMMMMMMMMMMMMMMMMMMMMMMX.    ",
        "\e[38;5;208m;MMMMMMMMMMMMMMMMMMMMMMMM:     ",
        ":MMMMMMMMMMMMMMMMMMMMMMMM:     ",
        "\e[38;5;196m.MMMMMMMMMMMMMMMMMMMMMMMMX.    ",
        " kMMMMMMMMMMMMMMMMMMMMMMMMWd.  ",
        " \e[38;5;129m.XMMMMMMMMMMMMMMMMMMMMMMMMMMk ",
        "  .XMMMMMMMMMMMMMMMMMMMMMMMMK. ",
        "    \e[38;5;39mkMMMMMMMMMMMMMMMMMMMMMMd   ",
        "     ;KMMMMMMMWXXWMMMMMMMk.    ",
        "       .cooc,.    .,coo:.      ",
        "\e[0m                               "
        };
        for(int i = 0; i < 18; i++)
        {
                printf("%s\n", logo[i]);        
        }
}

