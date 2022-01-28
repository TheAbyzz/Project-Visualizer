#include <X11/Xlib.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <X11/extensions/XTest.h>
#include <X11/Xutil.h>
#include <fstream>
#include <string.h>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

struct Win{
    Window win;
    int delta_x = 0;
    int delta_y = 0;
};

int main(int argc, char *argv[]){
	Display *display;
	int screen;
	Window win1, win2;
	XEvent event;
	GC gc;

	display = XOpenDisplay(NULL);
    vector<Win> windows;
    screen = DefaultScreen(display);

    int x = 250;
    int y = 250;

    int delta_x_add = 0;
    int delta_y_add = 0;

    for(auto i = 0; i < 9; i++){
/*        
        if(i && !(i%3)){
            y += 249;  // window height (200) + title bar height (39) + spacing (10)
            x = 250;

            delta_x_add = 0;
            delta_y_add += 249;
        }
*/
        Win win;
        win.win = XCreateSimpleWindow(display, RootWindow(display, screen), x, 
    	y, 500, 500, 0, BlackPixel(display, screen), WhitePixel(display, screen));
        //XSelectInput(display, win.win, ExposureMask | FocusChangeMask | StructureNotifyMask | SubstructureNotifyMask);
        XSizeHints    my_hints = {0};
        my_hints.flags  = PPosition | PSize;        /* I want to specify position and size */
        my_hints.x      = x;                        /* The origin and size coords I want */
        my_hints.y      = y;
        my_hints.width  = 500;
        my_hints.height = 500;
        XSetNormalHints(display, win.win, &my_hints);   /* Where new_window is the new window */    
        XMapWindow(display, win.win);

        win.delta_x += delta_x_add;
        win.delta_y += delta_y_add;

        windows.push_back(win);

        x += 539;

        delta_x_add += 210;
    }

    XSelectInput(display, windows[0].win, ExposureMask | FocusChangeMask | StructureNotifyMask | SubstructureNotifyMask);
    
    XWindowAttributes xv;
    Window child;
    int src_x, src_y;
    while(true){
        XNextEvent(display, &event);
        XTranslateCoordinates(display, windows[0].win, DefaultRootWindow(display), 0, 0, &src_x, &src_y, &child);
        //XGetWindowAttributes(display, windows[0].win, &xv); // this works too

        //cout << src_x << " " << src_y << endl;
        //cout << windows[1].delta_x << " " << windows[1].delta_y << endl;
        //cout << endl;

        //XMoveWindow(display, windows[1].win, src_x+windows[1].delta_x, src_y-39+windows[1].delta_y);

        for(auto i = 1; i < windows.size(); i++){
            XMoveWindow(display, windows[i].win, src_x+windows[i].delta_x, src_y-39+windows[i].delta_y);
        }        
    }
}