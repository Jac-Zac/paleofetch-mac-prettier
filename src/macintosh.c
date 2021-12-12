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
#include <stdint.h>

#include "sysctl_info.h"
#include "macintosh.h"

#define OS_VERS "kern.osproductversion"

char *pgmname;

char *get_os_name(const char *cmd)
{
        FILE *stdout_file = popen(cmd, "r");
        char *os_name = malloc(8);
        if (stdout_file)
        {
                fgets(os_name, 8, stdout_file);
                pclose(stdout_file);
        }
        for (uint i = strlen(os_name); i != 0; i--)
        {
                if(os_name[i] == '\n')
                {
                        os_name[i] = '\0';
                        break;
                }      
        }
        return os_name;

}
char *get_kernel()
{
        char *kernel = malloc(BUFFER64);
        strlcpy(kernel, "Darwin ", BUFFER64);
        strlcat(kernel, details.release, BUFFER64);
        return kernel;
        
}
uint64_t get_mem_from_vm_stat()
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

char *get_ram_usage()
{
        int64_t *ram_size =(int64_t *)get_sysctl_info(CTL_HW, HW_MEMSIZE);
        uint ram_size_short = ram_size[0] >> 20;
        uint64_t used_memory = get_mem_from_vm_stat();
        char *ram_usage = malloc(BUFFER64);
        snprintf(ram_usage, BUFFER64, "%lluMB/%dMB %c%llu%s",
                used_memory, ram_size_short, '(', used_memory * 100/(ram_size_short != 0 ? ram_size_short : 1) , "%)");
        return ram_usage;
}
char *complete_os()
{
        char *cmd_build = "sw_vers -buildVersion";
        char *cmd_name  = "sw_vers -productName";
        char *os        = malloc(BUFFER256);
        sprintf(os, "%s %s %s %s", get_os_name(cmd_name), get_sysctlbyname_info_str(OS_VERS), get_os_name(cmd_build), details.machine);
        return os;
}
char *get_resolution()
{
        uint screen_width = CGDisplayPixelsWide(CGMainDisplayID());
        uint screen_height = CGDisplayPixelsHigh(CGMainDisplayID());
        char *resolution = malloc(BUFFER64);
        snprintf(resolution, BUFFER64, "%u%c%u", screen_width, 'x', screen_height);
        return resolution;
}
#if defined(_is_arm_)
char *get_gpu()
{
    char *s = malloc(BUFFER256);
    snprintf(s, BUFFER256, "%s SoC GPU", get_sysctlbyname_info_str(CPU));
    return s;
}
#else
char *get_gpu()
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
