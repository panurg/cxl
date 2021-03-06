#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBrules.h>

#define DELIMETER ","

typedef void (printer_t)(int grp_num, const void *ctxt);

static void literal_prt(int grp_num, const void *ctxt)
{
    fprintf(stdout, "%s\n", ((char**)ctxt)[grp_num]);
}

static void num_prt(int grp_num, const void *ctxt)
{
    fprintf(stdout, "%d\n", grp_num);
}

static void print_version(const char *cmd)
{
    fprintf(stdout, "%s %s\n", cmd, VERSION);
}

static void print_usage(const char *cmd)
{
    fprintf(stdout, "usage: %s [options]\n", cmd);
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -d\t\tPrint current layout and exit (dump).\n");
    fprintf(stdout, "  -l\t\tPrint name of layout (default).\n");
    fprintf(stdout, "  -n\t\tPrint number of layout.\n");
    fprintf(stdout, "  -D display\tSpecify display name to use.\n");
    fprintf(stdout, "  -v\t\tOutput version information and exit.\n");
    fprintf(stdout, "  -h\t\tDisplay this message and exit.\n");
}

static void monitor_events(Display *dpy, printer_t *prt, const void *prt_ctxt)
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
    while ((opt = getopt(argc, argv, "dlnD:vh")) != -1) {
        switch (opt) {
        case 'd':
            dump = 1;
            break;
        case 'l':
            printer = literal_prt;
            break;
        case 'n':
            printer = num_prt;
            break;
        case 'D':
            display_name = optarg;
            break;
        case 'v':
            print_version(argv[0]);
            rc = EXIT_SUCCESS;
            goto out;
        case 'h':
            rc = EXIT_SUCCESS;
            /* fall through */
        default:
            print_usage(argv[0]);
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

