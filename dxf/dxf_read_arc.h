#include <variable.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <structs.h>
#include <math.h>
#include <algorithm>

void rgba_arc(int color);
void calculate_arc_start_arc_end(double xcen, double ycen, double zcen, double deg_start_angle, double deg_end_angle, double radius);

void read_arc(std::string filename)
{
    bool trigger=0;
    double deg_angle_start=0;
    double deg_angle_end=0;
    unsigned int layer_line=0;
    unsigned int color_line=0;
    unsigned int x_start_line=0;
    unsigned int y_start_line=0;
    unsigned int z_start_line=0;
    unsigned int radius_line=0;
    unsigned int start_angle_line=0;
    unsigned int end_angle_line=0;

    unsigned int linenumber=0;
    char count[100]; //line buffer
    FILE *file=fopen(filename.c_str(), "r"); //convert std::string to std::char

    while(file && fgets(count, sizeof(count), file)){
        linenumber++;

        if(strstr(count,"ARC") && !strstr(count,"$DIMARCSYM")){
            //cout<<"lwline found at:"<<linenumber<< endl;
            trigger=1;
            object.type="arc";
        }

        if(trigger && strstr(count,"  8")){
            //cout<<"8 found at:"<<linenumber<< endl;
            layer_line=linenumber+1;
        }
        if(trigger && linenumber==layer_line){
            object.layer=count;
            //cout<<"layer:"<<count<< endl;
        }

        if(trigger && strstr(count," 62")){
            //cout<<"62 found at:"<<linenumber<< endl;
            color_line=linenumber+1;
        }
        if(trigger && linenumber==color_line){
            //cout<<"color:"<<count<< endl;
            rgba_arc(std::stoi(count));
        }

        if(trigger && strstr(count," 10")){
            //cout<<"10 found at:"<<linenumber<< endl;
            x_start_line=linenumber+1;
        }
        if(trigger && linenumber==x_start_line){
            //cout<<"x start:"<<count<< endl;
            //object.xcenter=std::stod(count);
            object.center.x=std::stod(count);
        }

        if(trigger && strstr(count," 20")){
            //cout<<"20 found at:"<<linenumber<< endl;
            y_start_line=linenumber+1;
        }

        if(trigger && linenumber==y_start_line){
            //cout<<"y start:"<<count<< endl;
            //object.ycenter=std::stod(count);
            object.center.y=std::stod(count);
        }

        if(trigger && strstr(count," 30")){
            //cout<<"30 found at:"<<linenumber<< endl;
            z_start_line=linenumber+1;
        }
        if(trigger && linenumber==z_start_line){
            //cout<<"z start:"<<count<< endl;
            //object.zcenter=std::stod(count);
            object.center.z=std::stod(count);
        }

        if(trigger && strstr(count," 40")){
            //cout<<"11 found at:"<<linenumber<< endl;
            radius_line=linenumber+1;
        }
        if(trigger && linenumber==radius_line){
            //cout<<"x end:"<<count<< endl;
            object.radius=std::stod(count);
        }

        if(trigger && strstr(count," 50")){
            //cout<<"21 found at:"<<linenumber<< endl;
            start_angle_line=linenumber+1;
        }
        if(trigger && linenumber==start_angle_line){
            //cout<<"y end:"<<count<< endl;
            deg_angle_start=std::stod(count);
        }

        if(trigger && strstr(count," 51")){
            //cout<<"31 found at:"<<linenumber<< endl;
            end_angle_line=linenumber+1;
        }
        if(trigger && linenumber==end_angle_line){
            //cout<<"z end:"<<count<< endl;
            deg_angle_end=std::stod(count);

            calculate_arc_start_arc_end(object.center.x,object.center.y,object.center.z,deg_angle_start,deg_angle_end,object.radius);

            //when no colorcode " 62" is found..
            if(object.red==0 && object.green==0 && object.blue==0 && object.alpha==0){
                object.red = 255;
                object.green = 255;
                object.blue = 255;
                object.alpha = 255;
            }

            //calculate the determinant.
            //double xp = object.end.x;
            //double yp = object.end.y;
            //double x1 = object.start.x;
            //double y1 = object.start.y;
            //double x2 = object.control.front().x;
            //double y2 = object.control.front().y;
            //object.d = ((xp-x1)*(y2-y1))-((yp-y1)*(x2-x1));

            cad.push_back(object);

            //reset object value's
            object.control.clear();
            object.type="";
            object.layer="";
            object.red=0;
            object.green=0;
            object.blue=0;
            object.alpha=0;
            object.start.x=0;
            object.start.y=0;
            object.start.z=0;
            object.end.x=0;
            object.end.y=0;
            object.end.z=0;
            object.center.x=0;
            object.center.y=0;
            object.center.z=0;
            object.radius=0;
            object.deg_angle_start=0;
            object.deg_angle_end=0;
            trigger=0;
        }
    }
}

void calculate_arc_start_arc_end(double xcen, double ycen, double zcen, double deg_start_angle, double deg_end_angle, double radius){ //create arc start-, end-, control-points from centerpoint, start-, end- angle

    if(deg_end_angle==360)
    {
        deg_end_angle=0;   //autocad degree problem..
    }

    //draw arc..
    double pi_angle_start = (deg_start_angle/360)*(2*M_PI);
    double pi_angle_end = (deg_end_angle/360)*(2*M_PI);
    if (pi_angle_end<pi_angle_start)   //this to avoid the start angle is bigger then the end angle value
    {
        pi_angle_end+=2*M_PI;
    }
    double pi_angle_arc = pi_angle_end - pi_angle_start;
    double pi_angle_half = (pi_angle_end + pi_angle_start)/2;

    object.start.x=(radius*cosf(pi_angle_start))+xcen;
    object.start.y=(radius*sinf(pi_angle_start))+ycen;
    object.start.z=zcen;

    object.control.push_back({(radius*cosf(pi_angle_half))+xcen,(radius*sinf(pi_angle_half))+ycen,zcen}); //create new controlpoint

    object.end.x=(radius*cosf(pi_angle_end))+xcen;
    object.end.y=(radius*sinf(pi_angle_end))+ycen;
    object.end.z=zcen;
}

void rgba_arc(int color)
{
    if(color == 1){ //red
        object.red = 255; //red
        object.green = 0; //green
        object.blue = 0; //blue
        object.alpha = 255; //alpha
    } else if(color == 2){ //yellow
        object.red = 255; //red
        object.green = 255; //green
        object.blue = 0; //blue
        object.alpha = 255; //alpha
    } else if(color == 3){ //green
        object.red = 0; //red
        object.green = 255; //green
        object.blue = 0; //blue
        object.alpha = 255; //alpha
    } else if(color == 4){ //cyan
        object.red = 255; //red
        object.green = 0; //green
        object.blue = 255; //blue
        object.alpha = 255; //alpha
    } else if(color == 5){ //blue
        object.red = 49; //red
        object.green = 112; //green
        object.blue = 206; //blue
        object.alpha = 255; //alpha
    } else if(color == 6){ //magenta
        object.red = 0; //red
        object.green = 255; //green
        object.blue = 255; //blue
        object.alpha = 255; //alpha
    } else if(color == 7){ //white
        object.red = 255; //red
        object.green = 255; //green
        object.blue = 255; //blue
        object.alpha = 255; //alpha
    } else if(color == 8){ //grey
        object.red = 106; //red
        object.green = 106; //green
        object.blue = 106; //blue
        object.alpha = 255; //alpha
    } else if(color == 9){ //orange/brown
        object.red = 255; //red
        object.green = 170; //green
        object.blue = 0; //blue
        object.alpha = 255; //alpha
    } else {
        object.red = 255; //when no standard colorcode is found we use white
        object.green = 255;
        object.blue = 255;
        object.alpha = 255;
    }
}
