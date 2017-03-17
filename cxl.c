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
    char **tmp = groups;
    char *display_name = NULL;
    Display *display = NULL;
    if (!(display = XOpenDisplay(display_name))) {
        fprintf(stderr, "failed to open display\n");
        goto out;
    }
    if (XkbSelectEventDetails(display,
                              XkbUseCoreKbd,
                              XkbStateNotify,
                              XkbGroupLockMask,
                              XkbGroupLockMask) != True) {
        fprintf(stderr, "failed to select layout change event\n");
        goto out_close_display;
    }
    if (XkbRF_GetNamesProp(display, NULL, &vd) != True) {
        fprintf(stderr, "failed to get layout names\n");
        goto out_close_display;
    }
    while ((*(tmp++) = strsep(&vd.layout, DELIMETER)));
    if (XkbGetState(display, XkbUseCoreKbd, &state) == Success)
        fprintf(stdout, LAYOUT_FORMAT, groups[state.locked_group]);
    else
        fprintf(stderr, "failed to get current keyboard state\n");
    while (1)
        if (XNextEvent(display, &event.core) == Success)
            fprintf(stdout, LAYOUT_FORMAT, groups[event.state.locked_group]);
        else
            fprintf(stderr, "failed to get next event\n");

    XFree(vd.model);
    XFree(vd.layout);
    XFree(vd.variant);
    XFree(vd.options);
out_close_display:
    rc = XCloseDisplay(display);
out:
    return rc;
}

