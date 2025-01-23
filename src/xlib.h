/*

The GPL License (GNU Public License)

Copyright (c) 2025 mkfs,DRom

xlib.h (~/initfetch/src)

*/

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Display* xwrap_dpy = NULL;

void xwrap_init() {
    xwrap_dpy = XOpenDisplay(NULL);
    if (!xwrap_dpy) {
        fprintf(stderr, "failed to open x display\n");
        exit(1);
    }
}

Window prop_get_window(Window win, const char* prop_name) {
    Atom prop = XInternAtom(xwrap_dpy, prop_name, True);
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char* prop_data = NULL;

    if (XGetWindowProperty(xwrap_dpy, win, prop, 0, (~0L), False, AnyPropertyType,
                           &actual_type, &actual_format, &nitems, &bytes_after, &prop_data) == Success && prop_data) {
        Window result = *(Window*)prop_data;
        XFree(prop_data);
        return result;
    }

    return None;
}

char* prop_get_str(Window win, const char* prop_name) {
    Atom prop = XInternAtom(xwrap_dpy, prop_name, True);
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char* prop_data = NULL;

    if (XGetWindowProperty(xwrap_dpy, win, prop, 0, (~0L), False, AnyPropertyType,
                           &actual_type, &actual_format, &nitems, &bytes_after, &prop_data) == Success && prop_data) {
        char* result = strdup((char*)prop_data);
        XFree(prop_data);
        return result;
    }

    return strdup("");
}

