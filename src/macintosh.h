//
//  macintosh.h 
//  paleofetch-mac
//
//  Created by DesantBucie on 16/09/2021.
//
#ifndef MACINTOSH_H
#define MACINTOSH_H

const char *logo[] =
{
"\033[38;5;76;1m                    'c.        ",
"\033[38;5;76;1m                 ,xNMM.        ",
"\033[38;5;76;1m               .OMMMMo         ",
"\033[38;5;76;1m               OMMM0,          ",
"\033[38;5;76;1m     .;loddo:' loolloddol;.    ",
"\033[38;5;76;1m   cKMMMMMMMMMMNWMMMMMMMMMM0:  ",
"\033[38;5;184;1m .KMMMMMMMMMMMMMMMMMMMMMMMWd.  ",
"\033[38;5;184;1m XMMMMMMMMMMMMMMMMMMMMMMMX.    ",
"\033[38;5;208;1m;MMMMMMMMMMMMMMMMMMMMMMMM:     ",
"\033[38;5;208;1m:MMMMMMMMMMMMMMMMMMMMMMMM:     ",
"\033[38;5;196;1m.MMMMMMMMMMMMMMMMMMMMMMMMX.    ",
"\033[38;5;196;1m kMMMMMMMMMMMMMMMMMMMMMMMMWd.  ",
"\033[38;5;129;1m .XMMMMMMMMMMMMMMMMMMMMMMMMMMk ",
"\033[38;5;129;1m  .XMMMMMMMMMMMMMMMMMMMMMMMMK. ",
"\033[38;5;45;1m    kMMMMMMMMMMMMMMMMMMMMMMd   ",
"\033[38;5;45;1m     ;KMMMMMMMWXXWMMMMMMMk.    ",
"\033[38;5;45;1m       .cooc,.    .,coo:.      ",
"\033[0m                               "
};
static char *get_os_name(const char*),
            *get_ram_usage(void),
            *get_kernel(void),
            *complete_os(void),
            *get_resolution(void),
            *get_gpu(void);

static uint64_t get_mem_from_vm_stat();

#endif
