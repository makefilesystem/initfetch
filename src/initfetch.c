/*

The GPL License (GNU Public License)

Copyright (c) 2025 mkfs,DRom

initfetch.c (~/initfetch/src)

*/

#include <libgen.h>
#include <math.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "xlib.h"

char* read_grep(char* file, char* pattern, int offset, int cut) {
    FILE* fp = fopen(file, "r");
    if (!fp) return "";

    char* line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1) {
        if (strstr(line, pattern)) {
            line += strlen(pattern) + offset;
            line[strlen(line) - cut - 1] = '\0';
            fclose(fp);
            return strdup(line);
        }
    }

    fclose(fp);
    return "";
}

char* read_all(char* file) {
    FILE* fp = fopen(file, "r");
    if (!fp) return "";

    char* line = NULL;
    size_t len = 0;
    char* content = malloc(1);
    *content = '\0';

    while (getline(&line, &len, fp) != -1) {
        content = realloc(content, strlen(content) + strlen(line) + 1);
        strcat(content, line);
    }

    fclose(fp);
    return content;
}

char* fetch_wm_name() {
    Window root = DefaultRootWindow(xwrap_dpy);
    Window wm_check = prop_get_window(root, "_NET_SUPPORTING_WM_CHECK");
    return prop_get_str(wm_check, "_NET_WM_NAME");
}

char* fetch_distro() {
    char* distro_name = read_grep("/etc/os-release", "PRETTY_NAME=", 1, 1);
    struct utsname uts_name;
    uname(&uts_name);

    char* result = malloc(strlen(distro_name) + strlen(uts_name.machine) + 2);
    sprintf(result, "%s %s", distro_name, uts_name.machine);
    return result;
}

char* fetch_kernel() {
    char* kernel = read_all("/proc/sys/kernel/osrelease");
    kernel[strlen(kernel) - 1] = '\0';
    return kernel;
}

char* fetch_shell() {
    struct passwd* user_entry = getpwuid(getuid());
    return basename(user_entry->pw_shell);
}

void fetch_uptime() {
    struct sysinfo info;
    sysinfo(&info);
    printf("Uptime: %02ldh %02ldm\n", info.uptime / 3600, (info.uptime % 3600) / 60);
}

int main() {
    xwrap_init();

    printf("Distro: %s\n", fetch_distro());
    printf("Kernel: %s\n", fetch_kernel());
    fetch_uptime();
    printf("WM: %s\n", fetch_wm_name());
    printf("Shell: %s\n", fetch_shell());

    return 0;
}
