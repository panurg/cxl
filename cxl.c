#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBrules.h>

#define DELIMETER ","
#define LAYOUT_FORMAT "%s\r\n"

int main(int argc, char *argv[])
{
    int rc = EXIT_FAILURE;
    XkbEvent event;
    XkbRF_VarDefsRec vd;
    XkbStateRec state;
    char *groups[XkbNumKbdGroups];
    unsigned char num_groups = 0;
    char *display_name = NULL;
    Display *display = NULL;
    if (!(display = XOpenDisplay(display_name)))
        goto out;
    if (XkbSelectEventDetails(display,
                              XkbUseCoreKbd,
                              XkbStateNotify,
                              XkbGroupLockMask,
                              XkbGroupLockMask) != True)
        goto out_close_display;
    if (XkbRF_GetNamesProp(display, NULL, &vd) != True)
        goto out_close_display;
    while ((groups[num_groups] = strsep(&vd.layout, DELIMETER))) num_groups++;
    if (XkbGetState(display, XkbUseCoreKbd, &state) == Success)
        printf(LAYOUT_FORMAT, groups[state.locked_group]);
    while (1)
        if (XNextEvent(display, &event.core) == Success)
            printf(LAYOUT_FORMAT, groups[event.state.locked_group]);

    XFree(vd.model);
    XFree(vd.layout);
    XFree(vd.variant);
    XFree(vd.options);
out_close_display:
    rc = XCloseDisplay(display);
out:
    return rc;
}

