#include <stdio.h>

int main()
{
        char *logo[] =
        {
        "                    'c.        ",
        "                 ,xNMM.        ",
        "               .OMMMMo         ",
        "               OMMM0,          ",
        "     .;loddo:' loolloddol;.    ",
        "   cKMMMMMMMMMMNWMMMMMMMMMM0:  ",
        " .KMMMMMMMMMMMMMMMMMMMMMMMWd.  ",
        " XMMMMMMMMMMMMMMMMMMMMMMMX.    ",
        ";MMMMMMMMMMMMMMMMMMMMMMMM:     ",
        ":MMMMMMMMMMMMMMMMMMMMMMMM:     ",
        ".MMMMMMMMMMMMMMMMMMMMMMMMX.    ",
        " kMMMMMMMMMMMMMMMMMMMMMMMMWd.  ",
        " .XMMMMMMMMMMMMMMMMMMMMMMMMMMk ",
        "  .XMMMMMMMMMMMMMMMMMMMMMMMMK. ",
        "    kMMMMMMMMMMMMMMMMMMMMMMd   ",
        "     ;KMMMMMMMWXXWMMMMMMMk.    ",
        "       .cooc,.    .,coo:.      ",
        "                               "
        };
        for(int i = 0; i < 18; i++)
        {
                printf("%s\n", logo[i]);        
        }
}

