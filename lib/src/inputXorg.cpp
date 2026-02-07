#include <input.hpp>
#include "shm.h"
#include "configuration.h"
#include "types.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <chrono>

#ifdef WIN32
#elif __linux__
#include <X11/X.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/Xrandr.h>
#include <dirent.h>
#include <cstring>

static bool XShmHaveFail = false;
static bool XShmBadMatch = false;  /* SHM non supporté (fréquent sous XWayland) */

int handlerXorgException(Display * d, XErrorEvent *e)
{
    if (    e->error_code   == 8
        &&  e->minor_code   == 4
        &&  e->request_code == 130
        )
    {
        XShmHaveFail = true;
    }
    else if (e->error_code == 8 && e->request_code == 73)
    {
        /* BadMatch sur MIT-SHM (XShmGetImage) : courant sous XWayland */
        if (!XShmBadMatch) {
            XShmBadMatch = true;
            std::cerr << "Argus: X11 SHM capture failed (BadMatch), using XGetImage fallback (e.g. under XWayland)." << std::endl;
        }
    }
    else if (    e->error_code   == 9
             &&  e->minor_code   == 0
             &&  e->request_code == 73
             )
    {
        std::cerr << "Source window killed, exiting" << std::endl;
        exit(0);
    }
    else
    {
        std::cerr << "Xorg exception catched" << std::endl;
        std::cerr << "Type         :" << e->type << std::endl;
        std::cerr << "Serial       :" << e->serial << std::endl;
        std::cerr << "Error Code   :" << (unsigned int) e->error_code << std::endl;
        std::cerr << "Minor Code   :" << (unsigned int) e->minor_code << std::endl;
        std::cerr << "Resource ID  :" << e->resourceid << std::endl;
        std::cerr << "Request Code :" << (unsigned int) e->request_code << std::endl;
    }
    return 0;
}


bool input::initXSHM()
{
    XSetErrorHandler(handlerXorgException);

    ximg = XShmCreateImage(display,
                                    DefaultVisualOfScreen(attributes.screen),
                                    DefaultDepthOfScreen(attributes.screen),
                                    ZPixmap, NULL,
                                    &shminfo,
                                    width, height);

    shminfo.shmid = shmget(IPC_PRIVATE, ximg->bytes_per_line * ximg->height, IPC_CREAT|IPC_EXCL|0777);
    shminfo.shmaddr = ximg->data = (char*)shmat(shminfo.shmid, 0, 0);
    shminfo.readOnly = False;
    if(shminfo.shmid < 0)
        std::cerr << "Fatal shminfo error!" << std::endl;
    Status s1 = XShmAttach(display, &shminfo);
    std::cerr << "XShmAttach() " <<  (s1 ? "success!" : "failure!") << std::endl;;
    return true;
}

bool input::captureXSHM()
{
    if (XShmBadMatch) {
        /* Fallback sans SHM (XWayland / fenêtre composée) */
        XImage *tmp = XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);
        if (tmp && ximg && tmp->data && ximg->data && tmp->bytes_per_line == ximg->bytes_per_line
            && tmp->height == ximg->height) {
            memcpy(ximg->data, tmp->data, (size_t)(ximg->bytes_per_line * ximg->height));
        }
        if (tmp)
            XDestroyImage(tmp);
        return true;
    }
    XShmGetImage(display, root, ximg, 0, 0, 0x00ffffff);
    if (XShmHaveFail)
    {
        ximg = XGetSubImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap, ximg, 0, 0);
    }
    return true;
}

void input::cleanupXSHM()
{
    XCloseDisplay(display);
}
#endif
