//
//  macintosh.h 
//  paleofetch-mac
//
//  Created by DesantBucie on 16/09/2021.
//
#ifndef MACINTOSH_H
#define MACINTOSH_H
#include <stdint.h>

char        *execute_command(char const *const),
            *iokit_info(char *);

void        get_ram_usage(char *),
            get_kernel(char *),
            get_complete_os(char *),
            get_resolution(char *),
            get_gpu(char *),
            get_battery_procentage(char *);

uint64_t get_mem_from_vm_stat();

#if \
    defined(__ARM_ARCH) || defined(__TARGET_ARCH_ARM) || \
    defined(__TARGET_ARCH_THUMB) || defined(_M_ARM) || \
    defined(__arm__) || defined(__arm64) || defined(__thumb__) || \
    defined(_M_ARM64) || defined(__aarch64__) || defined(__AARCH64EL__) || \
    defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || \
    defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || \
    defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || \
    defined(__ARM_ARCH_6KZ__) || defined(__ARM_ARCH_6T2__) || \
    defined(__ARM_ARCH_5TE__) || defined(__ARM_ARCH_5TEJ__) || \
    defined(__ARM_ARCH_4T__) || defined(__ARM_ARCH_4__)
#define _is_arm_
#elif \
    defined(__x86_64) || defined(__x86_64__) || \
    defined(__amd64__) || defined(__amd64) || \
    defined(_M_X64) 
#define _is_x86_64_
#elif \
    defined(i386) || defined(__i386__) \
    defined(__i486__) || defined(__i568__) \
    defined(__i686__) || defined(__i386) \
    defined(_M_IX86) || defined(_X86_) \
    defined(__THW_INTEL__) || defined(__I86__) \
    defined(__INTEL__)
#define _is_x86
#else 
#define _is_ppc_
#endif  

#endif //MACINTOSH_H
