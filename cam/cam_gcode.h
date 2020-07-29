#ifndef CAM_GCODE_H
#define CAM_GCODE_H
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
    DATE      : 26.07.2020
    REVISION  :
    TESTED    :
    TODO      :
    USAGE     :
    PROTOCOL  :
    NOTES     : Control your gcode output with : https://ncviewer.com/

*/

#include <iostream>
#include <algorithm>
#include <structs.h>
#include <math.h>
#include <thread>
#include <fstream>
#include <unistd.h> //user for find current directory
#include <variable.h>
#include <iomanip> // std::setprecision

void area();
double sum_plus_controlpoints(int i);
void map_cw_ccw();
void swap_contour(std::vector<CONTOUR>::iterator it);
std::string get_current_dir();

void generate_gcode(std::string filename, std::string intro, std::string outtro, int precision, std::string delay, std::string line_number_symbol, bool use_line_numbers){

    int linenumber=0;

    if(use_line_numbers){ //append line numbers to intro and outtro
        std::string str;
        for (std::string::iterator it=intro.begin(); it<intro.end(); it++){
            if(it==intro.begin()){ //add before first line
                str+=line_number_symbol+std::to_string(linenumber++)+" ";
            }
            str.push_back(*it);
            if (*it=='\n'){ //add in the text string
                str+=line_number_symbol+std::to_string(linenumber++)+" ";
            }
        }
        intro.clear();
        intro=str;
    }

    double travelheight=0;
    double plungeheight=0;
    double workheight=0;
    double plungespeed=0;
    double feedrate=0;
    double rpm=0;
    bool drill_operation=0;
    unsigned int retract_times=0;
    double retract_dist=0;
    bool use_retract=0;
    double offset_x=0; //tooloffset head
    double offset_y=0;
    std::string axis;
    std::string tooltype;

    bool m3=0;
    //double start_x=0, start_y=0;

    std::ofstream myfile;
    myfile.open(filename, std::ios::app); //ios app = append true
    myfile.precision(precision);
    myfile << std::fixed;
    myfile<<intro<<std::endl;

    for(unsigned int j=0; j<jobs.size(); j++){
        std::cout<<"jobs_size: "<<jobs.size()<<std::endl;

        for(unsigned int i=0; i<cam.size(); i++){

            if(cam.at(i).job==j){

                //find head xy tool offset
                for(unsigned int k=0; k<heads.size(); k++){
                    if(heads.at(k).axis==jobs.at(j).head){
                        offset_x=heads.at(k).offset_x;
                        offset_y=heads.at(k).offset_y;
                        axis=heads.at(k).axis;
                        axis.erase(std::remove(axis.begin(), axis.end(), '\n'),axis.end());
                    }
                }

                if(m3==0){
                    m3=1;
                    //start_x=cam.at(i).start.x+offset_x;
                    //start_y=cam.at(i).start.y+offset_y;

                    //find travelheight + plungeheight in tool struct library
                    for(unsigned int k=0; k<tools.size(); k++){
                        if(job.tool==tools.at(k).toolname){
                            travelheight=tools.at(k).travelheight;
                            workheight=tools.at(k).workheight;
                            plungeheight=tools.at(k).plungeheight;
                            plungespeed=tools.at(k).plungespeed;
                            feedrate=tools.at(k).feedrate;
                            rpm=tools.at(k).rpm;


                            tooltype=tools.at(k).tooltype;
                            tooltype.erase(std::remove(tooltype.begin(), tooltype.end(), '\n'),tooltype.end());
                            if(tooltype=="Drill"){
                                drill_operation=1;
                                std::cout<<"drill operation"<<std::endl;
                                retract_times=tools.at(k).retract_times;
                                retract_dist=abs(tools.at(k).retract_distance);
                                use_retract=tools.at(k).use_retract;
                            }
                        }
                    }

                    if(use_line_numbers){
                        if(drill_operation){
                            myfile<<line_number_symbol<<linenumber++<<" G0 X"<<cam.at(i).center.x+offset_x << " Y" << cam.at(i).center.y+offset_y << " " << axis << travelheight <<std::endl;
                        } else {
                            myfile<<line_number_symbol<<linenumber++<<" G0 X"<<cam.at(i).start.x+offset_x << " Y" << cam.at(i).start.y+offset_y << " " << axis << travelheight <<std::endl;
                        }
                        myfile<<line_number_symbol<<linenumber++<<" G0 "<< axis << plungeheight << " (plunge-pierce height)"<<std::endl;
                    } else {
                        if(drill_operation){
                            myfile<<"G0 X"<<cam.at(i).center.x+offset_x << " Y" << cam.at(i).center.y+offset_y << " " << axis << travelheight <<std::endl;
                        } else {
                            myfile<<"G0 X"<<cam.at(i).start.x+offset_x << " Y" << cam.at(i).start.y+offset_y << " " << axis << travelheight <<std::endl;
                        }
                        myfile<<"G0 " << axis << plungeheight << " (plunge-pierce height)"<<std::endl;
                    }

                    //tool on, find correct M code in heads struct library
                    for(unsigned int k=0; k<heads.size(); k++){
                        if(heads.at(k).axis==jobs.at(j).head){
                            std::string m3_str=heads.at(k).m_code_start;
                            m3_str.erase(std::remove(m3_str.begin(), m3_str.end(), '\n'),m3_str.end());

                            if(use_line_numbers){
                                myfile<<line_number_symbol<<linenumber++<<" "<<m3_str<<" S"<<rpm<<" (tool on)"<<std::endl;
                            } else {
                                myfile<<m3_str<<" S"<<rpm<<" (tool on)"<<std::endl;
                            }
                        }
                    }

                    //find tool delay
                    for(unsigned int k=0; k<tools.size(); k++){
                        if(job.tool==tools.at(k).toolname){
                            if(tools.at(k).delay>0){ //don't add empty tool time to gcode
                                if(use_line_numbers){
                                    myfile<<line_number_symbol<<linenumber++<<" "<<delay<<tools.at(k).delay<<" (delay)"<<std::endl;
                                } else {
                                    myfile<<delay<<tools.at(k).delay<<" (delay)"<<std::endl;
                                }
                            }
                        }
                    }

                    //check if it is a drill operation, if yes, use drill rectract option if enabled.
                    if(drill_operation){

                        double drill_down=abs(workheight)/retract_times; //workheight = drilldepth
                        double height=0; //height = top of workpiece

                        if(use_retract){
                            for(unsigned int i=0; i<retract_times; i++){
                                std::cout<<"uses drill retract"<<std::endl;
                                height-=drill_down;
                                if(use_line_numbers){
                                    myfile<<line_number_symbol<<linenumber++<<" G1 "<< axis << height <<" F"<<plungespeed<<std::endl;
                                    myfile<<line_number_symbol<<linenumber++<<" G0 "<< axis << height+retract_dist<<std::endl;
                                    myfile<<line_number_symbol<<linenumber++<<" G0 "<< axis << height<<std::endl;
                                } else {
                                    myfile<<"G1 " << axis << height<<" F"<<plungespeed<<std::endl;
                                    myfile<<"G0 " << axis << height+retract_dist<<std::endl;
                                    myfile<<"G0 " << axis << height<<std::endl;
                                }
                            }
                        } else { //no retract, just drill the hole at once.
                            if(use_line_numbers){
                                myfile<<line_number_symbol<<linenumber++<<" G1 "<< axis << workheight<<" F"<<plungespeed<<std::endl;
                            } else {
                                myfile<<"G1 "<< axis << workheight<<" F"<<plungespeed<<std::endl;
                            }
                        }

                    } else { //normal operation

                        if(use_line_numbers){
                            myfile<<line_number_symbol<<linenumber++<<" G1 "<< axis << workheight<<" F"<<plungespeed<<" (cut depth)"<<std::endl;
                            myfile<<line_number_symbol<<linenumber++<<" F"<<feedrate<<std::endl;
                        } else {
                            myfile<<"G1 "<< axis << workheight <<" F"<< plungespeed <<" (cut depth)"<<std::endl;
                            myfile<<"F"<< feedrate <<std::endl;
                        }
                    }
                }

                // ---- normal loop :

                //line section
                if(cam.at(i).type=="line"){
                    if(use_line_numbers){
                        myfile<<line_number_symbol<<linenumber++<<" G1 X"<<cam.at(i).end.x+offset_x<<" Y"<<cam.at(i).end.y+offset_y<</*" " << axis << workheight<<*/std::endl;
                    } else {
                        myfile<<"G1 X"<<cam.at(i).end.x+offset_x<<" Y"<<cam.at(i).end.y+offset_y<</*" "<< axis << workheight<<*/std::endl;
                    }
                }

                //arc section
                //X=xend, Y=yend. For arc given a G0 startposition and a XY endposition. http://linuxcnc.org/docs/html/gcode/g-code.html#gcode:g2-g3
                //I=offset xcenter-xstart, J=offset ycenter-ystart, G2=clockwise (cw), G3=counterclockwise (ccw)
                if(cam.at(i).type=="arc" || cam.at(i).type=="circle"){
                    if(cam.at(i).d<0){  //bulge 0=line, bulge neg=g2, bulge pos=g3
                        if(use_line_numbers){
                            myfile<<line_number_symbol<<linenumber++<<" G2 X"<<cam.at(i).end.x+offset_x<<" Y"<<cam.at(i).end.y+offset_y<<" I"<<(cam.at(i).center.x-cam.at(i).start.x)+offset_x<<" J"<<(cam.at(i).center.y-cam.at(i).start.y)+offset_y<</*" " << axis << workheight<<*/std::endl;
                        } else {
                            myfile<< "G2 X"<<cam.at(i).end.x+offset_x<<" Y"<<cam.at(i).end.y+offset_y<<" I"<<(cam.at(i).center.x-cam.at(i).start.x)+offset_x<<" J"<<(cam.at(i).center.y-cam.at(i).start.y)+offset_y<<std::endl;
                        }
                    }
                    if(cam.at(i).d>0){ //bulge 0=line, bulge neg=g2, bulge pos=g3
                        if(use_line_numbers){
                            myfile<<line_number_symbol<<linenumber++<<" G3 X"<<cam.at(i).end.x+offset_x<<" Y"<<cam.at(i).end.y+offset_y<<" I"<<(cam.at(i).center.x-cam.at(i).start.x)+offset_x<<" J"<<(cam.at(i).center.y-cam.at(i).start.y)+offset_y<</*" " << axis << workheight<<*/std::endl;
                        } else {
                            myfile<< "G3 X"<<cam.at(i).end.x+offset_x<<" Y"<<cam.at(i).end.y+offset_y<<" I"<<(cam.at(i).center.x-cam.at(i).start.x)+offset_x<<" J"<<(cam.at(i).center.y-cam.at(i).start.y)+offset_y<<std::endl;
                        }
                    }
                }

                //end contour
                if( /*(m3==1 && start_x==cam.at(i).end.x+offset_x && start_y==cam.at(i).end.y+offset_y) ||*/(m3==1 && drill_operation) || (m3==1 && cam.at(i).end_contour==1)){ //spotted the end of current job contour, or spotted a drill operation
                    drill_operation=0;
                    m3=0; //next contour needs a start

                    //for milling and drilling, not for plasma or oxyfuel, first retract spindle to save height, then splindle off.
                    if(tooltype=="Mill" || tooltype=="Drill"){
                        if(use_line_numbers){
                            myfile<<line_number_symbol<<linenumber++<<" G0 "<< axis <<travelheight<<std::endl;
                        } else {
                            myfile<<"G0 "<< axis <<travelheight<<std::endl;
                        }
                    }

                    //tool off, find correct M code in heads struct library
                    for(unsigned int k=0; k<heads.size(); k++){
                        if(heads.at(k).axis==jobs.at(j).head){
                            std::string str=heads.at(k).m_code_stop;
                            str.erase(std::remove(str.begin(), str.end(), '\n'),str.end());
                            if(use_line_numbers){
                                myfile<<line_number_symbol<<linenumber++<<" "<<str+" (tool off)"<<std::endl;
                            } else {
                                myfile<<str+" (tool off)"<<std::endl;
                            }
                        }
                    }

                    //for plasma and oxyfuel, first stop plasma source, then retract to save height.
                    if(tooltype=="Plasma-laser" || tooltype=="Oxyfuel"){
                        if(use_line_numbers){
                            myfile<<line_number_symbol<<linenumber++<<" G0 "<< axis <<travelheight<<std::endl;
                        } else {
                            myfile<<"G0 "<< axis <<travelheight<<std::endl;
                        }
                    }
                }
            }
        }
    }

    if(use_line_numbers){ //add line numbers to outtro
        std::string str;
        for (std::string::iterator it=outtro.begin(); it<outtro.end(); it++){
            if(it==outtro.begin()){ //add before first line
                str+=line_number_symbol+std::to_string(linenumber++)+" ";
            }
            str.push_back(*it);
            if (*it=='\n'){ //add in the text string
                str+=line_number_symbol+std::to_string(linenumber++)+" ";
            }
        }
        outtro.clear();
        outtro=str;
    }

    myfile<<outtro<<std::endl;
    myfile.close();
}

void area(){
    std::vector<CONTOUR>::iterator it;
    std::vector<int>::iterator it1;
    for(it=contours.begin(); it<contours.end(); it++){
        double sum=0;
        if(it->contourtype!=0){  //0=open, 1=closed, 2=single object closed
            for(it1=it->sequence.begin();  it1<it->sequence.end(); it1++){
                sum+=sum_plus_controlpoints(*it1);
            }
        }
        it->area=sum/2;
        //std::cout<<"area: "<<sum/2<<std::endl;
    }
}

double sum_plus_controlpoints(int i){
    double sum=0;
    //if object has controlpoints
    if(cad.at(i).control.size()>0){
        sum+= (cad.at(i).control.front().x-cad.at(i).start.x) * (cad.at(i).control.front().y+cad.at(i).start.y);
        for(int j=0; j<cad.at(i).control.size()-1; j++){
            sum+= (cad.at(i).control.at(j+1).x-cad.at(i).control.at(j).x) * (cad.at(i).control.at(j+1).y+cad.at(i).control.at(j).y);
        }
        sum+= (cad.at(i).end.x-cad.at(i).control.back().x) * (cad.at(i).end.y+cad.at(i).control.back().y);
    }

    //if object has no controlpoints
    if(cad.at(i).control.size()==0){
        sum+= (cad.at(i).end.x-cad.at(i).start.x) * (cad.at(i).end.y+cad.at(i).start.y);
    }
    return sum;
}

void map_cw_ccw(){
    std::vector<CONTOUR>::iterator it;
    for(it=contours.begin(); it<contours.end(); it++){
        if(it->area==0){
            //do nothing, open contour
        }
        if(it->area>0){
            //cw contour, check if contour color is red, if not, swap contour
            int object=it->sequence.front();

            if(cad.at(object).red==255 && cad.at(object).green==0 && cad.at(object).blue==0 && cad.at(object).alpha==255){
                //std::cout<<"cw contour, color is red, ok"<<std::endl;
            }
            if(cad.at(object).red==255 && cad.at(object).green==255 && cad.at(object).blue==0 && cad.at(object).alpha==255){
                //std::cout<<"cw contour, color is yellow, not ok, swap"<<std::endl;
                swap_contour(it);
                it->area=it->area*-1; //keep the area value in line with the color
            }
        }
        if(it->area<0){
            int object=it->sequence.front();

            if(cad.at(object).red==255 && cad.at(object).green==0 && cad.at(object).blue==0 && cad.at(object).alpha==255){
                //std::cout<<"ccw contour, color is red, not ok, swap"<<std::endl;
                swap_contour(it);
                it->area=it->area*-1;
            }
            if(cad.at(object).red==255 && cad.at(object).green==255 && cad.at(object).blue==0 && cad.at(object).alpha==255){
                //std::cout<<"ccw contour, color is yellow, ok"<<std::endl;
            }
        }
    }
}
void swap_contour(std::vector<CONTOUR>::iterator it){
    std::vector<int>::iterator it1;
    std::reverse(it->sequence.begin(), it->sequence.end());
    for(it1=it->sequence.begin(); it1<it->sequence.end(); it1++){
        //swap it1 parameters
        double start_x = cad.at(*it1).start.x;
        double start_y = cad.at(*it1).start.y;
        double start_z = cad.at(*it1).start.z;
        cad.at(*it1).start.x=cad.at(*it1).end.x;
        cad.at(*it1).start.y=cad.at(*it1).end.y;
        cad.at(*it1).start.z=cad.at(*it1).end.z;
        cad.at(*it1).end.x=start_x;
        cad.at(*it1).end.y=start_y;
        cad.at(*it1).end.z=start_z;
        std::reverse(cad.at(*it1).control.begin(), cad.at(*it1).control.end());
    }
}

std::string get_current_dir() { //use std:: libs instead of qt libs
    char buff[FILENAME_MAX]; //create string buffer to hold path
    getcwd( buff, FILENAME_MAX );
    std::string current_working_dir(buff);
    return current_working_dir;
}

#endif // CAM_GCODE_H

















