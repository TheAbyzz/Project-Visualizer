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

// occasionally 2-3 nodes are launched to the corner of the window and then the whole graph is dragged
// and squished into that corner into a small point (1 pixel)

using namespace std;

struct Point{
    int id = 0;
    double c_cords[3] = {0};
    double n_cords[3] = {0};
    vector<Point*> neighbours;
};

int main(){

    // create force directed graph vertices
    random_device pixel_pos_seed;
    mt19937 engine(pixel_pos_seed());
    uniform_int_distribution<mt19937::result_type> distr(-25,25);

    vector<Point> graph_vertices;
    for(auto i = 0; i < 10; i++){ // 100
        Point point;
        point.id = i;
        double a = (int)distr(engine)*0.001;
        point.c_cords[0] = a;
        double b = (int)distr(engine)*0.001;
        point.c_cords[1] = b;
        double c = (int)distr(engine)*0.001;
        point.c_cords[2] = c;

        graph_vertices.push_back(point);  
    }    

    // number of connections / vertex (point)
    random_device nr_con_seed;
    mt19937 engine1(nr_con_seed());
    uniform_int_distribution<mt19937::result_type> distr1(1,10); // 50,80

    // index of vertex (point)
    random_device indx_vertex_seed;
    mt19937 engine2(indx_vertex_seed());
    uniform_int_distribution<mt19937::result_type> distr2(0,9);

    // create random connections/edges
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

	win = XCreateSimpleWindow(disp, RootWindow(disp, screen), 20, 
    	50, 1500, 1500, 0, WhitePixel(disp, screen), color.pixel);

    XSizeHints    my_hints = {0};
    my_hints.flags  = PPosition | PSize;        
    my_hints.x      = 20;                        
    my_hints.y      = 50;
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

    double K = sqrt((1500*1500)/graph_vertices.size());
    double delta_x, delta_y, delta_z; 
    double mag;
    double t = 0.999;
    
    double fovx = 120 * (M_PI/180);
    double fovy = 120 * (M_PI/180);
    double zfar = 1000;
    double znear = 1;    

    //transformed points (3D)
    double trans_point1[3] = {0};
    double trans_point2[3] = {0};

    //rotated points (3d)
    double rot_point1[3] = {0};
    double rot_point2[3] = {0};

    //projected points (3D -> 2D)
    double proj_point1[3] = {0};
    double proj_point2[3] = {0};

    // wait for window to be drawable
    XEvent event;
    int counter = 0;
    while(counter < 2){
        XNextEvent(disp, &event);
        if(event.xany.window == win && event.type == Expose){
            counter++;
        }
    }

    // The size of the plane of projection will be [-1, 1]. All points that project outside of this range are not drawn.
    // https://paroj.github.io/gltut/Positioning/Tut04%20Perspective%20Projection.html

    for(auto i = 0; i < 100; i++){

        //cout << i << endl;

        // force-directed graph
        for(auto j = 0; j < graph_vertices.size(); j++){
            // rotating
            trans_point1[0] = graph_vertices[j].c_cords[0]; 
            trans_point1[1] = graph_vertices[j].c_cords[1];
            trans_point1[2] = graph_vertices[j].c_cords[2]+1.0; // changing this changes the size

            // translating x and y
            //trans_point1[0] -= 1.0;
            //trans_point1[1] -= 1.0;

            // scaling x and y
            //trans_point1[0] *= 0.005;        //0.9;
            //trans_point1[1] *= 0.005;        //0.9;
            
            // projecting
            proj_point1[0] = trans_point1[0] * (1/tan(fovx/2));
            proj_point1[1] = trans_point1[1] * (1/tan(fovy/2));
            proj_point1[2] = trans_point1[2] * (-((zfar+znear)/(zfar-znear))) + (-(2*(znear*zfar)/(zfar-znear))); 
            double w1 = trans_point1[2] * (-1);
            if(w1 != 0){
                proj_point1[0] /= w1;
                proj_point1[1] /= w1;
            } 
    
            // offsetting/translating???
            proj_point1[0] += 1.0;
            proj_point1[1] += 1.0;
            proj_point1[0] *= 0.5;
            proj_point1[1] *= 0.5;      

            for(auto k = 0; k < graph_vertices[j].neighbours.size(); k++){
                // rotating
                trans_point2[0] = graph_vertices[j].neighbours[k]->c_cords[0]; 
                trans_point2[1] = graph_vertices[j].neighbours[k]->c_cords[1];
                trans_point2[2] = graph_vertices[j].neighbours[k]->c_cords[2]+1.0;
                           
                // transalting x and y
                //trans_point2[0] -= 1.0;
                //trans_point2[1] -= 1.0;

                // scaling x and y
                //trans_point2[0] *= 0.005;       //0.9;
                //trans_point2[1] *= 0.005;       //0.9;

                // projecting
                proj_point2[0] = trans_point2[0] * (1/tan(fovx/2));
                proj_point2[1] = trans_point2[1] * (1/tan(fovy/2));
                proj_point2[2] = trans_point2[2] * (-((zfar+znear)/(zfar-znear))) + (-(2*(znear*zfar)/(zfar-znear))); 
                double w2 = trans_point2[2] * (-1);
                if(w2 != 0){
                    proj_point2[0] /= w2;
                    proj_point2[1] /= w2;
                }

                // offsetting/translating???
                proj_point2[0] += 1.0;
                proj_point2[1] += 1.0;
                proj_point2[0] *= 0.5;
                proj_point2[1] *= 0.5;

                cout << proj_point1[0] << " " << proj_point1[1] << endl;
                cout << proj_point2[0] << " " << proj_point2[1] << endl;
                //cin.get();

                XDrawLine(disp, win, gc, round(proj_point1[0]*(double)1500), round(proj_point1[1]*(double)1500), 
                    round(proj_point2[0]*(double)1500), round(proj_point2[1]*(double)1500));
                XFlush(disp);
            }
        }        

        // envolve forec directed graph
        for(auto j = 0; j < graph_vertices.size(); j++){
            double v_i = 0;
            double v_j = 0;
            double v_z = 0;

            // repelling forces
            for(auto k = 0; k < graph_vertices.size(); k++){
                if(&graph_vertices[j] != &graph_vertices[k]){
                    delta_x = graph_vertices[j].c_cords[0] - graph_vertices[k].c_cords[0];
                    delta_y = graph_vertices[j].c_cords[1] - graph_vertices[k].c_cords[1];
                    delta_z = graph_vertices[j].c_cords[2] - graph_vertices[k].c_cords[2];
                    mag = sqrt(pow(delta_x,2)+pow(delta_y,2)+pow(delta_z,2));
                    double r_force = (K*K)/mag;
                    v_i += (delta_x/mag)*r_force;
                    v_j += (delta_y/mag)*r_force;
                    v_z += (delta_z/mag)*r_force;        
                }
            }
  
            // attracting forces
            for(auto k = 0; k < graph_vertices[j].neighbours.size(); k++){
                delta_x = graph_vertices[j].neighbours[k]->c_cords[0] - graph_vertices[j].c_cords[0];
                delta_y = graph_vertices[j].neighbours[k]->c_cords[1] - graph_vertices[j].c_cords[1];
                delta_z = graph_vertices[j].neighbours[k]->c_cords[2] - graph_vertices[j].c_cords[2];
                mag = sqrt(pow(delta_x,2)+pow(delta_y,2)+pow(delta_z,2));
                double a_force = (mag*mag)/K;
                v_i += (delta_x/mag)*a_force;
                v_j += (delta_y/mag)*a_force;
                v_z += (delta_z/mag)*a_force;
            }

            mag = sqrt(pow(v_i,2)+pow(v_j,2)+pow(v_z,2));
            
            graph_vertices[j].n_cords[0] = graph_vertices[j].c_cords[0] + (v_i/mag) * min(mag,t);
            graph_vertices[j].n_cords[1] = graph_vertices[j].c_cords[1] + (v_j/mag) * min(mag,t);
            graph_vertices[j].n_cords[2] = graph_vertices[j].c_cords[2] + (v_z/mag) * min(mag,t);

            if(pow(graph_vertices[j].n_cords[0],2)+pow(graph_vertices[j].n_cords[1],2)+pow(graph_vertices[j].n_cords[2],2) > 0.5){
                double mag = sqrt(pow(graph_vertices[j].n_cords[0],2)+pow(graph_vertices[j].n_cords[1],2)+pow(graph_vertices[j].n_cords[2],2));

                graph_vertices[j].n_cords[0] = ((double)0.5/mag)*graph_vertices[j].n_cords[0];
                graph_vertices[j].n_cords[1] = ((double)0.5/mag)*graph_vertices[j].n_cords[1];
                graph_vertices[j].n_cords[2] = ((double)0.5/mag)*graph_vertices[j].n_cords[2];
            }
        }

        for(auto i = 0; i < graph_vertices.size(); i++){
            graph_vertices[i].c_cords[0] = graph_vertices[i].n_cords[0];
            graph_vertices[i].c_cords[1] = graph_vertices[i].n_cords[1];
            graph_vertices[i].c_cords[2] = graph_vertices[i].n_cords[2];
        }

        this_thread::sleep_for(chrono::milliseconds(10));
        t *= 0.1;
        XClearWindow(disp, win);
    } 

    //cin.get();
    //return 0;

    double angle = 0;

    while(true){
        double rad = angle * (M_PI/180);
/*
        for(auto j = 0; j < graph_vertices.size(); j++){
            // rotating
            trans_point1[0] = graph_vertices[j].c_cords[0] * cos(rad) + graph_vertices[j].c_cords[2] * sin(rad); 
            trans_point1[1] = graph_vertices[j].c_cords[1];
            trans_point1[2] = graph_vertices[j].c_cords[0] * (-sin(rad)) + graph_vertices[j].c_cords[2] * cos(rad);

            // translating x and y
            trans_point1[0] += 2.0;
            trans_point1[1] += 2.0;

            // scaling x and y
            trans_point1[0] *= 0.5;
            trans_point1[1] *= 0.5;
            
            // projecting
            proj_point1[0] = trans_point1[0] * (1/tan(fovx/2));
            proj_point1[1] = trans_point1[1] * (1/tan(fovy/2));
            proj_point1[2] = trans_point1[2] * (-((zfar+znear)/(zfar-znear))) + (-(2*(znear*zfar)/(zfar-znear))); 
            double w1 = trans_point1[2] * (-1);
            if(w1 != 0){
                proj_point1[0] /= w1;
                proj_point1[1] /= w1;
            } 
    
            // offsetting/translating???
            proj_point1[0] += 1.0;
            proj_point1[1] += 1.0;

            XDrawArc(disp, win, gc, )            
        }
*/

        // force-directed graph
        for(auto j = 0; j < graph_vertices.size(); j++){
            // rotating
            //trans_point1[0] = graph_vertices[j].c_cords[0] * cos(rad) + (graph_vertices[j].c_cords[2]+3.0) * sin(rad); 
            //trans_point1[1] = graph_vertices[j].c_cords[1];
            //trans_point1[2] = graph_vertices[j].c_cords[0] * (-sin(rad)) + (graph_vertices[j].c_cords[2]+3.0) * cos(rad);

            rot_point1[0] = graph_vertices[j].c_cords[0] * cos(rad) + graph_vertices[j].c_cords[2] * sin(rad); 
            rot_point1[1] = graph_vertices[j].c_cords[1];
            rot_point1[2] = graph_vertices[j].c_cords[0] * (-sin(rad)) + graph_vertices[j].c_cords[2] * cos(rad);

            trans_point1[0] = rot_point1[0];
            trans_point1[1] = rot_point1[1];
            trans_point1[2] = rot_point1[2]+1.0;

            // translating x and y
            //trans_point1[0] += 2.0;
            //trans_point1[1] += 2.0;

            // scaling x and y
            //trans_point1[0] *= 0.5;
            //trans_point1[1] *= 0.5;
            
            // projecting
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
            proj_point1[0] *= 0.5;
            proj_point1[1] *= 0.5;
    
            // offsetting/translating???
            //proj_point1[0] += 2.0;
            //proj_point1[1] += 2.0;      

            for(auto k = 0; k < graph_vertices[j].neighbours.size(); k++){
                // rotating
                //trans_point2[0] = graph_vertices[j].neighbours[k]->c_cords[0] * cos(rad) + (graph_vertices[j].neighbours[k]->c_cords[2]+3.0) * sin(rad); 
                //trans_point2[1] = graph_vertices[j].neighbours[k]->c_cords[1];
                //trans_point2[2] = graph_vertices[j].neighbours[k]->c_cords[0] * (-sin(rad)) + (graph_vertices[j].neighbours[k]->c_cords[2]+3.0) * cos(rad);

                rot_point2[0] = graph_vertices[j].neighbours[k]->c_cords[0] * cos(rad) + graph_vertices[j].neighbours[k]->c_cords[2] * sin(rad); 
                rot_point2[1] = graph_vertices[j].neighbours[k]->c_cords[1];
                rot_point2[2] = graph_vertices[j].neighbours[k]->c_cords[0] * (-sin(rad)) + graph_vertices[j].neighbours[k]->c_cords[2] * cos(rad);

                trans_point2[0] = rot_point2[0];
                trans_point2[1] = rot_point2[1];
                trans_point2[2] = rot_point2[2]+1.0;

                // transalting x and y
                //trans_point2[0] += 2.0;
                //trans_point2[1] += 2.0;

                // scaling x and y
                //trans_point2[0] *= 0.5;
                //trans_point2[1] *= 0.5;

                // projecting
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
                proj_point2[0] *= 0.5;
                proj_point2[1] *= 0.5;

                // offsetting/translating???
                //proj_point2[0] += 2.0;
                //proj_point2[1] += 2.0;

                //cout << proj_point1[0] << " " << proj_point1[1] << endl;
                //cout << proj_point2[0] << " " << proj_point2[1] << endl;

                XDrawLine(disp, win, gc, round(proj_point1[0]*(double)1500), round(proj_point1[1]*(double)1500), 
                    round(proj_point2[0]*(double)1500), round(proj_point2[1]*(double)1500));
                XFlush(disp);
            }
        }

        angle += 0.2;
        angle = (angle / 360) == 1 ? 0 : angle;       
        this_thread::sleep_for(chrono::milliseconds(5));
        XClearWindow(disp, win);
    }
    
    XFreeGC(disp, gc);
    XDestroyWindow(disp, win);
    XCloseDisplay(disp);
    return 0;
}