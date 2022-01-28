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
#include <random>
#include <utility>
#include <cmath>

using namespace std;

struct Point{
    int id = 0;
    int x, new_x;
    int y, new_y;
    vector<Point*> neighbours;
};

int main(){
	Display *disp;
	int screen;
	Window win;
	GC gc;

	disp = XOpenDisplay(NULL);
    screen = DefaultScreen(disp);

    XColor color;
    Colormap colormap;
    char black[] = "#201f41";
    colormap = DefaultColormap(disp, 0);
    XParseColor(disp, colormap, black, &color);
    XAllocColor(disp, colormap, &color);

	win = XCreateSimpleWindow(disp, RootWindow(disp, screen), 800, 
    	250, 1500, 1500, 0, WhitePixel(disp, screen), color.pixel);

    XSizeHints    my_hints = {0};
    my_hints.flags  = PPosition | PSize;        
    my_hints.x      = 800;                        
    my_hints.y      = 250;
    my_hints.width  = 1500;
    my_hints.height = 1500;
    XSetNormalHints(disp, win, &my_hints);   
    XMapWindow(disp, win);	

    gc = XCreateGC(disp, win, 0, 0);

    XColor color1;
    Colormap colormap1;
    char red[] = "#912e54"; 
    colormap1 = DefaultColormap(disp, 0);
    XParseColor(disp, colormap1, red, &color1);
    XAllocColor(disp, colormap1, &color1);  

    XSetForeground(disp, gc, color1.pixel);  

    XSelectInput(disp, win, ExposureMask | KeyPressMask | StructureNotifyMask | 
        SubstructureNotifyMask | SubstructureRedirectMask | VisibilityChangeMask | 
        ResizeRedirectMask | PropertyChangeMask | ColormapChangeMask | EnterWindowMask |
        ButtonPressMask | ButtonReleaseMask);

    // 500*500 pixel window with 750*750 as center
    random_device pixel_pos_seed;
    mt19937 engine(pixel_pos_seed());
    uniform_int_distribution<mt19937::result_type> distr(0,2499);

    // create vertices (points)
    vector<Point> points;
    for(auto i = 0; i < 100; i++){
        int win_pixel_indx = distr(engine);
        int row = 725 + (win_pixel_indx / 50);
        int colm = 725 + (win_pixel_indx % 50);

        //cout << row << " " << colm << endl;

        Point point;
        point.id = i;
        point.x = colm;
        point.y = row;

        points.push_back(point);  
    }

    // number of connections / vertex (point)
    random_device nr_con_seed;
    mt19937 engine1(nr_con_seed());
    uniform_int_distribution<mt19937::result_type> distr1(1,3);

    // index of vertex (point)
    random_device indx_vertex_seed;
    mt19937 engine2(indx_vertex_seed());
    uniform_int_distribution<mt19937::result_type> distr2(0,99);

    // create random connections
    for(auto i = 0; i < points.size(); i++){
        int nr_of_con = distr1(engine1);
        nr_of_con = nr_of_con - points[i].neighbours.size();
        nr_of_con = nr_of_con < 0 ? 0 : nr_of_con;
        while(nr_of_con){
            int indx = distr2(engine2);
            if(points[i].id == indx){
                continue;
            }
            bool found = false;
            for(auto j = 0; j < points[i].neighbours.size(); j++){
                if(points[i].neighbours[j]->id == indx){
                    found = true;
                    break;
                }
            }
            if(!found){
                points[i].neighbours.push_back(&points[indx]);
                points[indx].neighbours.push_back(&points[i]);
                nr_of_con--;
            }
        }
    }

    for(auto i = 0; i < points.size(); i++){
        cout << points[i].id << ": ";
        for(auto j = 0; j < points[i].neighbours.size(); j++){
            cout << points[i].neighbours[j]->id << " ";
        }
        cout << endl;
    }

    //return 0;

/*
    Point point1;
    point1.id = 1;
    point1.x = 750;
    point1.y = 750;

    Point point2;
    point2.id = 2;
    point2.x = 760;
    point2.y = 760;

    Point point3;
    point3.id = 3;
    point3.x = 800;
    point3.y = 750;

    Point point4;
    point4.id = 4;
    point4.x = 750;
    point4.y = 800;

    Point point5;
    point5.x = 1400;
    point5.y = 1390;

    point1.neighbours.push_back(&point2);
    point1.neighbours.push_back(&point3);
    point1.neighbours.push_back(&point4);

    point2.neighbours.push_back(&point1);
    point2.neighbours.push_back(&point3);
    point2.neighbours.push_back(&point4);
    
    point3.neighbours.push_back(&point1);
    point3.neighbours.push_back(&point2);
    point3.neighbours.push_back(&point4);

    point4.neighbours.push_back(&point1);
    point4.neighbours.push_back(&point2);
    point4.neighbours.push_back(&point3); 
    //point4.neighbours.push_back(&point5);

    //point5.neighbours.push_back(&point4);

    vector<Point*> points;
    points.push_back(&point1);
    points.push_back(&point2);
    points.push_back(&point3);
    points.push_back(&point4);
*/
    double K = 2.8*sqrt((1500*1500)/points.size());  
    //double K = 10;
    int delta_x, delta_y; 
    double mag;
    double t = 5; //5

    for(auto i = 0; i < 10000; i++){
        for(auto j = 0; j < points.size(); j++){
            for(auto k = 0; k < points[j].neighbours.size(); k++){
                XDrawLine(disp, win, gc, points[j].x, points[j].y, 
                    points[j].neighbours[k]->x, points[j].neighbours[k]->y);
                XFlush(disp);
            }
        }        

        for(auto j = 0; j < points.size(); j++){
            double v_i = 0;
            double v_j = 0;
            for(auto k = 0; k < points.size(); k++){
                if(&points[j] != &points[k]){
                    delta_x = points[j].x - points[k].x;
                    delta_y = points[j].y - points[k].y;
                    mag = sqrt(pow(delta_x,2)+pow(delta_y,2));
                    double r_force = (K*K)/mag;
                    v_i += (delta_x/mag)*r_force;
                    v_j += (delta_y/mag)*r_force;        
                }
            }
            
            for(auto k = 0; k < points[j].neighbours.size(); k++){
                delta_x = points[j].neighbours[k]->x - points[j].x;
                delta_y = points[j].neighbours[k]->y - points[j].y;
                mag = sqrt(pow(delta_x,2)+pow(delta_y,2));
                double a_force = (mag*mag)/K;
                v_i += (delta_x/mag)*a_force;
                v_j += (delta_y/mag)*a_force;
            }

            mag = sqrt(pow(v_i,2)+pow(v_j,2));
            
            points[j].new_x = round(points[j].x + (v_i/mag) * min(mag,t));
            points[j].new_y = round(points[j].y + (v_j/mag) * min(mag,t));                
           
            points[j].new_x = min(1400, max(100, points[j].new_x));
            points[j].new_y = min(1400, max(100, points[j].new_y));
        }

        //t = pow(0.95,i);         //pow(1.1,i);

        for(auto j = 0; j < points.size(); j++){
            points[j].x = points[j].new_x;
            points[j].y = points[j].new_y;
        }

        this_thread::sleep_for(chrono::milliseconds(10));
        t *= 0.9999;
        XClearWindow(disp, win);
        cout << i << endl;
    } 

    cin.get();
    XFreeGC(disp, gc);
    XDestroyWindow(disp, win);
    XCloseDisplay(disp);
    return 0;
}