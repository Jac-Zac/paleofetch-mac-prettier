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
    defined(__ia64__) || defined(_IA64) || \
    defined(__IA64__) || defined(__ia64) || \
    defined(_M_IA64) || defined(__itanium__) || \
    defined(__powerpc) || defined(__powerpc__) || \
    defined(__POWERPC__) || defined(__ppc__) || \
    defined(_M_PPC) || defined(_ARCH_PPC) || \
    defined(__PPCGECKO__) || defined(__PPCBROADWAY__) || \
    defined(_XENON)
#define _is_ppc_or_intel_
#endif 

#endif //MACINTOSH_H
