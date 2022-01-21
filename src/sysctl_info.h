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
#define BUFF_32 32
#define BUFF_64 64
#define BUFF_256 256
#define BUFF_512 512

#define halt_and_catch_fire(fmt, status) \
    if(status != 0) { \
                fprintf(stderr, "paleofetch: " fmt "\n"); \
                exit(status); \
    } 
struct utsname details;

void *get_sysctl_info(int const, int const),
     *malloc_s(size_t const),
     free_s(void *);

char *get_sysctlbyname_info_str(char const *const);  
#endif // SYSCTL_INFO_H
