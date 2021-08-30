//
//  macintosh.c
//  paleofetch-mac
//
//  Created by DesantBucie on 07/04/2021.
//
#include <mach/host_info.h>
#include <mach/mach_host.h>
#include <mach/task_info.h>
#include <mach/task.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFDictionary.h>
#include <SystemConfiguration/SystemConfiguration.h>
#include <ApplicationServices/ApplicationServices.h>

struct color_numbers darwin_colors = {76,184};
char *logo[] =
{
"\e[38;5;76;1m                    'c.        ",
"\e[38;5;76;1m                 ,xNMM.        ",
"\e[38;5;76;1m               .OMMMMo         ",
"\e[38;5;76;1m               OMMM0,          ",
"\e[38;5;76;1m     .;loddo:' loolloddol;.    ",
"\e[38;5;76;1m   cKMMMMMMMMMMNWMMMMMMMMMM0:  ",
"\e[38;5;184;1m .KMMMMMMMMMMMMMMMMMMMMMMMWd.  ",
"\e[38;5;184;1m XMMMMMMMMMMMMMMMMMMMMMMMX.    ",
"\e[38;5;208;1m;MMMMMMMMMMMMMMMMMMMMMMMM:     ",
"\e[38;5;208;1m:MMMMMMMMMMMMMMMMMMMMMMMM:     ",
"\e[38;5;196;1m.MMMMMMMMMMMMMMMMMMMMMMMMX.    ",
"\e[38;5;196;1m kMMMMMMMMMMMMMMMMMMMMMMMMWd.  ",
"\e[38;5;129;1m .XMMMMMMMMMMMMMMMMMMMMMMMMMMk ",
"\e[38;5;129;1m  .XMMMMMMMMMMMMMMMMMMMMMMMMK. ",
"\e[38;5;45;1m    kMMMMMMMMMMMMMMMMMMMMMMd   ",
"\e[38;5;45;1m     ;KMMMMMMMWXXWMMMMMMMk.    ",
"\e[38;5;45;1m       .cooc,.    .,coo:.      ",
"\e[0m                               "
};
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
                        if (GPUModel != NULL) {
                                if (CFGetTypeID(GPUModel) == CFDataGetTypeID()) {
                                        char *gpu_name = (char *)CFDataGetBytePtr(GPUModel);
                                        return gpu_name;
                                }
                        }
                        CFRelease(serviceDictionary);
                        IOObjectRelease(regEntry);
                }
                IOObjectRelease(iterator);
        }
    return "";
}
