//
//  macintosh.c
//  paleofetch-mac
//
//  Created by DesantBucie on 07/04/2021.
//
#include <IOKit/IOKitLib.h>
#include <Availability.h>
#include <ApplicationServices/ApplicationServices.h>

#include <mach/mach.h>
#include <mach/vm_page_size.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "sysctl_info.h"
#include "macintosh.h"

#define OS_VERS "kern.osproductversion"

char *const pgmname;

char *execute_command(char const *const cmd)
{
        FILE *stdout_file = popen(cmd, "r");
        char *os_name = malloc(BUFF_256);
        if(os_name == NULL){
            pclose(stdout_file);
            halt_and_catch_fire("Malloc error", 127);
        }
        if (stdout_file)
        {
                fgets(os_name, BUFF_256, stdout_file);
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
void get_kernel(char *kernel)
{
        //char *const kernel = malloc_s(BUFF_64);
        strlcpy(kernel, "Darwin ", BUFF_64);
        strlcat(kernel, details.release, BUFF_64);
        //return kernel;
}
uint64_t get_mem_from_vm_stat()
{
        mach_port_t myHost;
        vm_statistics64_data_t const vm_stat;
        myHost = mach_host_self();
        uint count = HOST_VM_INFO64_COUNT;
        kern_return_t ret;
        if ((ret = host_statistics64(myHost, HOST_VM_INFO64, (host_info64_t)&vm_stat, &count) != KERN_SUCCESS)) {
            fprintf(stderr, "%s: failed to get statistics. error %d\n", pgmname, ret);
            exit(EXIT_FAILURE);
        }
        uint const pagesize = (mach_vm_size_t)vm_kernel_page_size;
        uint64_t total = (uint64_t) (vm_stat.compressor_page_count + vm_stat.wire_count + vm_stat.active_count + vm_stat.speculative_count);
        total *= pagesize;
        total >>= 20;
        return total;
}
#if defined(_is_arm_) || defined(_is_x86_64_)
void get_ram_usage(char *ram_usage)
{
        int64_t *const ram_size =(int64_t *)get_sysctl_info(CTL_HW, HW_MEMSIZE);
        uint const ram_size_short = ram_size[0] >> 20;
        uint64_t const used_memory = get_mem_from_vm_stat();
        //char *const ram_usage = malloc_s(BUFF_64);
        snprintf(ram_usage, BUFF_64, "%lluMB/%dMB %c%llu%s",
                used_memory, ram_size_short, '(', used_memory * 100/(ram_size_short != 0 ? ram_size_short : 1) , "%)");
        //return ram_usage;
}
#else
char *get_ram_usage()
{
        uint32_t *const ram_size =(uint32_t *)get_sysctl_info(CTL_HW, HW_MEMSIZE);
        uint const ram_size_short = ram_size[0] >> 20;
        uint32_t const used_memory = get_mem_from_vm_stat();
        char *const ram_usage = malloc_s(BUFF_64);
        snprintf(ram_usage, BUFF_64, "%lluMB/%dMB %c%llu%s",
                used_memory, ram_size_short, '(', used_memory * 100/(ram_size_short != 0 ? ram_size_short : 1) , "%)");
        return ram_usage;
}


#endif
void get_complete_os(char *os)
{
        char const cmd_build[] = "sw_vers -buildVersion";
        char const cmd_name[]  = "sw_vers -productName";
        char *build   = execute_command(cmd_build);
        char *name    = execute_command(cmd_name);
        char *version = get_sysctlbyname_info_str(OS_VERS);
        sprintf(os, "%s %s %s %s", name, version, build, details.machine);
        free(name); free(build); free(version);
        build = NULL; name = NULL; version = NULL;
}
void get_battery_procentage(char *battery_procentage)
{
    char const cmd[] = "pmset -g batt | grep -Eo '\134d+\045'";
    char *battery = execute_command(cmd);
    //char *battery = iokit_info("AppleSmartBattery");
    if(battery == NULL){
        strcpy(battery_procentage, "Unknown");
        return;
    }
    strcpy(battery_procentage, battery);
}
void get_resolution(char *resolution)
{
        uint const screen_width = CGDisplayPixelsWide(CGMainDisplayID());
        uint const screen_height = CGDisplayPixelsHigh(CGMainDisplayID());

        snprintf(resolution, BUFF_64, "%u%c%u", screen_width, 'x', screen_height);
}
#if defined(_is_arm_)
void get_gpu(char *s)
{
    char *cpu = get_sysctlbyname_info_str(CPU);
    snprintf(s, BUFF_256, "%s SoC GPU", cpu);
    free(cpu);
    cpu = NULL;
}
#else
void get_gpu(char *gpu)
{
    char *gpu_name;
    CFMutableDictionaryRef matchDict = IOServiceMatching("IOPCIDevice");
        
    io_iterator_t iterator;
    if (IOServiceGetMatchingServices(
#ifdef __MAC_OS_X_VERSION_MAX_ALLOWED
#if __MAC_OS_X_VERSION_MAX_ALLOWED < 120000
                kIOMasterPortDefault,
#else
                kIOMainPortDefault,
#endif
#endif
                matchDict,
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
                    gpu_name = (char *)CFDataGetBytePtr(GPUModel);
                    strlcpy(gpu, gpu_name, 256);
            }
            else {
                strcpy(gpu, "Unknown");
            }
            CFRelease(serviceDictionary);
            IOObjectRelease(regEntry);
        }
        IOObjectRelease(iterator);

    } 
}
#endif
