#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBrules.h>

int main(int argc, char *argv[])
{
    Display *display = XOpenDisplay(NULL);
    XkbEvent event;
    XkbRF_VarDefsRec varDefs;
    XkbStateRec state;
    char *tmp = NULL;
    char *groups[XkbNumKbdGroups];
    int num_groups = 0;
    XkbSelectEventDetails(display, XkbUseCoreKbd, XkbStateNotify, XkbGroupLockMask, XkbGroupLockMask);
    XkbRF_GetNamesProp(display, &tmp, &varDefs);
    groups[num_groups] = strtok(varDefs.layout, ",");
    printf("%s\r\n", groups[num_groups]);
    while(groups[num_groups])
    {
        num_groups++;
        groups[num_groups] = strtok(NULL, ",");
    }
    XkbGetState(display, XkbUseCoreKbd, &state);
    while (1)
    {
        XNextEvent(display, &event.core);
        printf("%s\r\n", groups[event.state.locked_group]);
    }
    return XCloseDisplay(display);
}

