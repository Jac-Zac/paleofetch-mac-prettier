//
//  macintosh.c
//  paleofetch-mac
//
//  Created by DesantBucie on 07/04/2021.
//

#include <IOKit/IOKitLib.h>
#include <ApplicationServices/ApplicationServices.h>
#include "macintosh.h"

#define OS_VERS "kern.osproductversion"

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
static uint get_mem_from_vm_stat()
{
        mach_msg_type_number_t count = HOST_VM_INFO_COUNT;

        vm_statistics64_data_t vmstat;
        if (host_statistics (mach_host_self(), HOST_VM_INFO, (host_info_t) &vmstat, &count) != KERN_SUCCESS)
        {
                halt_and_catch_fire("Failed to get VM statistics.", EXIT_FAILURE);
        }
        // You may have noticed that USED RAM amount is not exactly the same as in activity
        // monitor, but i cannot really find a way to make this perfectly right. About +-100MB could be wrong.
        uint total = (vmstat.compressor_page_count + vmstat.wire_count + vmstat.active_count + vmstat.speculative_count) /1024 * 4;
        return total;
}
static char *get_ram_usage()
{
        long ram_size = get_sysctl_info_int(CTL_HW, HW_MEMSIZE);
        uint ram_size_short = ram_size / (1024*1024);
        long used_memory = get_mem_from_vm_stat();
        char *ram_usage = malloc(BUFFER64);
        snprintf(ram_usage, BUFFER64, "%ldMB/%dMB %c%ld%s", used_memory, ram_size_short, '(', used_memory * 100/ram_size_short , "%)");
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
