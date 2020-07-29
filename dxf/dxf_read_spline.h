/*
    This header file is part of CadCam.

    This header file is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This header file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CadCam.  If not, see <https://www.gnu.org/licenses/>.

    AUTOR     : Grotius
    DATE      : June 2020
    REVISION  : 5.6.2020     CHANGES: Added count line spaces before first character
    TODO      : 1. Read in spline's with points outside the spline path. (weighted spline's are not compatible now).
                2. Transfer code to use with std::iterators
    USAGE     : this header file's can read in textfiles with dxf extension, this header file needs also the object.h file.
    PROTOCOL  : 1. Read file line by line.
                2. Save data into object structure.
                3. Prefer to work with std::iterators.
                4. Minimize the use of QT libraries to stay compatible with other program's.
    NOTES     : http://help.autodesk.com/view/ACD/2016/ENU/?guid=GUID-E1F884F8-AA90-4864-A215-3182D47A9C74
                compatible with : AC1027 Inkscape dxf R14 files, AC1021 Librecad dxf 2007 files
*/

#include <variable.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <structs.h>
void rgba_spline(int color);

void read_spline(std::string filename) //dxftype="inkscape" or "librecad"
{

    bool trigger=0;
    unsigned int layer_line=0;
    unsigned int color_line=0;
    unsigned int x_line=0;
    unsigned int y_line=0;
    unsigned int z_line=0;
//    unsigned int x_control_line=0;
//    unsigned int y_control_line=0;
//    unsigned int z_control_line=0;
//    unsigned int x_start_tangent_line=0;
//    unsigned int y_start_tangent_line=0;
//    unsigned int z_start_tangent_line=0;
//    unsigned int x_end_tangent_line=0;
//    unsigned int y_end_tangent_line=0;
//    unsigned int z_end_tangent_line=0;
    std::string layer;
    std::string dxftype="inkscape"; //standard..
//    unsigned int color = 0;
//    double x=0;
//    double y=0;
//    double z=0;

    bool spline_startpoint=1;

    unsigned int linenumber=0;
    char count[100]; //line buffer
    FILE *file=fopen(filename.c_str(), "r"); //convert std::string to std::char

    while(file && fgets(count, sizeof(count), file)){

        //count the spaces in the dxf file
        std::string line=count;
        int a=line.size();
        //std::cout<<"a size: "<<a<<std::endl;
        int spaces=0;
        for(int i=0;i<a;i++){
            if(line[i]==' '){
                spaces++;
                //std::cout<<"spaces: "<<spaces<<std::endl;
            } else {break;}
        }

        linenumber++;

        if(strstr(count,"AC1021")){
            //AC1021 Librecad dxf 2007 files
            dxftype="librecad";
        }

        if(strstr(count,"SPLINE")&& !strstr(count,"$SPLINETYPE")&& !strstr(count,"$SPLINESEGS")){
            //std::cout<<"spline found at:"<<linenumber<< std::endl;
            trigger=1;
            object.type="spline";
        }

        if(trigger && spaces==2 && strstr(count,"  8")){
            //std::cout<<"8 found at:"<<linenumber<<std::endl;
            layer_line=linenumber+1;
        }
        if(trigger && linenumber==layer_line){
            //std::cout<<"layer:"<<count<<std::endl;
            //spline.layer=count;
            object.layer=count;
        }

        if(trigger && spaces==1 &&strstr(count," 62")){
            //std::cout<<"62 found at:"<<linenumber<<std::endl;
            color_line=linenumber+1;
        }
        if(trigger && linenumber==color_line){
            //std::cout<<"color:"<<count<<std::endl;
            rgba_spline(std::stoi(count));
        }

        if(dxftype=="librecad"){
                    //10,20,30 are Control points, cad test
            if(trigger && spaces==1 && strstr(count," 10")){
                //std::cout<<"10 found at:"<<linenumber<<std::endl;
                x_line=linenumber+1;
            }
            if(trigger && linenumber==x_line){
                //std::cout<<"x fit:"<<count<<std::endl;
                if(spline_startpoint){
                    object.start.x=std::stod(count);
                } else {
                    object.control.push_back({0,0,0}); //create controlpoint
                    object.control.back().x=std::stod(count); //fill controlpoint
                }
            }

            if(trigger && spaces==1 && strstr(count," 20")){
                //std::cout<<"20 found at:"<<linenumber<<std::endl;
                y_line=linenumber+1;
            }
            if(trigger && linenumber==y_line){
                //std::cout<<"y fit:"<<count<<std::endl;
                if(spline_startpoint){
                    object.start.y=std::stod(count);
                } else {
                    object.control.back().y=std::stod(count);
                }
            }

            if(trigger && spaces==1 && strstr(count," 30")){
                //std::cout<<"30 found at:"<<linenumber<<std::endl;
                z_line=linenumber+1;
            }
            if(trigger && linenumber==z_line){
                //std::cout<<"z fit:"<<count<<std::endl;
                if(spline_startpoint){
                    object.start.z=std::stod(count);
                } else {
                    object.control.back().z=std::stod(count);
                }

                spline_startpoint=0; //go on with controlpoints
            }
        }

        //        //12,22,32 is start tangent, cad test
        //        if(trigger && strstr(count," 12")){
        //            x_start_tangent_line=linenumber+1;
        //        }
        //        if(trigger && linenumber==x_start_tangent_line){
        //            std::cout<<"x_start_tangent:"<<count<<std::endl;
        //        }

        //        if(trigger && strstr(count," 22")){
        //            y_start_tangent_line=linenumber+1;
        //        }
        //        if(trigger && linenumber==y_start_tangent_line){
        //            std::cout<<"y_start_tangent:"<<count<<std::endl;
        //        }

        //        if(trigger && strstr(count," 32")){
        //            z_start_tangent_line=linenumber+1;
        //        }
        //        if(trigger && linenumber==z_start_tangent_line){
        //            std::cout<<"z_start_tangent:"<<count<<std::endl;
        //        }

        if(dxftype=="inkscape"){
            //11,21,31 are Fit points, is used by Inkscape for drawing the complete spline
            if(trigger && spaces==1 && strstr(count," 11")){
                //cout<<"10 found at:"<<linenumber<< endl;
                x_line=linenumber+1;
            }
            if(trigger && linenumber==x_line){
                //std::cout<<"x fit:"<<count<<std::endl;
                if(spline_startpoint){
                    object.start.x=std::stod(count);
                } else {
                    object.control.push_back({0,0,0}); //create controlpoint
                    object.control.back().x=std::stod(count); //fill controlpoint
                }

            }

            if(trigger && spaces==1 && strstr(count," 21")){
                //std::cout<<"20 found at:"<<linenumber<<std::endl;
                y_line=linenumber+1;
            }
            if(trigger && linenumber==y_line){
                //std::cout<<"y fit:"<<count<<std::endl;
                if(spline_startpoint){
                    object.start.y=std::stod(count);
                } else {
                    object.control.back().y=std::stod(count);
                }
            }

            if(trigger && spaces==1 && strstr(count," 31")){
                //std::cout<<"30 found at:"<<linenumber<<std::endl;
                z_line=linenumber+1;
            }
            if(trigger && linenumber==z_line){
                //std::cout<<"z fit:"<<count<<std::endl;
                if(spline_startpoint){
                    object.start.z=std::stod(count);
                } else {
                    object.control.back().z=std::stod(count);
                }

                spline_startpoint=0; //go on with controlpoints
            }
        }

        if(trigger && spaces==2 && strstr(count,"  0")){
            //std::cout<<"0 found at:"<<linenumber<<std::endl;

            //endpoint coordinates from last controlpoint
            object.end.x=object.control.back().x;
            object.end.y=object.control.back().y;
            object.end.z=object.control.back().z;

            //erase last controlpoint
            object.control.pop_back();

            //when no colorcode " 62" is found..
            if(int(object.red)==0 && int(object.green)==0 && int(object.blue)==0 && int(object.alpha)==0){
                object.red = 255;
                object.green = 255;
                object.blue = 255;
                object.alpha = 255;
            }

            cad.push_back(object); //save program data

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
            spline_startpoint=1;
        }
    }
}

void rgba_spline(int color)
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








