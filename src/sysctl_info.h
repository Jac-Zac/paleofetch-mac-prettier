//
//  sysctl_info.h 
//  paleofetch-mac
//
//  Created by DesantBucie on 14/11/2021.
//

#ifndef SYSCTL_INFO_H
#define SYSCTL_INFO_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <errno.h>

#define CPU "machdep.cpu.brand_string"
#define MEM_SIZE "hw.memsize"
#define HOSTNAME "kern.hostname"
#define PAGES "vm.pages"
#define LOGICAL_CPU "hw.logicalcpu"
#define MODEL "hw.model"
#define BUFFER32 32 * sizeof(char)
#define BUFFER64 64 * sizeof(char)
#define BUFFER256 256 * sizeof(char)
#define BUFFER512 512 * sizeof(char)
#define halt_and_catch_fire(fmt, status) \
        do { \
                if(status != 0) { \
                        fprintf(stderr, "paleofetch: " fmt "\n"); \
                        exit(status); \
                } \
        } while(0)

struct utsname details;
void *get_sysctl_info(const int, const int);

char *get_sysctlbyname_info_str(const char*);  
#endif // SYSCTL_INFO_H
