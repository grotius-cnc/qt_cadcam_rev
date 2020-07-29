#include <string>
#include <variable.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <structs.h>
#include <algorithm>

bool trigger=0;
bool trigger_end=0; //trigger the end after we got the last y value
unsigned int layer_line=0;
unsigned int color_line=0;
unsigned int x_start_line=0;
unsigned int y_start_line=0;
unsigned int z_start_line=0;
unsigned int x_end_line=0;
unsigned int y_end_line=0;
unsigned int z_end_line=0;

void rgba_line(int color);

void read_line(std::string filename){

    std::string stdfilename = std::string(filename); //convert wxstring to std::string
    unsigned int linenumber=0;
    char count[100];
    FILE *file=fopen(stdfilename.c_str(), "r"); //convert std::string to std::char

    while(file && fgets(count, sizeof(count), file)){
        linenumber++;

        if(strstr(count,"LINE") && !strstr(count,"SPLINE") && !strstr(count,"ACAD_MLINESTYLE") && !strstr(count,"AcDbLine")
                && !strstr(count,"LWPOLYLINE") && !strstr(count,"$PLINEWID") && !strstr(count,"$SPLINETYPE")
                && !strstr(count,"ACAD_MLINESTYLE") && !strstr(count,"$PLINEGEN") && !strstr(count,"MLINESTYLE")
                && !strstr(count,"$SPLINESEGS") && !strstr(count,"AcDbLinetypeTableRecord") && !strstr(count,"Solid line") && !strstr(count,"fold lines")){
            //std::cout<<"line found at:"<<linenumber<< std::endl;
            trigger=1;
            trigger_end=0;
            object.type="line";
        }

        if(trigger && strstr(count,"  8")){
            //std::cout<<"8 found at:"<<linenumber<<std::endl;
            layer_line=linenumber+1;
        }
        if(trigger && linenumber==layer_line){
            //cout<<"layer:"<<count<< endl;
            object.layer=count;
        }

        if(trigger && strstr(count," 62")){
            //std::cout<<"62 found at:"<<linenumber<<std::endl;
            color_line=linenumber+1;
        }
        if(trigger && linenumber==color_line){
            //cout<<"color:"<<count<< endl;
            rgba_line(std::stoi(count));
        }

        if(trigger && strstr(count," 10")){
            //std::cout<<"10 found at:"<<linenumber<<std::endl;
            x_start_line=linenumber+1;
        }
        if(trigger && linenumber==x_start_line){
            //std::cout<<"x start:"<<count<<std::endl;
            object.start.x=std::stod(count);
        }

        if(trigger && strstr(count," 20")){
            //std::cout<<"20 found at:"<<linenumber<<std::endl;
            y_start_line=linenumber+1;
        }
        if(trigger && linenumber==y_start_line){
            //cout<<"y start:"<<count<< endl;
            object.start.y=std::stod(count);
        }

        if(trigger && strstr(count," 30")){
            //std::cout<<"30 found at:"<<linenumber<<std::endl;
            z_start_line=linenumber+1;
        }
        if(trigger && linenumber==z_start_line){
            //cout<<"z start:"<<count<< endl;
            object.start.z=std::stod(count);
        }

        if(trigger && strstr(count," 11")){
            //std::cout<<"11 found at:"<<linenumber<<std::endl;
            x_end_line=linenumber+1;
        }
        if(trigger && linenumber==x_end_line){
            //cout<<"x end:"<<count<< endl;
            object.end.x=std::stod(count);
        }

        if(trigger && strstr(count," 21")){
            //std::cout<<"21 found at:"<<linenumber<<std::endl;
            y_end_line=linenumber+1;
        }
        if(trigger && linenumber==y_end_line){
            //cout<<"y end:"<<count<< endl;
            object.end.y=std::stod(count);
            trigger_end=1;
        }

        if(trigger && strstr(count," 31")){
            //std::cout<<"31 found at:"<<linenumber<<std::endl;
            z_end_line=linenumber+1;
        }
        if(trigger && linenumber==z_end_line){
            //cout<<"z end:"<<count<< endl;
            object.end.z=std::stod(count);
        }

        if(trigger && trigger_end && strstr(count,"  0") ){

            //when no colorcode " 62" is found..
            if(object.red==0 && object.green==0 && object.blue==0 && object.alpha==0){
                object.red = 255;
                object.green = 255;
                object.blue = 255;
                object.alpha = 255;
            }
            object.control.reserve(2);  //grotius add 16.07.2020
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

            trigger=0;
        }
    }
}

void rgba_line(int color)
{
    if(color == 1){ //red
        object.red = 255;
        object.green = 0;
        object.blue = 0;
        object.alpha = 255;
    } else if(color == 2){ //yellow
        object.red = 255;
        object.green = 255;
        object.blue = 0;
        object.alpha = 255;
    } else if(color == 3){ //green
        object.red = 0;
        object.green = 255;
        object.blue = 0;
        object.alpha = 255;
    } else if(color == 4){ //cyan
        object.red = 255;
        object.green = 0;
        object.blue = 255;
        object.alpha = 255;
    } else if(color == 5){ //blue
        object.red = 49;
        object.green = 112;
        object.blue = 206;
        object.alpha = 255;
    } else if(color == 6){ //magenta
        object.red = 0;
        object.green = 255;
        object.blue = 255;
        object.alpha = 255;
    } else if(color == 7){ //white
        object.red = 255;
        object.green = 255;
        object.blue = 255;
        object.alpha = 255;
    } else if(color == 8){ //grey
        object.red = 106;
        object.green = 106;
        object.blue = 106;
        object.alpha = 255;
    } else if(color == 9){ //orange/brown
        object.red = 255;
        object.green = 170;
        object.blue = 0;
        object.alpha = 255;
    } else {
        object.red = 255; //when no standard colorcode is found we use white
        object.green = 255;
        object.blue = 255;
        object.alpha = 255;
    }
}

