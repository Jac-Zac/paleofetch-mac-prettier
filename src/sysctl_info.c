//
//  sysctl_info.c 
//  paleofetch-mac
//
//  Created by DesantBucie on 14/11/2021.
//

#include "sysctl_info.h"
#include <sys/_types/_va_list.h>
#include <stdarg.h>

char *get_sysctlbyname_info_str(char const *const input)
{
        
        size_t sysctl_info_length;
        sysctlbyname(input, NULL, &sysctl_info_length, NULL, 0);
        char *const sysctl_info = malloc(sysctl_info_length);
        int n = sysctlbyname(input, sysctl_info, &sysctl_info_length, NULL, 0);
        if (n != 0)
        {
                halt_and_catch_fire("sysctlbyname error", EXIT_FAILURE);
        }
        return sysctl_info;
}
void *get_sysctl_info(int const input1, int const input2)
{
        int mib[2] = {input1, input2};
        size_t sysctl_info_lenght;
        sysctl(mib, 2, NULL, &sysctl_info_lenght, NULL, 0);
        void *const sysctl_info = malloc(sysctl_info_lenght);
        int n = sysctl(mib, 2, sysctl_info, &sysctl_info_lenght, NULL, 0);
        if (n != 0)
        {
                halt_and_catch_fire("sysctl error", EXIT_FAILURE);
        }
        return sysctl_info;
}
