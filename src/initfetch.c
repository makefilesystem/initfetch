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

char* read_grep(char* file, char* pattern, int offset, int cut);
char* read_all(char* file);

typedef enum {
    MAGENTA = 35,
    BLACK = 30,
    WHITE = 37,
    RED = 31,
    GREEN = 32,
    BLUE = 34,
    YELLOW = 33
} Color;

Color selected_color = MAGENTA;
bool enable_distro = true, enable_kernel = true, enable_uptime = true, enable_ram = true, enable_wm = true, enable_shell = true;

void apply_color(Color color) {
    printf("\033[1;%dm", color);
}

void reset_color() {
    printf("\033[0m");
}

void print_label(const char* label) {
    apply_color(selected_color);
    printf("%s", label);
    reset_color();
}

void read_config(const char* config_file) {
    FILE* file = fopen(config_file, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open config file %s\n", config_file);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "colors=", 7) == 0) {
            char* color = line + 7;
            color[strcspn(color, "\n")] = '\0';  // Remove newline character

            if (strcmp(color, "magenta") == 0) selected_color = MAGENTA;
            else if (strcmp(color, "black") == 0) selected_color = BLACK;
            else if (strcmp(color, "white") == 0) selected_color = WHITE;
            else if (strcmp(color, "red") == 0) selected_color = RED;
            else if (strcmp(color, "green") == 0) selected_color = GREEN;
            else if (strcmp(color, "blue") == 0) selected_color = BLUE;
            else if (strcmp(color, "yellow") == 0) selected_color = YELLOW;
        } else if (strncmp(line, "distro=", 7) == 0) enable_distro = strcmp(line + 7, "true\n") == 0;
        else if (strncmp(line, "kernel=", 7) == 0) enable_kernel = strcmp(line + 7, "true\n") == 0;
        else if (strncmp(line, "uptime=", 7) == 0) enable_uptime = strcmp(line + 7, "true\n") == 0;
        else if (strncmp(line, "ram=", 4) == 0) enable_ram = strcmp(line + 4, "true\n") == 0;
        else if (strncmp(line, "wm=", 3) == 0) enable_wm = strcmp(line + 3, "true\n") == 0;
        else if (strncmp(line, "shell=", 6) == 0) enable_shell = strcmp(line + 6, "true\n") == 0;
    }

    fclose(file);
}

void fetch_ram_usage() {
    struct sysinfo info;
    sysinfo(&info);

    long total_ram_mb = info.totalram / 1024 / 1024;
    long used_ram_mb = (info.totalram - info.freeram - info.bufferram) / 1024 / 1024;

    print_label("RAM: ");
    printf("%ldMB / %ldMB\n", used_ram_mb, total_ram_mb);
}

void fetch_user_host() {
    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    struct passwd* user_entry = getpwuid(getuid());
    char* username = user_entry->pw_name;

    apply_color(selected_color);
    printf("%s@%s\n", username, hostname);
    printf("--------------\n");
    reset_color();
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

void fetch_uptime() {
    struct sysinfo info;
    sysinfo(&info);

    print_label("Uptime: ");
    printf("%02ldh %02ldm\n", info.uptime / 3600, (info.uptime % 3600) / 60);
}

char* fetch_shell() {
    struct passwd* user_entry = getpwuid(getuid());
    return basename(user_entry->pw_shell);
}

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

int main() {
    xwrap_init();
    read_config("initfetch.conf");

    fetch_user_host();
    if (enable_distro) {
        print_label("Distro: ");
        printf("%s\n", fetch_distro());
    }
    if (enable_kernel) {
        print_label("Kernel: ");
        printf("%s\n", fetch_kernel());
    }
    if (enable_uptime) {
        fetch_uptime();
    }
    if (enable_ram) {
        fetch_ram_usage();
    }
    if (enable_wm) {
        print_label("WM: ");
        printf("%s\n", fetch_wm_name());
    }
    if (enable_shell) {
        print_label("Shell: ");
        printf("%s\n", fetch_shell());
    }

    return 0;
}

