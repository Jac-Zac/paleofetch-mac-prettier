//
//  macintosh.c
//  paleofetch-mac
//
//  Created by DesantBucie on 07/04/2021.
//

#include <IOKit/IOKitLib.h>
#include <ApplicationServices/ApplicationServices.h>

#include <mach/mach.h>
#include <mach/vm_page_size.h>

#include "macintosh.h"

#define OS_VERS "kern.osproductversion"

char *pgmname;
char *logo[] =
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
static char *get_kernel(const char *version)
{
        char *kernel = malloc(BUFFER64);
        strlcpy(kernel, "Darwin ", BUFFER64);
        strlcat(kernel, version, BUFFER64);
        return kernel;
        
}
static uint64_t get_mem_from_vm_stat()
{
        mach_port_t myHost;
        vm_statistics64_data_t vm_stat;
        myHost = mach_host_self();
        unsigned int count = HOST_VM_INFO64_COUNT;
        kern_return_t ret;
        if ((ret = host_statistics64(myHost, HOST_VM_INFO64, (host_info64_t)&vm_stat, &count) != KERN_SUCCESS)) {
            fprintf(stderr, "%s: failed to get statistics. error %d\n", pgmname, ret);
            exit(EXIT_FAILURE);
        }
        uint pagesize = (mach_vm_size_t)vm_kernel_page_size;
        uint64_t total = (uint64_t) (vm_stat.compressor_page_count + vm_stat.wire_count + vm_stat.active_count + vm_stat.speculative_count);
        total *= pagesize;
        total >>= 20;
        return total;
}
static char *get_ram_usage()
{
        long ram_size = get_sysctl_info_int(CTL_HW, HW_MEMSIZE);
        uint ram_size_short = ram_size >> 20;
        uint64_t used_memory = get_mem_from_vm_stat();
        char *ram_usage = malloc(BUFFER64);
        snprintf(ram_usage, BUFFER64, "%lluMB/%dMB %c%llu%s", used_memory, ram_size_short, '(', used_memory * 100/ram_size_short , "%)");
        return ram_usage;
}
static char *complete_os(const char *input)
{
        char *cmd_build = "sw_vers -buildVersion";
        char *cmd_name  = "sw_vers -productName";
        char *os        = malloc(BUFFER256);
        sprintf(os, "%s %s %s %s", get_os_name(cmd_name), get_sysctlbyname_info_str(OS_VERS), get_os_name(cmd_build), input);
        return os;
}
static char *get_resolution()
{
        uint screen_width = CGDisplayPixelsWide(CGMainDisplayID());
        uint screen_height = CGDisplayPixelsHigh(CGMainDisplayID());
        char *resolution = malloc(BUFFER64);
        snprintf(resolution, BUFFER64, "%u%c%u", screen_width, 'x', screen_height);
        return resolution;
}
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
static char *get_gpu()
{
    char *s = malloc(BUFFER256);
    snprintf(s, BUFFER256, "%s iGPU", get_sysctlbyname_info_str(CPU));
    return s;
}
#endif

#if defined(__ia64__) || defined(_IA64) || \
    defined(__IA64__) || defined(__ia64) || \
    defined(_M_IA64) || defined(__itanium__) || \
    defined(__powerpc) || defined(__powerpc__) || \
    defined(__POWERPC__) || defined(__ppc__) || \
    defined(_M_PPC) || defined(_ARCH_PPC) || \
    defined(__PPCGECKO__) || defined(__PPCBROADWAY__) || \
    defined(_XENON)
static char *get_gpu()
{
        CFMutableDictionaryRef matchDict = IOServiceMatching("IOPCIDevice");
        
        io_iterator_t iterator;
        
        if (IOServiceGetMatchingServices(kIOMasterPortDefault,matchDict,
                        &iterator) == kIOReturnSuccess)
        {
                io_registry_entry_t regEntry;
          
                while ((regEntry = IOIteratorNext(iterator))) {
                        CFMutableDictionaryRef serviceDictionary;
                        if (IORegistryEntryCreateCFProperties(regEntry,
                                                              &serviceDictionary,
                                                               kCFAllocatorDefault,
                                                               kNilOptions) != kIOReturnSuccess)
                        {
                                IOObjectRelease(regEntry);
                                continue;
                        }
                        const void *GPUModel = CFDictionaryGetValue(serviceDictionary, CFSTR("model"));
                        if (GPUModel != NULL && CFGetTypeID(GPUModel) == CFDataGetTypeID()) {
                                char *gpu_name = (char *)CFDataGetBytePtr(GPUModel);
                                /*for ( ulong i=0; i < strlen(gpu_name); i++)
                                {
                                        //Check if this is letter or space. If not, put null termination
                                        if(gpu_name[i] < 32 || (gpu_name[i] > 32 && gpu_name[i] < 48) || (gpu_name[i] > 57 && gpu_name[i] < 65) || (gpu_name[i] > 90 && gpu_name[i] < 97) || gpu_name[i] > 123 )
                                        {
                                                gpu_name[i] = '\0';
                                        }
                                }*/
                                return gpu_name;
                        }
                        CFRelease(serviceDictionary);
                        IOObjectRelease(regEntry);
                }
                IOObjectRelease(iterator);
        }
        return "Unknown";
}
#endif
