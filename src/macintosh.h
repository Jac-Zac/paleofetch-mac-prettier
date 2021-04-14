#include <mach/host_info.h>
#include <mach/mach_host.h>
#include <mach/task_info.h>
#include <mach/task.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <SystemConfiguration/SystemConfiguration.h>

static char *get_sysctlbyname_info_str(const char *input)
{
        char *sysctl_info;
        size_t sysctl_info_length;
        sysctlbyname(input, NULL, &sysctl_info_length, NULL, 0);
        sysctl_info = malloc(sysctl_info_length);
        int n = sysctlbyname(input, sysctl_info, &sysctl_info_length, NULL, 0);
        if (n != 0) 
        {
                perror("sysctlbyname error");
                return 0;
        }
        return sysctl_info;
}
