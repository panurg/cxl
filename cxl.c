#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBrules.h>

#define DELIMETER ","

typedef void (printer_t)(int grp_num, void *ctxt);

static void literal_prt(int grp_num, void *ctxt)
{
    fprintf(stdout, "%s\n", ((char**)ctxt)[grp_num]);
}

static void print_usage()
{
    fprintf(stdout, "usage: cxl [options]\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -d\tPrint current layout and exit (dump).\n");
    fprintf(stdout, "  -h\tThis message.\n");
}

static void monitor_events(Display *dpy, printer_t *prt, void *prt_ctxt)
{
    XkbEvent event;
    if (XkbSelectEventDetails(dpy,
                              XkbUseCoreKbd,
                              XkbStateNotify,
                              XkbGroupLockMask,
                              XkbGroupLockMask) == True) {
        while (1)
            if (XNextEvent(dpy, &event.core) == Success)
                prt(event.state.locked_group, prt_ctxt);
            else
                fprintf(stderr, "failed to get next event\n");
    } else {
        fprintf(stderr, "failed to select layout change event\n");
    }
}

int main(int argc, char *argv[])
{
    int rc = EXIT_FAILURE;
    XkbRF_VarDefsRec vd;
    XkbStateRec state;
    char *groups[XkbNumKbdGroups];
    char **tmp = groups;
    char *display_name = NULL;
    Display *display = NULL;
    int opt = 0;
    int dump = 0;
    printer_t *printer = literal_prt;
    while ((opt = getopt(argc, argv, "dh")) != -1) {
        switch (opt) {
        case 'd':
            dump = 1;
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
        printer(state.locked_group, groups);
    else
        fprintf(stderr, "failed to get current keyboard state\n");
    if (!dump) monitor_events(display, printer, groups);

    XFree(vd.model);
    XFree(vd.layout);
    XFree(vd.variant);
    XFree(vd.options);
out_close_display:
    rc = XCloseDisplay(display);
out:
    return rc;
}

