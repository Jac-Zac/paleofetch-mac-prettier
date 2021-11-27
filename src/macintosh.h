//
//  macintosh.h 
//  paleofetch-mac
//
//  Created by DesantBucie on 16/09/2021.
//
#ifndef MACINTOSH_H
#define MACINTOSH_H
#include <stdint.h>

char        *get_os_name(const char*),
            *get_ram_usage(void),
            *get_kernel(void),
            *complete_os(void),
            *get_resolution(void),
            *get_gpu(void);

uint64_t get_mem_from_vm_stat();

#endif //MACINTOSH_H
