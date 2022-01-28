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
#include <math.h>
#include <iomanip>

using namespace std;

// USE THE CUBES CHANING CORDINATES TO RETRCIT THE GRAHP!!! NOW WE ARE ONLY USING -1 AND 1

struct Point{
    int id = 0;
    double c_cords[3] = {0};
    double n_cords[3] = {0};
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
    char black[] = "#000000";
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
    char red[] = "#0e8c80"; 
    colormap1 = DefaultColormap(disp, 0);
    XParseColor(disp, colormap1, red, &color1);
    XAllocColor(disp, colormap1, &color1);  

    XSetForeground(disp, gc, color1.pixel);  

    XSelectInput(disp, win, ExposureMask | KeyPressMask | StructureNotifyMask | 
        SubstructureNotifyMask | SubstructureRedirectMask | VisibilityChangeMask | 
        ResizeRedirectMask | PropertyChangeMask | ColormapChangeMask | EnterWindowMask |
        ButtonPressMask | ButtonReleaseMask);

    // create cube vertices
    Point point0;
    point0.id = 0;
    point0.c_cords[0] = -1;
    point0.c_cords[1] =  1;
    point0.c_cords[2] = -1;
    
    Point point1;
    point1.id = 1;
    point1.c_cords[0] =  1;
    point1.c_cords[1] =  1;
    point1.c_cords[2] = -1;
    
    Point point2;
    point2.id = 2;
    point2.c_cords[0] = -1;
    point2.c_cords[1] = -1;
    point2.c_cords[2] = -1;

    Point point3;
    point3.id = 3;
    point3.c_cords[0] =  1;
    point3.c_cords[1] = -1;
    point3.c_cords[2] = -1;

    Point point4;
    point4.id = 4;
    point4.c_cords[0] = -1;
    point4.c_cords[1] =  1;
    point4.c_cords[2] =  1;

    Point point5;
    point5.id = 5;
    point5.c_cords[0] =  1;
    point5.c_cords[1] =  1;
    point5.c_cords[2] =  1;

    Point point6;
    point6.id = 6;
    point6.c_cords[0] = -1;
    point6.c_cords[1] = -1;
    point6.c_cords[2] =  1;

    Point point7;
    point7.id = 7;
    point7.c_cords[0] =  1;
    point7.c_cords[1] = -1;
    point7.c_cords[2] =  1;

    // connect cube vertices
    point0.neighbours.push_back(&point1);
    point0.neighbours.push_back(&point2);
    point0.neighbours.push_back(&point4);

    point1.neighbours.push_back(&point0);
    point1.neighbours.push_back(&point3);
    point1.neighbours.push_back(&point5);

    point2.neighbours.push_back(&point0);
    point2.neighbours.push_back(&point3);
    point2.neighbours.push_back(&point6);

    point3.neighbours.push_back(&point2);
    point3.neighbours.push_back(&point1);
    point3.neighbours.push_back(&point7);

    point4.neighbours.push_back(&point5);
    point4.neighbours.push_back(&point6);
    point4.neighbours.push_back(&point0);

    point5.neighbours.push_back(&point4);
    point5.neighbours.push_back(&point7);
    point5.neighbours.push_back(&point1);

    point6.neighbours.push_back(&point4);
    point6.neighbours.push_back(&point7);
    point6.neighbours.push_back(&point2);

    point7.neighbours.push_back(&point5);
    point7.neighbours.push_back(&point6);
    point7.neighbours.push_back(&point3);

    vector<Point> cube_vertices;
    cube_vertices.push_back(point0);
    cube_vertices.push_back(point1);
    cube_vertices.push_back(point2);
    cube_vertices.push_back(point3);
    cube_vertices.push_back(point4);
    cube_vertices.push_back(point5);
    cube_vertices.push_back(point6);
    cube_vertices.push_back(point7);

    // create force directed graph vertices
    random_device pixel_pos_seed;
    mt19937 engine(pixel_pos_seed());
    uniform_int_distribution<mt19937::result_type> distr(-25,25);

    vector<Point> graph_vertices;
    for(auto i = 0; i < 100; i++){
        Point point;
        point.id = i;
        double a = (int)distr(engine)*0.001;
        point.c_cords[0] = a;
        double b = (int)distr(engine)*0.001;
        point.c_cords[1] = b;
        double c = (int)distr(engine)*0.001;
        point.c_cords[2] = c;
        //cout << a << " " << b << " " << c << endl;

        graph_vertices.push_back(point);  
    }    

    // number of connections / vertex (point)
    random_device nr_con_seed;
    mt19937 engine1(nr_con_seed());
    uniform_int_distribution<mt19937::result_type> distr1(5,10);

    // index of vertex (point)
    random_device indx_vertex_seed;
    mt19937 engine2(indx_vertex_seed());
    uniform_int_distribution<mt19937::result_type> distr2(0,99);

    // create random connections
    for(auto i = 0; i < graph_vertices.size(); i++){
        int nr_of_con = distr1(engine1);
        nr_of_con = nr_of_con - graph_vertices[i].neighbours.size();
        nr_of_con = nr_of_con < 0 ? 0 : nr_of_con;
        while(nr_of_con){
            int indx = distr2(engine2);
            if(graph_vertices[i].id == indx){
                continue;
            }
            bool found = false;
            for(auto j = 0; j < graph_vertices[i].neighbours.size(); j++){
                if(graph_vertices[i].neighbours[j]->id == indx){
                    found = true;
                    break;
                }
            }
            if(!found){
                graph_vertices[i].neighbours.push_back(&graph_vertices[indx]);
                graph_vertices[indx].neighbours.push_back(&graph_vertices[i]);
                nr_of_con--;
            }
        }
    }

    cout << "initial points" << endl;
    for(auto i = 0; i < cube_vertices.size(); i++){
        cout << cube_vertices[i].id << " ";
        cout << cube_vertices[i].c_cords[0] << " ";
        cout << cube_vertices[i].c_cords[1] << " ";
        cout << cube_vertices[i].c_cords[2] << " ";
        cout << cube_vertices[i].neighbours.size() << endl;
    }

    cout << "starting" << endl;
    // WE ARE NOT LETTING THE GRAPH TAKE OVER THE WHOLE WINDOW!!!

    double K = sqrt((100*100)/graph_vertices.size());
    double delta_x, delta_y, delta_z; 
    double mag;
    double t = 0.997;
    
    double angle = 0;
    double fovx = 120 * (M_PI/180);
    double fovy = 120 * (M_PI/180);
    double zfar = 1000;
    double znear = 1;    

    //transformed points (3D)
    double trans_point1[3] = {0};
    double trans_point2[3] = {0};

    //projected points (3D -> 2D)
    double proj_point1[3] = {0};
    double proj_point2[3] = {0};

    double rad = 0.2 * (M_PI/180);

    XEvent event;
    int counter = 0;
    while (true){
        XNextEvent(disp, &event);
        if(event.xany.window == win && event.type == Expose){
            counter++;
            if(counter == 3){
                break;
            }
        }
    }

    while(true){
        //double rad = angle * (M_PI/180);

        for(auto i = 0; i < cube_vertices.size(); i++){
            trans_point1[0] = cube_vertices[i].c_cords[0];
            trans_point1[1] = cube_vertices[i].c_cords[1];
            trans_point1[2] = cube_vertices[i].c_cords[2];
            
            trans_point1[0] += 2.0;
            trans_point1[1] += 2.0;
            
            trans_point1[0] *= 0.5;
            trans_point1[1] *= 0.5;
            
            proj_point1[0] = trans_point1[0] * (1/tan(fovx/2));
            proj_point1[1] = trans_point1[1] * (1/tan(fovy/2));
            proj_point1[2] = trans_point1[2] * (-((zfar+znear)/(zfar-znear))) + (-(2*(znear*zfar)/(zfar-znear))); 
            
            double w1 = trans_point1[2] * (-1);
            if(w1 != 0){
                proj_point1[0] /= w1;
                proj_point1[1] /= w1;
            } 
            
            proj_point1[0] += 1.0;
            proj_point1[1] += 1.0; 
            
            for(auto k = 0; k < cube_vertices[i].neighbours.size(); k++){
                trans_point2[0] = cube_vertices[i].neighbours[k]->c_cords[0];
                trans_point2[1] = cube_vertices[i].neighbours[k]->c_cords[1];
                trans_point2[2] = cube_vertices[i].neighbours[k]->c_cords[2];
                
                trans_point2[0] += 2.0;
                trans_point2[1] += 2.0;
                
                trans_point2[0] *= 0.5;
                trans_point2[1] *= 0.5;
                
                proj_point2[0] = trans_point2[0] * (1/tan(fovx/2));
                proj_point2[1] = trans_point2[1] * (1/tan(fovy/2));
                proj_point2[2] = trans_point2[2] * (-((zfar+znear)/(zfar-znear))) + (-(2*(znear*zfar)/(zfar-znear))); 
                
                double w2 = trans_point2[2] * (-1);
                if(w2 != 0){
                    proj_point2[0] /= w2;
                    proj_point2[1] /= w2;
                }
                
                proj_point2[0] += 1.0;
                proj_point2[1] += 1.0;         

                XDrawLine(disp, win, gc, round(trans_point1[0]*(double)750), round(trans_point1[1]*(double)750), 
                    round(trans_point2[0]*(double)750), round(trans_point2[1]*(double)750));
                XFlush(disp);
            }
        }
        
        for(auto i = 0; i < cube_vertices.size(); i++){
            cube_vertices[i].n_cords[0] = round((cube_vertices[i].c_cords[0] * cos(rad) + cube_vertices[i].c_cords[2] * sin(rad))*100000.0)/100000.0; 
            cube_vertices[i].n_cords[1] = round(cube_vertices[i].c_cords[1]*100000.0)/100000.0;
            cube_vertices[i].n_cords[2] = round((cube_vertices[i].c_cords[0] * (-sin(rad)) + cube_vertices[i].c_cords[2] * cos(rad))*100000.0)/100000.0;
        }

        for(auto j = 0; j < cube_vertices.size(); j++){
            cube_vertices[j].c_cords[0] = cube_vertices[j].n_cords[0];
            cube_vertices[j].c_cords[1] = cube_vertices[j].n_cords[1];
            cube_vertices[j].c_cords[2] = cube_vertices[j].n_cords[2];
        }

        //angle += 0.2;
        //angle = (angle / 360) == 1 ? 0 : angle;
      
        this_thread::sleep_for(chrono::milliseconds(100));
        XClearWindow(disp, win);
    } 

    XFreeGC(disp, gc);
    XDestroyWindow(disp, win);
    XCloseDisplay(disp);
    return 0;
}