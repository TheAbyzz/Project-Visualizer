/* Links

   https://stackoverflow.com/questions/44476594/x11-why-i-cant-draw-any-text
   https://www.lemoda.net/c/xlib-text-box/

   no borders
   https://groups.google.com/g/comp.unix.programmer/c/Ot2YLbPQhVA
   https://tronche.com/gui/x/xlib/window/attributes/

   SEMI TRANSPARENT WINDOW?

   font size
   https://stackoverflow.com/questions/15038180/increase-font-size-and-properties-in-x11-r6
   http://euklid.mi.uni-koeln.de/c/mirror/www.cs.curtin.edu.au/units/cg252-502/notes/lect5f1.html

   walking up the frames
   https://www.linuxquestions.org/questions/programming-9/why-can%27t-i-get-all-of-the-window-attributes-using-xlib-759964/
*/

//x = 2cost
// y = 1sint

//#include <stdio.h>
//#include <stdlib.h>
#include <X11/Xlib.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <X11/extensions/XTest.h>
#include <X11/Xutil.h>
#include <fstream>
#include <string.h>

using namespace std;

int main(int argc, char *argv[]){
	Display *display;
	int screen;
	Window window;
	//XEvent event;
	GC gc;
	XFontStruct *font;

	display = XOpenDisplay(NULL);
    if(display == NULL){
		cout << "cannot open Display" << endl;
		return 0;
	}

/*  transparent window
    XVisualInfo vinfo;
    XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);

    XSetWindowAttributes attr;
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);
    attr.border_pixel = 0;
    attr.background_pixel = 0; 
    attr.override_redirect = true;

    window = XCreateWindow(display, DefaultRootWindow(display), 0, 0, 1000, 1000, 0, vinfo.depth, InputOutput, vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel, &attr);
    XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask | ButtonPressMask);
    gc = XCreateGC(display, window, 0, 0);

    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, window, &wm_delete_window, 1);
*/    

    //setting background color
    XColor color;
    Colormap colormap;
    char cyan[] = "#00ffff"; // dark pink #f61e61

    colormap = DefaultColormap(display, 0);
    XParseColor(display, colormap, cyan, &color);
    XAllocColor(display, colormap, &color);

    // creating window
    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 
    	0, 1000, 1000, 0, BlackPixel(display, screen), color.pixel);
    XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask | ButtonPressMask | ButtonReleaseMask);
    XSelectInput(display, XDefaultRootWindow(display), SubstructureNotifyMask);
    XMapWindow(display, window);

    //creating the function for updating window content 
    gc = XCreateGC(display, window, 0, 0);
    XSetBackground (display, gc, WhitePixel(display, screen)); // do we need this??? we set background in xcreatesimpleiwindow
    XSetForeground (display, gc, BlackPixel(display, screen));
    
    //setting up the font (150, 150 affect the size of font) xlsfonts will list all available fonts
    //font = XLoadQueryFont(display, "-misc-fixed-bold-r-normal--15-140-75-75-c-90-iso8859-8");
    //font = XLoadQueryFont(display, "-bitstream-bitstream charter-bold-i-normal--0-100-0-0-p-0-adobe-standard"); // 24 0 - 1800 stays in frame
    //font = XLoadQueryFont(display, "-bitstream-bitstream charter-bold-r-normal--15-48-232-225-p-0-iso10646-1");
    
    // working one
    //font = XLoadQueryFont(display, "-bitstream-bitstream charter-bold-r-normal--20-48-309-300-p-0-iso10646-1");
    //font = XLoadQueryFont(display, "fixed");
    font = XLoadQueryFont(display, "-bitstream-courier 10 pitch-bold-i-normal--20-48-309-300-m-0-adobe-standard");
    XSetFont(display, gc, font->fid);

//    cout << "kukkuu" << endl;

/*  attempt to change font size
    changing these will just increase the spacing between lines
    cout << "ascent: " << font->ascent << endl;
    font->ascent *= 2;
    cout << "descent: " << font->descent << endl;
    font->descent *= 2;
    cout << endl;

    cout << "max bounds, lbearing: " << font->max_bounds.lbearing << endl;
    font->max_bounds.lbearing *= 2;
    cout << "max bounds, lbearing: " << font->max_bounds.rbearing << endl;
    font->max_bounds.rbearing *= 2;
    cout << "max bounds, ascent: " << font->max_bounds.ascent << endl;
    font->max_bounds.ascent *= 2;
    cout << "max bounds, descent: " << font->max_bounds.descent << endl;
    font->max_bounds.descent *= 2;
    cout << endl;

    cout << "min bounds, lbearing: " << font->min_bounds.lbearing << endl;
    font->min_bounds.lbearing *= 2;
    cout << "min bounds, lbearing: " << font->min_bounds.rbearing << endl;
    font->min_bounds.rbearing *= 2;
    cout << "min bounds, ascent: " << font->min_bounds.ascent << endl;
    font->min_bounds.ascent *= 2;
    cout << "min bounds, descent: " << font->min_bounds.descent << endl;
    font->min_bounds.descent *= 2;
*/

    int x = 0;
    int y = 0;
    XEvent event;
    while(true){
		//XEvent event;
		XNextEvent(display, &event);
		if(event.type == Expose){
			// get title bar height/length
            //cout << event.xmotion.x << " " << event.xmotion.y << endl;
            Atom a, t;
            int f;
            unsigned long n, b;
            unsigned char *data = 0; 
            long *extents;

            a = XInternAtom(display, "_NET_FRAME_EXTENTS", True); /* Property to check */
            /* Window manager doesn't set up the extents immediately */
            /* Wait until they are set up and there are 4 of them */
            while (XGetWindowProperty(display, window, a,
                   0, 4, False, AnyPropertyType,
                   &t, &f,
                   &n, &b, &data) != Success || n != 4 || b != 0) 
            {
                //printf ("Waiting for extents\n");
                XNextEvent(display, &event);
            }  
            extents = (long*)data; 

            XClearWindow(display, window);
            fstream file("/home/palmer/Desktop/main.cpp");
            string line;
            //int base = (int)extents[2];
            int base = 13; // or 14 why do we get 39??;
            //cout << extents[2] << endl;
            //cout << base << endl;
            //cout << extents[0] << " " << extents[1] << " " << extents[2] << " " << extents[3] << endl;
            int add = font->ascent + font->descent;
            //cout << endl;
            //cout << font->ascent << " " << font->descent << endl;
            const char *msg;
/*
            cout << "caption height " << (int)extents[2] << endl;
            cout << "em square height " << 18 << endl;
            cout << "font ascent " << font->ascent << endl;
            cout << "font descent " << font->descent << endl;

            int tot = font->ascent + font->descent;
            cout << "font->ascent + font->descent " << tot << endl;
            int a_n = 20 - tot;
            cout << "pixel size - tot " << a_n << endl;
            int b_n = a_n / 2;
            cout << "b_n " << b_n << endl;
            int c_n = a_n % 2;
            cout << "c_n " << c_n << endl;
            int d_n = b_n < c_n ? c_n : b_n;
            cout << "d_n " << d_n << endl;
            //base = (int)extents[2] - d_n;   // 18 or 19, 19 is better
            base = 15;
            cout << "base " << base << endl;
            cout << "--------------------------------------------------------------------------------" << endl;
*/
            while(getline(file, line)){
                msg = line.c_str();
                XDrawString(display, window, gc, 0, base, msg, line.length());
                //Xutf8DrawString(display, window, gc, 0, base, msg, line.length());
                base += add; // add and 150 too much
            }

			XMoveWindow(display, window, x, y);

            Window root_win;
            Window parent_win;
            Window *children = nullptr;
            unsigned int num_children;
            XWindowAttributes xv;

            cout << "window structure" << endl;
            XQueryTree(display, window, &root_win, &parent_win, &children, &num_children);
            XGetWindowAttributes(display, parent_win, &xv);
            cout << "parent_win " << xv.width << " " << xv.height << " " << num_children << endl;    

            Window parent_win_2;
            XQueryTree(display, parent_win, &root_win, &parent_win_2, &children, &num_children);
            XGetWindowAttributes(display, parent_win_2, &xv);
            cout << "parent parent_win " << xv.width << " " << xv.height << " " << num_children << endl;
            XGetWindowAttributes(display, children[0], &xv);
            cout << "child " << xv.width << " " << xv.height << endl;

            Window parent_win_3;
            XQueryTree(display, parent_win_2, &root_win, &parent_win_3, &children, &num_children);
            XGetWindowAttributes(display, parent_win_3, &xv);            
            cout << " parent parent parent_win " << xv.width << " " << xv.height << " " << num_children << endl;
            XGetWindowAttributes(display, children[0], &xv);            
            cout << "child " << xv.width << " " << xv.height << " " << endl;
            XGetWindowAttributes(display, children[1], &xv);            
            cout << "child " << xv.width << " " << xv.height << " " << endl;
            cout << "-------------------------------------------------------------" << endl;

            if(&parent_win_3 == &root_win){
                cout << "last is root" << endl;
            }

/*
            2560x1600 is root since scree dimensions are 2560x1600
            root has 2 childen; 1028x1067 and 1000x1000 (our application window)
            1028x1067 has 1 child; 1000x1000, 1028x1067 is window frame for 1000x1000
            1000x1000 has no children casue we haven't created any in our application

            How is 1000x1000 place in the window frame 1028x1067? evenly?
            67/2 e.g. 39 at top and 28 bottom 

            We get 39 for caption/tile bar heigth/length

            and 28/2 e.g. 14 at left and 14 right

            point size is the height of the imaginery box that surrounds the character
            1 point = 1/72 of an inch

            pixel size = point size ???

            --------- frame window 

                                                39 pixels

            --------- our application window 

            dot = point

            In your computer you calculate Points to Pixels like this PX = Points * SystemDPI / 72. DPI in a video world

            what is the difference delta y between the top point of character and imaginary box border
            in pixels must be between 10 - 14, right?

            The size of the em square can be set from Element > Font Info…
            XFontStruct
            

*/
/*
            Window parent_win_4;
            XQueryTree(display, parent_win_3, &root_win, &parent_win_4, &children, &num_children);
            XGetWindowAttributes(display, parent_win_4, &xv);
            cout << xv.width << " " << xv.height << " " << num_children << endl;        
*/
            // always true if put outside this if condition. Should XNextEvent be used before it?
            if(event.type == ConfigureNotify){
			    XConfigureEvent xce = event.xconfigure;
                if(xce.x != x || xce.y != y){
                	XMoveWindow(display,window,x,y);
                }
		    }
        }
        else if(event.type == ConfigureNotify){
            /*cout << "morooo" << endl;
            XConfigureEvent xce = event.xconfigure;
            if(xce.x != x || xce.y != y){
                XMoveWindow(display,window,x,y);
            }*/

            //cout << event.xmotion.x << " " << event.xmotion.y << endl;
        }
        
		else if(event.type == ButtonPress){
			switch(event.xbutton.button){
                /*case Button0:
                    cout << "Button 0" << endl;
                    break;*/
                case Button1:
                    cout << "Button 1" << endl;
                    break;
                case Button2:
                    cout << "Button 2" << endl;
                    break;
                case Button3:
                    cout << "Button 3" << endl;
                    break;
				case Button4:
					y -= 80;
					break;
				case Button5:
                    y += 80;
                    break;
			}
			XMoveWindow(display, window, x, y);
		}
        else if(event.type == ButtonRelease){
            switch(event.xbutton.button){
                /*case Button0:
                    cout << "Button 0 release" << endl;
                    break;*/
                case Button1:
                    cout << "Button 1 release" << endl;
                    break;
                case Button2:
                    cout << "Button 2 release" << endl;
                    break;
                case Button3:
                    cout << "Button 3 release" << endl;
                    break;     
            }       
        }
        else if(event.type = EnterNotify){
            //cout << " EnterNotify" << endl;
/*            XWindowAttributes window_attributes_return;
            XGetWindowAttributes(display, window, &window_attributes_return);
            cout << window_attributes_return.x << " " << window_attributes_return.y << endl; 
            Window root;
            int x, y;
            unsigned int width, height;
            unsigned int ba;
            unsigned int depth;
            XGetGeometry(display, window, &root, &x, &y, &width, &height, &ba, &height);
            cout << x << " " << y << endl;
            cout << event.xmotion.x << " " << event.xmotion.y << endl;
            cout << "-----------------------------------------------------" << endl;
*/        
            Window root_win;
            Window parent_win;
            Window *children = nullptr;
            unsigned int num_children;
            XWindowAttributes xv;
            
            cout << "window other info" << endl;
            XGetWindowAttributes(display, window, &xv);
            cout << "our window: " << xv.x << " " << xv.y << endl;

            XQueryTree(display, window, &root_win, &parent_win, &children, &num_children);
            XGetWindowAttributes(display, parent_win, &xv);
            cout << "parent_win: " << xv.x << " " << xv.y << " " << num_children << endl; 
            
            Window parent_win2;
            XQueryTree(display, parent_win, &root_win, &parent_win2, &children, &num_children);  
            XGetWindowAttributes(display, parent_win2, &xv);
            cout << "parent parent_win " << xv.x << " " << xv.y << " " << num_children << endl;
            cout << "-------------------------------------------------------" << endl;
        }
        else if(event.type = LeaveNotify){cout << "LeaveNotify " << endl;}
        else if(event.type = FocusIn ){cout << " FocusIn" << endl;}
        else if(event.type = FocusOut){cout << " FocusOut" << endl;}
        else if(event.type = KeymapNotify){cout << "KeymapNotify " << endl;}
        else if(event.type = GraphicsExpose){cout << "GraphicsExpose " << endl;}
        else if(event.type = NoExpose){cout << " NoExpose" << endl;}
        else if(event.type = CirculateRequest){cout << "CirculateRequest " << endl;}
        else if(event.type = MapRequest){cout << "MapRequest " << endl;}
        else if(event.type = CirculateNotify){cout << "CirculateNotify, " << endl;}
        else if(event.type = MapNotify){cout << " MapNotify" << endl;}
        else if(event.type = MappingNotify){cout << "MappingNotify " << endl;}
        else if(event.type = UnmapNotify){cout << " UnmapNotify" << endl;}
        else if(event.type = ClientMessage){cout << " ClientMessage" << endl;}
        else if(event.type =PropertyNotify ){cout << " PropertyNotify" << endl;}
        else if(event.type = SelectionNotify){cout << "SelectionNotify" << endl;}
        else if(event.type = SelectionRequest){cout << "SelectionRequest" << endl;}
        else if(event.type = SelectionClear){cout << "SelectionClear" << endl;}

		else if(event.type == KeyPress){
			XFreeGC(display, gc);
            XDestroyWindow(display, window);
	        XCloseDisplay(display);
	        return 0;
		}
	}

    XFreeGC(display, gc);
    XDestroyWindow(display, window);
	XCloseDisplay(display);

	return 0;
}

/*

https://askubuntu.com/questions/197828/how-to-find-and-change-the-screen-dpi

Let's calculate optimal DPI for my monitor. Actual size of the screen can be found with the command 
xrandr | grep -w connected (convert output to centimetres) or with a long ruler by hand. In my 
case: X = 47.4cm ; Y = 29.6cm. Divide them by 2.54 to get the size in inches: X ~ 18.66in ; Y ~ 11.65in. 
Lastly divide the actual dots amount (based on your resolution) by the size in inches: X = 1680/18.66 ~ 90dpi ; 
Y = 1050/11.65 ~ 90dpi. So my real dpi is 90.

*/