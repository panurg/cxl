#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBrules.h>

#define DELIMETER ","
#define LAYOUT_FORMAT "%s\r\n"

static void print_usage()
{
    fprintf(stdout, "usage: cxl [options]\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -d\tPrint current layout and exit (dump).\n");
    fprintf(stdout, "  -h\tThis message.\n");
}

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
    int opt = 0;
    int done = 0;
    while ((opt = getopt(argc, argv, "dh")) != -1) {
        switch (opt) {
        case 'd':
            done = 1;
            break;
        case 'h':
            rc = EXIT_SUCCESS;
            /* fall through */
        default:
            print_usage();
            goto out;
        }
    }
    if (!(display = XOpenDisplay(display_name))) {
        fprintf(stderr, "failed to open display\n");
        goto out;
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
    if (!done && XkbSelectEventDetails(display,
                                       XkbUseCoreKbd,
                                       XkbStateNotify,
                                       XkbGroupLockMask,
                                       XkbGroupLockMask) != True) {
        fprintf(stderr, "failed to select layout change event\n");
        goto out_close_display;
    }
    while (!done)
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

