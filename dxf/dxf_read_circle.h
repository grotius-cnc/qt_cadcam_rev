#include <variable.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <structs.h>
#include <math.h>
#include <algorithm>

void controlpoints();
void rgba_cirlce(int color);

void read_circle(std::string filename)
{
    bool trigger=0;
    unsigned int layer_line=0;
    unsigned int color_line=0;
    unsigned int x_start_line=0;
    unsigned int y_start_line=0;
    unsigned int z_start_line=0;
    unsigned int radius_line=0;

    unsigned int linenumber=0;
    char count[100];
    FILE *file=fopen(filename.c_str(), "r"); //convert std::string to std::char

    while(file && fgets(count, sizeof(count), file)){
        linenumber++;

        if(strstr(count,"CIRCLE")){
            //cout<<"lwline found at:"<<linenumber<< endl;
            trigger=1;
            object.type="circle";
        }

        if(trigger && strstr(count,"  8")){
            //cout<<"8 found at:"<<linenumber<< endl;
            layer_line=linenumber+1;
        }
        if(trigger && linenumber==layer_line){
            //cout<<"layer:"<<count<< endl;
            object.layer=count;
        }

        if(trigger && strstr(count," 62")){
            //cout<<"62 found at:"<<linenumber<< endl;
            color_line=linenumber+1;
        }
        if(trigger && linenumber==color_line){
            //cout<<"color:"<<count<< endl;
            rgba_cirlce(std::stoi(count));
        }

        if(trigger && strstr(count," 10")){
            //cout<<"10 found at:"<<linenumber<< endl;
            x_start_line=linenumber+1;
        }
        if(trigger && linenumber==x_start_line){
            //cout<<"x start:"<<count<< endl;
            object.center.x=std::stod(count);
        }

        if(trigger && strstr(count," 20")){
            //cout<<"20 found at:"<<linenumber<< endl;
            y_start_line=linenumber+1;
        }
        if(trigger && linenumber==y_start_line){
            //cout<<"y start:"<<count<< endl;
            object.center.y=std::stod(count);
        }

        if(trigger && strstr(count," 30")){
            //cout<<"30 found at:"<<linenumber<< endl;
            z_start_line=linenumber+1;
        }
        if(trigger && linenumber==z_start_line){
            //cout<<"z start:"<<count<< endl;
            object.center.z=std::stod(count);
        }

        if(trigger && strstr(count," 40")){
            //cout<<"11 found at:"<<linenumber<< endl;
            radius_line=linenumber+1;
        }
        if(trigger && linenumber==radius_line){
            //cout<<"x end:"<<count<< endl;
            object.radius=std::stod(count);

            object.start.x=object.center.x+object.radius;
            object.start.y=object.center.y;
            object.end.x=object.center.x+object.radius;
            object.end.y=object.center.y;

            //when no colorcode " 62" is found..
            if(object.red==0 && object.green==0 && object.blue==0 && object.alpha==0){
                object.red = 255;
                object.green = 255;
                object.blue = 255;
                object.alpha = 255;
            }

            controlpoints();
            cad.push_back(object);

            //reset object value's
            object.type="";
            object.layer="";
            object.red=0;
            object.green=0;
            object.blue=0;
            object.alpha=0;
            object.start.x=0;
            object.start.y=0;
            object.start.z=0;
            object.control.clear();
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

void rgba_cirlce(int color)
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

void controlpoints(){
        int segments = 14;
        double radius = sqrt(pow(object.end.x-object.center.x,2)+pow(object.end.y-object.center.y,2));
        for (int j = 1; j<segments; j++){ //ccw direction
            double theta = 2.0 * M_PI * int(j) / int(segments);
            double x = radius * cosf(theta);
            double y = radius * sinf(theta);
            object.control.push_back({x+object.center.x,y+object.center.y,object.center.z});
            //std::cout<<"control x:"<<x+object.center.x<<" control y"<<y+object.center.y<<std::endl;
        }
        //morph circle into cw
        std::reverse(object.control.begin(),object.control.end());

        object.radius=radius;
}









