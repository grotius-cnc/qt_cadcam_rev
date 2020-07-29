#ifndef FILE_CAM_OFFSET
#define FILE_CAM_OFFSET
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
    DATE      : 14.06.2020
    REVISION  : --
    TODO      : --
    USAGE     : --
    PROTOCOL  : --
    NOTES     : --
*/
#include <iostream>
#include <algorithm>
#include <structs.h>
#include <math.h>
#include <lib/lib_intersect.h>
#include <lib/lib_offset.h>
#include <lib/lib_split.h>
#include <lib/lib_pocket.h>
#include <lib/lib_connect.h>
#include <lib/lib_matrix.h>
#include <lib/lib_convert.h>
#include <CavalierContours/include/cavc/polylineoffset.hpp>
#include <CavalierContours/include/cavc/polylineoffsetislands.hpp>

#include <thread>
#include <fstream>
#include <unistd.h> //user for find current directory
std::string get_current_dir_();

#include <iostream>
#include <fstream>
#include <string.h>

std::vector<OBJECT> find_intersections(std::vector<OBJECT> array);
OBJECT construct_arc(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1, double d, double offset);
std::vector<OBJECT> connect_objects(std::vector<OBJECT> array);
OBJECT raw_offset(std::vector<OBJECT>::iterator it);
POINT bulge_to_arc_controlpoint(POINT p1, POINT p2, double bulge);
std::vector<double> arc_bulge(std::vector<OBJECT>::iterator it);
POINT arc_center1(POINT a, POINT b, POINT c);
void translate_contour(std::vector<CONTOUR>::iterator it, double x, double y, double contour_center_x, double contour_center_y, double rotate_deg);
void swap_contour_i1(int i);
void swap_object_i1(int i);




void swap_contour_i1(int i){
    std::reverse(contours.at(i).sequence.begin(), contours.at(i).sequence.end());

    //swap objects if the contour is swapped
    for(int j=0; j<contours.at(i).sequence.size(); j++){
        swap_object_i1(j);
    }
}

void swap_object_i1(int i){
    double start_x = cad.at(i).start.x;
    double start_y = cad.at(i).start.y;
    double start_z = cad.at(i).start.z;
    cad.at(i).start.x=cad.at(i).end.x;
    cad.at(i).start.y=cad.at(i).end.y;
    cad.at(i).start.z=cad.at(i).end.z;
    cad.at(i).end.x=start_x;
    cad.at(i).end.y=start_y;
    cad.at(i).end.z=start_z;
    std::reverse(cad.at(i).control.begin(), cad.at(i).control.end());
}



OBJECT raw_offset(std::vector<OBJECT>::iterator it, double offset){
    //double offset=0.001;

    if(it->type=="line"){
        OBJECT obj=line_offset(it,offset);
        obj.red=255; obj.green=0; obj.blue=255; obj.alpha=255;
        return obj;
    }
    if(it->type=="arc"){
        if(std::round(it->red)==255 && std::round(it->green)==255){ //yellow, inside offset
            OBJECT obj=arc_offset(*it,offset);
            obj.red=255; obj.green=0; obj.blue=255; obj.alpha=255;
            return obj;
        }
        if(std::round(it->red)==255 && std::round(it->green)==0){ //red, outside offset
            OBJECT obj=arc_offset(*it,abs(offset));
            obj.red=255; obj.green=0; obj.blue=255; obj.alpha=255;
            return obj;
        }
    }
    if(it->type=="circle"){
        bool checked=true;
        double temp_offset=offset;
        if(temp_offset<0){
            if(abs(temp_offset)>it->radius){ //dont add circles with negative radius
                checked=false;
            }
        }
        if(checked){cam.clear();
            if(std::round(it->red)==255 && std::round(it->green)==255){ //yellow, inside offset
                OBJECT obj;
                obj=circle_offset(it,offset);
                obj.red=255; obj.green=0; obj.blue=255; obj.alpha=255;
                return obj;
            }
        }
        if(std::round(it->red)==255 && std::round(it->green)==0){ //red, outside offset
            OBJECT obj;
            obj=circle_offset(it,abs(offset));
            obj.red=255; obj.green=0; obj.blue=255; obj.alpha=255;
            return obj;
        }
    }
    if(it->type=="linestrip" || it->type=="rectangular" || it->type=="polygon"){
        OBJECT obj;
        obj=linestrip_offset(it,offset);
        obj.red=255; obj.green=0; obj.blue=255; obj.alpha=255;
        return obj;
    }
    if(it->type=="spline"){
        OBJECT obj;
        obj=spline_offset_as_linestrip(it,resolution,offset);
        obj.red=255; obj.green=0; obj.blue=255; obj.alpha=255;
        return obj;
    }
    if(it->type=="ellipse"){
        OBJECT obj;
        obj=ellipse_offset_as_linestrip(it,resolution,offset);
        obj.red=255; obj.green=0; obj.blue=255; obj.alpha=255;
        return obj;
    }
    if(it->type=="bezier"){
        OBJECT obj;
        obj=bezier_offset_as_linestrip(it,resolution,offset);
        obj.red=255; obj.green=0; obj.blue=255; obj.alpha=255;
        return obj;
    }
}

std::string get_current_dir_() { //use std:: libs instead of qt libs
    char buff[FILENAME_MAX]; //create string buffer to hold path
    getcwd( buff, FILENAME_MAX );
    std::string current_working_dir(buff);
    return current_working_dir;
}

void nest(){

    //write file nest_in.txt
    int id=0;
    std::vector<CONTOUR>::iterator it;
    std::vector<int>::iterator it1;
    std::vector<OBJECT>::iterator it2,it3;
    std::vector<POINT>::iterator it4;
    std::ofstream myfile;
    myfile.open(get_current_dir_()+"/"+"nest_in.txt" /*, std::ios::app*/); //ios app = append true
    myfile.clear();

    myfile << "binstart_x "<<bin_start_x<<std::endl;
    myfile << "binstart_y "<<bin_start_y<<std::endl;
    myfile << "binwidth "<<bin_widht<<std::endl;
    myfile << "binheight "<<bin_height<<std::endl;
    myfile << "rotation "<<rotation_mode<<std::endl;
    myfile << "use_hole "<<use_holes<<std::endl;
    myfile << "population_size "<<population_size<<std::endl;
    myfile << "mutation_rate "<<mutation_rate<<std::endl;
    myfile << "spacing "<<part_spacing<<std::endl;

    //copy first offset
    for(it=contours.begin(); it<contours.end(); it++){

        myfile << "bid "<<id<<std::endl; id++;
        for(it1=it->sequence.begin(); it1<it->sequence.end(); it1++){
            int object = *it1;
            it2=cad.begin()+object;
            if(it1==it->sequence.begin()){
                it3=it2;
            }

            if(it2->type=="circle"){
                for(it4=it2->control.begin(); it4<it2->control.end(); it4++){
                    myfile << it4->x << " " << it4->y << std::endl;
                }
            } else {

                myfile << int(it2->start.x) << " " << int(it2->start.y) << std::endl;
                for(it4=it2->control.begin(); it4<it2->control.end(); it4++){

                    if(it2->type!="arc"){
                        myfile << it4->x << " " << it4->y << std::endl;
                    }

                    //if arc use more controlpoints then one.
                    if(it2->type=="arc"){

                        std::vector<double> xcon,ycon; //holds the controlpoints

                        int segments = 15;
                        double x_center = it2->center.x;
                        double y_center = it2->center.y;
                        double dx = x_center - it2->start.x;
                        double dy = y_center - it2->start.y;
                        double radius = it2->radius;

                        //formula if arc is cw or ccw..
                        double x1 = it2->start.x;
                        double y1 = it2->start.y;
                        double x2 = it2->end.x;
                        double y2 = it2->end.y;
                        double xp = it2->control.front().x;
                        double yp =  it2->control.front().y;
                        double determinant = ((x2-x1)*(yp-y1))-((y2-y1)*(xp-x1));
                        double deg_angle_start=0,deg_angle_end=0;

                        double ye=it2->end.y;
                        double xe=it2->end.x;
                        double ys=it2->start.y;
                        double xs=it2->start.x;

                        if(determinant > 0){
                            deg_angle_start = 180/M_PI*atan2(ye-y_center, xe-x_center);    //start angle G2
                            deg_angle_end = 180/M_PI*atan2(ys-y_center, xs-x_center);    //end angle G2
                        }
                        if(determinant < 0){
                            deg_angle_start = 180/M_PI*atan2(ys-y_center, xs-x_center);    //start angle G3
                            deg_angle_end  = 180/M_PI*atan2(ye-y_center, xe-x_center);    //end angle G3
                        }
                        if(determinant == 0){
                            //draw straight line
                        }


                        //draw arc..
                        double pi_angle_start = (deg_angle_start/360)*(2*M_PI); //start angle
                        //if(end_angle==360)
                        //{
                        //    end_angle=0;   //autocad degree problem..
                        //}
                        double pi_angle_end = (deg_angle_end/360)*(2*M_PI);
                        if (pi_angle_end<pi_angle_start)   //this to avoid the start angle is bigger then the end angle value
                        {
                            pi_angle_end+=2*M_PI;
                        }

                        double pi_arc_angle = pi_angle_end - pi_angle_start;
                        double theta = pi_arc_angle / double(segments - 1);//theta is now calculated from the arc angle instead, the - 1 bit comes from the fact that the arc is open
                        double tangetial_factor = tan(theta);
                        double radial_factor = cos(theta);
                        double x = radius * cos(pi_angle_start);//we now start at the start angle
                        double y = radius * sin(pi_angle_start);


                        for(int ii = 0; ii < segments; ii++)
                        {
                            //glVertex2d(x + x_center, y + y_center); //x center, y center..
                            xcon.push_back(x+x_center); //ccw
                            ycon.push_back(y+y_center);

                            double tx = -y;
                            double ty = x;

                            x += tx * tangetial_factor;
                            y += ty * tangetial_factor;

                            x *= radial_factor;
                            y *= radial_factor;
                        }

                        // write controlpoints
                        if(it2->d<0){ //g3
                            for(unsigned int i=0; i<xcon.size(); i++){
                                myfile << xcon.at(i) << " " << ycon.at(i) << std::endl;
                            }
                        }

                        if(it2->d>0){ //g2
                            std::reverse(xcon.begin(), xcon.end());
                            std::reverse(ycon.begin(), ycon.end());
                            for(unsigned int i=0; i<xcon.size(); i++){
                                myfile << xcon.at(i) << " " << ycon.at(i) << std::endl;
                            }
                        }
                    }
                }
            }

        }
        myfile << it3->start.x << " " << it3->start.y << std::endl;
        myfile << "end" <<std::endl;

    }
    myfile.close();

    //delete nest_out.txt
    remove("nest_out.txt");
    remove("ready.txt");

    //start up the java nest app
    system("java -jar grotius_nest_interface.jar"); //append "&" is without freezing

}

void java_nest(){

    std::vector<CONTOUR>::iterator it;
    std::vector<int>::iterator it1;
    std::vector<OBJECT>::iterator it2,it3;
    std::vector<POINT>::iterator it4;

    //read file nest_out.txt
    int platenumber_line=0;
    int bid_line=0;
    int end_line=0;
    int trans_x_line=0;
    int trans_y_line=0;
    int rot_line=0;

    int platenumber=0,bid=0;
    double translation_x=0, translation_y=0, rotation=0;

    std::vector<double> data;
    std::vector<std::vector<double>> datas;

    unsigned int linenumber=0;
    char count[200];
    FILE *file=fopen("nest_out.txt", "r"); //convert std::string to std::char

    while(file && fgets(count, sizeof(count), file)){
        linenumber++;

        if(strstr(count,"bid")){
            bid_line=linenumber+1;
        }
        if(linenumber==bid_line){
            bid=std::stoi(count);
            data.push_back(bid);
            //std::cout<<"bid: "<<bid<<std::endl;
        }

        if(strstr(count,"platenumber")){
            platenumber_line=linenumber+1;
        }
        if(linenumber==platenumber_line){
            platenumber=std::stoi(count);
            data.push_back(platenumber);
            //std::cout<<"platenumber: "<<platenumber<<std::endl;
        }

        if(strstr(count,"translation x")){
            trans_x_line=linenumber+1;
        }
        if(linenumber==trans_x_line){
            translation_x=std::stod(count);
            data.push_back(translation_x);
            //std::cout<<"translation_x: "<<translation_x<<std::endl;
        }

        if(strstr(count,"translation y")){
            trans_y_line=linenumber+1;
        }
        if(linenumber==trans_y_line){
            translation_y=std::stod(count);
            data.push_back(translation_y);
            //std::cout<<"translation_y: "<<translation_y<<std::endl;
        }

        if(strstr(count,"rotation")){
            rot_line=linenumber+1;
        }
        if(linenumber==rot_line){
            rotation=std::stod(count);
            data.push_back(rotation);
            //std::cout<<"rotation: "<<rotation<<std::endl;
            datas.push_back(data);
            data.clear();
        }
    }
    fcloseall();

    //at this stage we have a container with data : bid, trans_x, trans_y, rotation.
    //sort bid.
    std::sort(datas.begin(),datas.end()); //checked ok
    //print output :
    //for(unsigned int i=0; i<datas.size(); i++){
    //std::cout<<datas[i][0]<<std::endl;
    //}
    //std::cout<<"datas.size: "<<datas.size()<<std::endl;


    for(unsigned int i=0; i<datas.size(); i++){
        if(bins<datas[i][1]+1){bins=datas[i][1]+1;}
    }

    //rotate
    int contournr=0;
    double fangle=0;
    for(it=contours.begin(); it<contours.end(); it++){

        for(unsigned int i=0; i<datas.size(); i++){
            if(datas[i][0]==contournr){

                for(it1=it->sequence.begin(); it1<it->sequence.end(); it1++){
                    int object = *it1;
                    it2=cad.begin()+object;

                    //std::cout<<"object.type: "<<it2->type<<std::endl;

                    //start pos
                    fangle=datas[i][4]*(M_PI/180); //std::cout<<"fangle: "<<fangle<<std::endl; //ok
                    double xnew=it2->start.x*cos(fangle)-it2->start.y*sin(fangle); //std::cout<<"xnew: "<<xnew<<std::endl;
                    double ynew=it2->start.x*sin(fangle)+it2->start.y*cos(fangle); //std::cout<<"ynew: "<<ynew<<std::endl;
                    xnew+=datas[i][2]; //std::cout<<"xnew + transx: "<<xnew<<std::endl;
                    ynew+=datas[i][3]; //std::cout<<"ynew + transy: "<<ynew<<std::endl;

                    //double bsx = bin_start_x+((bin_widht+bin_space_x)*i);

                    it2->start.x=xnew+bin_start_x+((bin_widht+bin_space_x)*datas[i][1]); //platenumber
                    it2->start.y=ynew+bin_start_y;

                    //end pos
                    xnew=it2->end.x*cos(fangle)-it2->end.y*sin(fangle); //std::cout<<"xnew: "<<xnew<<std::endl;
                    ynew=it2->end.x*sin(fangle)+it2->end.y*cos(fangle); //std::cout<<"ynew: "<<ynew<<std::endl;
                    xnew+=datas[i][2]; //std::cout<<"xnew + transx: "<<xnew<<std::endl;
                    ynew+=datas[i][3]; //std::cout<<"ynew + transy: "<<ynew<<std::endl;
                    it2->end.x=xnew+bin_start_x+((bin_widht+bin_space_x)*datas[i][1]);
                    it2->end.y=ynew+bin_start_y;

                    //center pos
                    xnew=it2->center.x*cos(fangle)-it2->center.y*sin(fangle); //std::cout<<"xnew: "<<xnew<<std::endl;
                    ynew=it2->center.x*sin(fangle)+it2->center.y*cos(fangle); //std::cout<<"ynew: "<<ynew<<std::endl;
                    xnew+=datas[i][2]; //std::cout<<"xnew + transx: "<<xnew<<std::endl;
                    ynew+=datas[i][3]; //std::cout<<"ynew + transy: "<<ynew<<std::endl;
                    it2->center.x=xnew+bin_start_x+((bin_widht+bin_space_x)*datas[i][1]);
                    it2->center.y=ynew+bin_start_y;

                    for(it4=it2->control.begin(); it4<it2->control.end(); it4++){

                        double fangle=datas[i][4]*M_PI/180; //std::cout<<"fangle: "<<fangle<<std::endl;
                        double xnew=it4->x*cos(fangle)-it4->y*sin(fangle); //std::cout<<"xnew: "<<xnew<<std::endl;
                        double ynew=it4->x*sin(fangle)+it4->y*cos(fangle); //std::cout<<"ynew: "<<ynew<<std::endl;
                        xnew+=datas[i][2]; //std::cout<<"xnew + transx: "<<xnew<<std::endl;
                        ynew+=datas[i][3]; //std::cout<<"ynew + transy: "<<ynew<<std::endl;
                        it4->x=xnew+bin_start_x+((bin_widht+bin_space_x)*datas[i][1]);
                        it4->y=ynew+bin_start_y;

                    }
                }
            }
        }
        contournr++;
    }
}

void translate_contour(std::vector<CONTOUR>::iterator it,  double x, double y, double contour_center_x, double contour_center_y, double rotate_deg){
    std::vector<OBJECT>::iterator it2,it3;
    std::vector<POINT>::iterator it4;
    std::vector<int>::iterator it1;

    for(it1=it->sequence.begin(); it1<it->sequence.end(); it1++){
        int object = *it1;
        it2=cad.begin()+object;

        //translate startpoint
        it2->start=translate_3d(it2->start.x,it2->start.y,it2->start.z,x,y,0);

        //rotate startpoint
        double dist_x=it2->start.x-contour_center_x; //normalize
        double dist_y=it2->start.y-contour_center_y;

        POINT rot=rotate_3d(dist_x,dist_y,0,0,0,rotate_deg);

        it2->start.x=rot.x+contour_center_x; //bring back
        it2->start.y=rot.y+contour_center_y;


        //controlpoints
        for(it4=it2->control.begin(); it4<it2->control.end(); it4++){

            //translate controlpoint
            POINT p=translate_3d(it4->x,it4->y,0,x,y,0);
            it4->x=p.x;
            it4->y=p.y;

            //rotate controlpoint
            double dist_x=it4->x-contour_center_x; //normalize
            double dist_y=it4->y-contour_center_y;

            POINT rot=rotate_3d(dist_x,dist_y,0,0,0,rotate_deg);

            it4->x=rot.x+contour_center_x; //bring back
            it4->y=rot.y+contour_center_y;
        }

        //translate
        it2->end=translate_3d(it2->end.x,it2->end.y,it2->end.z,x,y,0);

        //rotate endpoint
        dist_x=it2->end.x-contour_center_x; //normalize
        dist_y=it2->end.y-contour_center_y;

        rot=rotate_3d(dist_x,dist_y,0,0,0,rotate_deg);

        it2->end.x=rot.x+contour_center_x; //bring back
        it2->end.y=rot.y+contour_center_y;
    }
}

void cam_drill_offset(double offset, std::string layer, int job){

    std::vector<CONTOUR>::iterator it;
    std::vector<int>::iterator it1;
    std::vector<OBJECT>::iterator it2;

    //copy first offset
    for(it=contours.begin(); it<contours.end(); it++){

        for(it1=it->sequence.begin(); it1<it->sequence.end(); it1++){
            int object = *it1;
            it2=cad.begin()+object;

            if(it2->layer==layer){ //we only want to cut the selected combobox layer
                if(it2->type=="circle"){

                    //avoid drilling a hole bigger then the hole itself.
                    if(it2->radius>=offset){
                        OBJECT obj;
                        obj=*it2;
                        obj.red=0; obj.green=0; obj.blue=255; obj.alpha=255;

                        obj.end.x=obj.center.x+offset;
                        obj.end.y=obj.center.y;

                        obj.start.x=0;
                        obj.start.y=0;

                        obj.layer=layer;
                        obj.job=job;
                        cam.push_back(obj);
                    }
                }
            }
        }
    }
}

void no_offset(std::string layer, int job){

    std::cout<<"ok until no_offset"<<std::endl;

    //normal pocket without islands. use offset -1 or 1.
    std::vector<CONTOUR>::iterator it;
    std::vector<int>::iterator it1;
    std::vector<OBJECT>::iterator it2;
    OBJECT obj,a,b,c;

    //copy first offset
    for(it=contours.begin(); it<contours.end(); it++){

        for(it1=it->sequence.begin(); it1<it->sequence.end(); it1++){
            int object = *it1;
            it2=cad.begin()+object;

            if(it2->layer==layer){ //we only want to cut the selected combobox layer

                if(it2->type=="line" || it2->type=="arc" || it2->type=="circle"){
                    std::cout<<"ok until here"<<std::endl;
                    obj=*it2;
                    obj.job=job;
                    obj.layer=layer;
                    obj.end_contour=0;
                    obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                    cam.push_back(obj);
                }
                if(it2->type=="linestrip"  || it2->type=="rectangular" || it2->type=="polygon"){
                    obj=*it2; //first copy all parameters
                    //obj=raw_offset(it2); //transform into linestrip
                    obj.type="line";
                    obj.job=job; //add job nr.
                    obj.layer=layer;
                    obj.end_contour=0;
                    obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;

                    //1. front of linestrip
                    a=obj;
                    a.end=a.control.front();
                    a.control.clear();
                    cam.push_back(a);

                    //2. controlpoints.
                    b=obj;
                    for(int i=0; i<obj.control.size()-1; i++){
                        b.start=obj.control.at(i);
                        b.end=obj.control.at(i+1);
                        b.control.clear();
                        cam.push_back(b);
                    }
                    c=obj;
                    c.start=c.control.back();
                    c.control.clear();
                    cam.push_back(c);
                }
                if(it2->type=="spline" || it2->type=="bezier" || it2->type=="ellipse" ){
                    obj=raw_offset(it2,0.0001); //transform into linestrip
                    obj.type="line";
                    obj.job=job; //add job nr.
                    obj.layer=layer;
                    obj.end_contour=0;
                    obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;

                    //1. front of linestrip
                    a=obj;
                    a.end=a.control.front();
                    a.control.clear();
                    cam.push_back(a);

                    //2. controlpoints.
                    b=obj;
                    for(int i=0; i<obj.control.size()-1; i++){
                        b.start=obj.control.at(i);
                        b.end=obj.control.at(i+1);
                        b.control.clear();
                        cam.push_back(b);
                    }
                    c=obj;
                    c.start=c.control.back();
                    c.control.clear();
                    cam.push_back(c);
                }
                if(it1==it->sequence.end()-1){ //add end of contour flag
                    cam.back().end_contour=1;
                }
            }
        }
    }
}

void cam_normal_offset(double offset, std::string layer, int job){

    //normal pocket without islands. use offset -1 or 1.
    std::vector<CONTOUR>::iterator it;
    std::vector<int>::iterator it1;
    std::vector<OBJECT>::iterator it2;
    std::vector<POINT>::iterator it3,it4;


    //copy first offset
    for(it=contours.begin(); it<contours.end(); it++){

        cavc::Polyline<double> outerloop;
        for(it1=it->sequence.begin(); it1<it->sequence.end(); it1++){
            int object = *it1;
            it2=cad.begin()+object;

            if(it2->layer==layer){ //we only want to cut the selected combobox layer
                if(it2->type=="spline"){
                    std::vector<POINT> points=spline_to_points(it2,0.01);
                    for(it3=points.begin(); it3<points.end(); it3++){ //we don't need the spline endpoint
                        outerloop.addVertex(it3->x, it3->y, 0);
                    }
                }
                if(it2->type=="ellipse"){
                    std::vector<POINT> points=ellipse_to_points(it2,0.01);
                    std::reverse(points.begin(),points.end()); //make cw contour
                    for(it3=points.begin(); it3<points.end(); it3++){ //we don't need the spline endpoint
                        outerloop.addVertex(it3->x, it3->y, 0);
                    }
                }
                if(it2->type=="bezier"){
                    std::vector<POINT> points=bezier_to_points(it2,0.01);
                    std::reverse(points.begin(),points.end()); //make cw contour
                    for(it3=points.begin(); it3<points.end(); it3++){ //we don't need the spline endpoint
                        outerloop.addVertex(it3->x, it3->y, 0);
                    }
                }
                if(it2->type=="rectangular"||it2->type=="linestrip"||it2->type=="polygon"){
                    outerloop.addVertex(it2->start.x, it2->start.y, 0); //startpoint
                    //std::cout<<"linestrip startpoint inserted"<<std::endl;
                    for(it4=it2->control.begin(); it4<it2->control.end(); it4++){ //controlpoints
                        //std::cout<<"linestrip controlpoint inserted"<<std::endl;
                        outerloop.addVertex(it4->x, it4->y, 0);
                    }
                }
                if(it2->type=="arc"){
                    std::vector<double> bulge=arc_bulge(it2);
                    if(bulge[1]==0){ //single arc
                        outerloop.addVertex(it2->start.x, it2->start.y, bulge[0]); //startpoint arc + bulge
                    }
                    if(bulge[1]==1){ //dual arc
                        outerloop.addVertex(it2->start.x, it2->start.y, bulge[0]); //startpoint arc + bulge
                        outerloop.addVertex(it2->control.front().x, it2->control.front().y, bulge[0]); //startpoint arc + bulge
                    }
                }
                if(it2->type=="circle"){
                    if(it2->red==255 && it2->green==255 && it2->blue==0 && it2->alpha==255){ //yellow
                        outerloop.addVertex(it2->center.x+it2->radius, it2->center.y, 1); //startpoint arc + full bulge=semicircle, -1=g2, 1=g3
                        outerloop.addVertex(it2->center.x-it2->radius, it2->center.y, 1); //startpoint arc + bulge
                    } else {
                        outerloop.addVertex(it2->center.x+it2->radius, it2->center.y, -1);
                        outerloop.addVertex(it2->center.x-it2->radius, it2->center.y, -1);
                    }
                }
                if(it2->type=="line"){
                    outerloop.addVertex(it2->start.x, it2->start.y, 0);
                }
            }
        }
        outerloop.isClosed() = true;

        //process data
        std::vector<cavc::Polyline<double>> results = cavc::parallelOffset(outerloop, offset);
        OBJECT obj;

        for(unsigned int i=0; i<results.size(); i++){ //cw loops
            for(unsigned int j=0; j<results.at(i).vertexes().size()-1; j++){

                if(results.at(i).vertexes().at(j).bulge()==0){ //line
                    obj.type="line";
                    obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                    obj.start.x=results.at(i).vertexes().at(j).x();
                    obj.start.y=results.at(i).vertexes().at(j).y();
                    obj.end.x=results.at(i).vertexes().at(j+1).x();
                    obj.end.y=results.at(i).vertexes().at(j+1).y();
                    obj.d=results.at(i).vertexes().at(j).bulge(); //bulge 0=line, bulge neg=g2, bulge pos=g3
                    obj.layer=layer;
                    obj.job=job;
                    obj.end_contour=0;
                    cam.push_back(obj);
                }
                if(results.at(i).vertexes().at(j).bulge()!=0){ //arc
                    obj.type="arc";
                    obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                    obj.start.x=results.at(i).vertexes().at(j).x();
                    obj.start.y=results.at(i).vertexes().at(j).y();
                    obj.end.x=results.at(i).vertexes().at(j+1).x();
                    obj.end.y=results.at(i).vertexes().at(j+1).y();
                    POINT p=bulge_to_arc_controlpoint({obj.start.x,obj.start.y,0},{obj.end.x,obj.end.y,0},results.at(i).vertexes().at(j).bulge());
                    obj.control.clear();
                    obj.control.push_back({p.x,p.y,0});
                    obj.center=arc_center1(obj.start,obj.control.front(),obj.end);
                    obj.d=results.at(i).vertexes().at(j).bulge();
                    obj.layer=layer;
                    obj.job=job;
                    obj.end_contour=0;
                    cam.push_back(obj);
                }
            }

            //add last primitive of contour
            if(results.at(i).vertexes().back().bulge()==0){ //line
                obj.type="line";
                obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                obj.end.x=results.at(i).vertexes().front().x();
                obj.end.y=results.at(i).vertexes().front().y();
                obj.start.x=results.at(i).vertexes().back().x();
                obj.start.y=results.at(i).vertexes().back().y();
                obj.d=results.at(i).vertexes().back().bulge();
                obj.layer=layer;
                obj.job=job;
                obj.end_contour=1;
                cam.push_back(obj);

            }
            if(results.at(i).vertexes().back().bulge()!=0){ //arc
                obj.type="arc";
                obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                obj.end.x=results.at(i).vertexes().front().x();
                obj.end.y=results.at(i).vertexes().front().y();
                obj.start.x=results.at(i).vertexes().back().x();
                obj.start.y=results.at(i).vertexes().back().y();
                POINT p=bulge_to_arc_controlpoint({obj.start.x,obj.start.y,0},{obj.end.x,obj.end.y,0},results.at(i).vertexes().back().bulge());
                obj.control.clear();
                obj.control.push_back({p.x,p.y,0});
                obj.center=arc_center1(obj.start,obj.control.front(),obj.end);
                obj.d=results.at(i).vertexes().back().bulge();
                obj.layer=layer;
                obj.job=job;
                obj.end_contour=1;
                cam.push_back(obj);
            }
        }
    }
}

void cam_pocket_offset_with_islands(double offset, std::string layer, double overlap, int job){ //pockets with islands. must draw a outside box (depth 0), a inner box (depth 1) with a inner box (depth 2).

    std::vector<CONTOUR>::iterator it,itx;
    std::vector<int>::iterator it1;
    std::vector<OBJECT>::iterator it2;
    std::vector<POINT>::iterator it3,it4;
    std::vector<OBJECT> a;
    int trigger=0;

    //convert cad primitives to cam line and linestrips.
    //cavc::Polyline<double> outerloop;
    //cavc::Polyline<double> island; //must be clockwise, red is clockwise already!
    //std::vector<cavc::Polyline<double>> islands; //save multiple inner islands

    //copy first offset
    for(it=contours.begin(); it<contours.end(); it++){


        it1=it->sequence.begin();

        it2=cad.begin()+ *it1;
        if(it2->layer==layer){
            trigger=1;
        } else {trigger=0;}

        if(trigger==1){

            cavc::Polyline<double> outerloop;
            std::vector<cavc::Polyline<double>> islands;
            std::vector<int> insides;

            insides=it->inside;
            for(int i=0; i<insides.size(); i++){
                std::cout<<"insides: "<<insides.at(i)<<std::endl;
            }

            if(it->depth==1){ //outer contour
                for(it1=it->sequence.begin(); it1<it->sequence.end(); it1++){
                    int object = *it1;
                    it2=cad.begin()+object;

                    if(it2->type=="spline"){
                        std::vector<POINT> points=spline_to_points(it2,0.01);
                        for(it3=points.begin(); it3<points.end(); it3++){ //we don't need the spline endpoint
                            outerloop.addVertex(it3->x, it3->y, 0);
                        }
                    }
                    if(it2->type=="ellipse"){
                        std::vector<POINT> points=ellipse_to_points(it2,0.01);
                        std::reverse(points.begin(),points.end()); //make cw contour
                        for(it3=points.begin(); it3<points.end(); it3++){ //we don't need the spline endpoint
                            outerloop.addVertex(it3->x, it3->y, 0);
                        }
                    }
                    if(it2->type=="bezier"){
                        std::vector<POINT> points=bezier_to_points(it2,0.01);
                        std::reverse(points.begin(),points.end()); //make cw contour
                        for(it3=points.begin(); it3<points.end(); it3++){ //we don't need the spline endpoint
                            outerloop.addVertex(it3->x, it3->y, 0);
                        }
                    }
                    if(it2->type=="rectangular"||it2->type=="linestrip"||it2->type=="polygon"){
                        outerloop.addVertex(it2->start.x, it2->start.y, 0); //startpoint
                        //std::cout<<"linestrip startpoint inserted"<<std::endl;
                        for(it4=it2->control.begin(); it4<it2->control.end(); it4++){ //controlpoints
                            //std::cout<<"linestrip controlpoint inserted"<<std::endl;
                            outerloop.addVertex(it4->x, it4->y, 0);
                        }
                    }
                    if(it2->type=="arc"){
                        std::vector<double> bulge=arc_bulge(it2);
                        if(bulge[1]==0){ //single arc
                            outerloop.addVertex(it2->start.x, it2->start.y, bulge[0]); //startpoint arc + bulge
                        }
                        if(bulge[1]==1){ //dual arc
                            outerloop.addVertex(it2->start.x, it2->start.y, bulge[0]); //startpoint arc + bulge
                            outerloop.addVertex(it2->control.front().x, it2->control.front().y, bulge[0]); //startpoint arc + bulge
                        }
                    }
                    if(it2->type=="circle"){
                        outerloop.addVertex(it2->center.x+it2->radius, it2->center.y, 1); //startpoint arc + full bulge=semicircle, -1=g2, 1=g3
                        outerloop.addVertex(it2->center.x-it2->radius, it2->center.y, 1); //startpoint arc + bulge
                    }
                    if(it2->type=="line"){
                        outerloop.addVertex(it2->start.x, it2->start.y, 0);
                    }
                }
                outerloop.isClosed() = true;
                //std::reverse(outerloop.vertexes().begin(),outerloop.vertexes().end()); //swap contour direction


                //std::vector<cavc::Polyline<double>> islands;
                int l=0;
                for(itx=contours.begin(); itx<contours.end(); itx++){

                    for(unsigned int i=0; i<insides.size(); i++){
                        if(insides.at(i)==l){

                            it1=itx->sequence.begin();
                            int object = *it1;
                            it2=cad.begin()+object;
                            if(it2->layer==layer){

                                cavc::Polyline<double> island; //must be clockwise, red is clockwise already!
                                for(it1=itx->sequence.begin(); it1<itx->sequence.end(); it1++){
                                    int object = *it1;
                                    it2=cad.begin()+object;


                                    if(it2->type=="spline"){
                                        std::vector<POINT> points=spline_to_points(it2,0.01);
                                        for(it3=points.begin(); it3<points.end(); it3++){ //we don't need the spline endpoint
                                            island.addVertex(it3->x, it3->y, 0);
                                        }
                                    }
                                    if(it2->type=="ellipse"){
                                        std::vector<POINT> points=ellipse_to_points(it2,0.01);
                                        std::reverse(points.begin(),points.end()); //make cw contour
                                        for(it3=points.begin(); it3<points.end(); it3++){ //we don't need the spline endpoint
                                            island.addVertex(it3->x, it3->y, 0);
                                        }
                                    }
                                    if(it2->type=="bezier"){
                                        std::vector<POINT> points=bezier_to_points(it2,0.01);
                                        std::reverse(points.begin(),points.end()); //make cw contour
                                        for(it3=points.begin(); it3<points.end(); it3++){ //we don't need the spline endpoint
                                            island.addVertex(it3->x, it3->y, 0);
                                        }
                                    }
                                    if(it2->type=="rectangular"||it2->type=="linestrip"||it2->type=="polygon"){
                                        island.addVertex(it2->start.x, it2->start.y, 0); //startpoint
                                        //std::cout<<"linestrip startpoint inserted"<<std::endl;
                                        for(it4=it2->control.begin(); it4<it2->control.end(); it4++){ //controlpoints
                                            //std::cout<<"linestrip controlpoint inserted"<<std::endl;
                                            island.addVertex(it4->x, it4->y, 0);
                                        }
                                    }
                                    if(it2->type=="arc"){
                                        std::vector<double> bulge=arc_bulge(it2);
                                        if(bulge[1]==0){ //single arc
                                            island.addVertex(it2->start.x, it2->start.y, bulge[0]); //startpoint arc + bulge
                                        }
                                        if(bulge[1]==1){ //dual arc
                                            island.addVertex(it2->start.x, it2->start.y, bulge[0]); //startpoint arc + bulge
                                            island.addVertex(it2->control.front().x, it2->control.front().y, bulge[0]); //startpoint arc + bulge
                                        }
                                    }
                                    if(it2->type=="circle"){
                                        island.addVertex(it2->center.x+it2->radius, it2->center.y, -1); //startpoint arc + full bulge=semicircle, -1=g2, 1=g3
                                        island.addVertex(it2->center.x-it2->radius, it2->center.y, -1); //startpoint arc + bulge
                                    }
                                    if(it2->type=="line"){
                                        island.addVertex(it2->start.x, it2->start.y, 0);
                                    }
                                }
                                island.isClosed() = true;
                                //std::reverse(island.vertexes().begin(),island.vertexes().end()); //swap contour direction
                                islands.push_back(island);
                            }
                        }
                    }
                    l++;
                }

                //out of loop here. calculate results:

                cavc::OffsetLoopSet<double> loopSet;
                loopSet.ccwLoops.push_back({0, outerloop, cavc::createApproxSpatialIndex(outerloop)});

                for(int i=0; i<islands.size(); i++){
                    loopSet.cwLoops.push_back({0, islands.at(i), cavc::createApproxSpatialIndex(islands.at(i))});
                }

                //loopSet.cwLoops.push_back({0, island, cavc::createApproxSpatialIndex(island)});
                // add the surrounding counter clockwise loop
                // constructed with {parent index, closed polyline, spatial index}
                // this structure is also returned and can be fed back into the offset algorithm (the spatial
                // indexes are created by the algorithm and used for the next iteration)
                // NOTE: parent index can always be 0 (it is just used internally, API likely to be improved in
                // the future...), spatial index must always be created with the associated polyline

                // add the clockwise loop islands
                //loopSet.cwLoops.push_back({0, islands, cavc::createApproxSpatialIndex(islands)});

                // NOTE: this algorithm requires all polylines to be closed and non-self intersecting, and not
                // intersect any of the other input polylines
                cavc::ParallelOffsetIslands<double> alg;
                // NOTE: offset delta is always taken as an absolute magnitude
                // (negative values are the same as positive, to change offset direction you can change the
                // orientation of the loops e.g. from clockwise to counter clockwise)
                //const double offsetDelta = 1.0;
                // compute first offset (offset by 1.0)

                double pocket_offset=offset;
                while(trigger==1){

                    cavc::OffsetLoopSet<double> results = alg.compute(loopSet, pocket_offset);
                    // compute offset of first offset result (offset by another 1.0)
                    //cavc::OffsetLoopSet<double> offsetResult2 = alg.compute(offsetResult, offsetDelta);

                    if(results.cwLoops.size()==0 && results.ccwLoops.size()==0){trigger=0; break;}

                    //std::vector<cavc::Polyline<double>> results = cavc::parallelOffset(input, offset);
                    OBJECT obj;
                    for(unsigned int i=0; i<results.cwLoops.size(); i++){ //cw loops
                        for(unsigned int j=0; j<results.cwLoops.at(i).polyline.size()-1; j++){

                            if(results.cwLoops.at(i).polyline.vertexes().at(j).bulge()==0){ //line
                                obj.type="line";
                                obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                                obj.start.x=results.cwLoops.at(i).polyline.vertexes().at(j).x();
                                obj.start.y=results.cwLoops.at(i).polyline.vertexes().at(j).y();
                                obj.end.x=results.cwLoops.at(i).polyline.vertexes().at(j+1).x();
                                obj.end.y=results.cwLoops.at(i).polyline.vertexes().at(j+1).y();
                                obj.layer=layer;
                                obj.job=job;
                                obj.end_contour=0;
                                cam.push_back(obj);
                            }
                            if(results.cwLoops.at(i).polyline.vertexes().at(j).bulge()!=0){ //arc
                                obj.type="arc";
                                obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                                obj.start.x=results.cwLoops.at(i).polyline.vertexes().at(j).x();
                                obj.start.y=results.cwLoops.at(i).polyline.vertexes().at(j).y();
                                obj.end.x=results.cwLoops.at(i).polyline.vertexes().at(j+1).x();
                                obj.end.y=results.cwLoops.at(i).polyline.vertexes().at(j+1).y();
                                POINT p=bulge_to_arc_controlpoint({obj.start.x,obj.start.y,0},{obj.end.x,obj.end.y,0},results.cwLoops.at(i).polyline.vertexes().at(j).bulge());
                                obj.control.clear();
                                obj.control.push_back({p.x,p.y,0});
                                obj.center=arc_center1(obj.start,obj.control.front(),obj.end);
                                obj.d=results.cwLoops.at(i).polyline.vertexes().at(j).bulge();
                                obj.layer=layer;
                                obj.job=job;
                                obj.end_contour=0;
                                cam.push_back(obj);
                            }
                        }

                        //add last primitive of contour
                        if(results.cwLoops.at(i).polyline.vertexes().back().bulge()==0){ //line
                            obj.type="line";
                            obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                            obj.end.x=results.cwLoops.at(i).polyline.vertexes().front().x();
                            obj.end.y=results.cwLoops.at(i).polyline.vertexes().front().y();
                            obj.start.x=results.cwLoops.at(i).polyline.vertexes().back().x();
                            obj.start.y=results.cwLoops.at(i).polyline.vertexes().back().y();
                            obj.layer=layer;
                            obj.job=job;
                            obj.end_contour=1;
                            cam.push_back(obj);

                        }
                        if(results.cwLoops.at(i).polyline.vertexes().back().bulge()!=0){ //arc
                            obj.type="arc";
                            obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                            obj.end.x=results.cwLoops.at(i).polyline.vertexes().front().x();
                            obj.end.y=results.cwLoops.at(i).polyline.vertexes().front().y();
                            obj.start.x=results.cwLoops.at(i).polyline.vertexes().back().x();
                            obj.start.y=results.cwLoops.at(i).polyline.vertexes().back().y();
                            POINT p=bulge_to_arc_controlpoint({obj.start.x,obj.start.y,0},{obj.end.x,obj.end.y,0},results.cwLoops.at(i).polyline.vertexes().back().bulge());
                            obj.control.clear();
                            obj.control.push_back({p.x,p.y,0});
                            obj.center=arc_center1(obj.start,obj.control.front(),obj.end);
                            obj.d=results.cwLoops.at(i).polyline.vertexes().back().bulge();
                            obj.layer=layer;
                            obj.job=job;
                            obj.end_contour=1;
                            cam.push_back(obj);
                        }
                    }

                    for(unsigned int i=0; i<results.ccwLoops.size(); i++){ //ccw loops
                        for(unsigned int j=0; j<results.ccwLoops.at(i).polyline.size()-1; j++){

                            if(results.ccwLoops.at(i).polyline.vertexes().at(j).bulge()==0){ //line
                                obj.type="line";
                                obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                                obj.start.x=results.ccwLoops.at(i).polyline.vertexes().at(j).x();
                                obj.start.y=results.ccwLoops.at(i).polyline.vertexes().at(j).y();
                                obj.end.x=results.ccwLoops.at(i).polyline.vertexes().at(j+1).x();
                                obj.end.y=results.ccwLoops.at(i).polyline.vertexes().at(j+1).y();
                                obj.layer=layer;
                                obj.job=job;
                                obj.end_contour=0;
                                cam.push_back(obj);
                            }
                            if(results.ccwLoops.at(i).polyline.vertexes().at(j).bulge()!=0){ //arc
                                obj.type="arc";
                                obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                                obj.start.x=results.ccwLoops.at(i).polyline.vertexes().at(j).x();
                                obj.start.y=results.ccwLoops.at(i).polyline.vertexes().at(j).y();
                                obj.end.x=results.ccwLoops.at(i).polyline.vertexes().at(j+1).x();
                                obj.end.y=results.ccwLoops.at(i).polyline.vertexes().at(j+1).y();
                                POINT p=bulge_to_arc_controlpoint({obj.start.x,obj.start.y,0},{obj.end.x,obj.end.y,0},results.ccwLoops.at(i).polyline.vertexes().at(j).bulge());
                                obj.control.clear();
                                obj.control.push_back({p.x,p.y,0});
                                obj.center=arc_center1(obj.start,obj.control.front(),obj.end);
                                obj.d=results.ccwLoops.at(i).polyline.vertexes().at(j).bulge();
                                obj.layer=layer;
                                obj.job=job;
                                obj.end_contour=0;
                                cam.push_back(obj);
                            }
                        }

                        //add last primitive of contour
                        if(results.ccwLoops.at(i).polyline.vertexes().back().bulge()==0){ //line
                            obj.type="line";
                            obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                            obj.end.x=results.ccwLoops.at(i).polyline.vertexes().front().x();
                            obj.end.y=results.ccwLoops.at(i).polyline.vertexes().front().y();
                            obj.start.x=results.ccwLoops.at(i).polyline.vertexes().back().x();
                            obj.start.y=results.ccwLoops.at(i).polyline.vertexes().back().y();
                            obj.layer=layer;
                            obj.job=job;
                            obj.end_contour=1;
                            cam.push_back(obj);

                        }
                        if(results.ccwLoops.at(i).polyline.vertexes().back().bulge()!=0){ //arc
                            obj.type="arc";
                            obj.red=0; obj.green=255; obj.blue=0; obj.alpha=255;
                            obj.end.x=results.ccwLoops.at(i).polyline.vertexes().front().x();
                            obj.end.y=results.ccwLoops.at(i).polyline.vertexes().front().y();
                            obj.start.x=results.ccwLoops.at(i).polyline.vertexes().back().x();
                            obj.start.y=results.ccwLoops.at(i).polyline.vertexes().back().y();
                            POINT p=bulge_to_arc_controlpoint({obj.start.x,obj.start.y,0},{obj.end.x,obj.end.y,0},results.ccwLoops.at(i).polyline.vertexes().back().bulge());
                            obj.control.clear();
                            obj.control.push_back({p.x,p.y,0});
                            obj.center=arc_center1(obj.start,obj.control.front(),obj.end);
                            obj.d=results.ccwLoops.at(i).polyline.vertexes().back().bulge();
                            obj.layer=layer;
                            obj.job=job;
                            obj.end_contour=1;
                            cam.push_back(obj);
                        }
                    }
                    pocket_offset+=offset*overlap;
                }
            }
        }
    }
}

POINT bulge_to_arc_controlpoint(POINT p1, POINT p2, double bulge){ //find the arc center

    //bulge neg=g2
    //bulge pos=g3
    //bulge 0=line
    //bulge 1=semicircle
    //m=(p1+p2)/2
    //Bulge = d1/d2=tan(Delta/4)
    //http://www.lee-mac.com/bulgeconversion.html#bulgearc
    //https://math.stackexchange.com/questions/1337344/get-third-point-from-an-arc-constructed-by-start-point-end-point-and-bulge

    double dist=sqrt(pow(p1.x-p2.x,2)+pow(p1.y-p2.y,2)); //dist=chord lenght p1-p2
    POINT m={(p1.x+p2.x)/2,(p1.y+p2.y)/2,0}; //m=point half chord lenght p1-p2
    double d1=(abs(bulge))*(0.5*dist); //d1=height from m to arc controlpoint

    //normalize p1, rotate around m, bring m back to x,y 0,0
    double p1x_nor=p1.x-m.x;
    double p1y_nor=p1.y-m.y;

    POINT p1_rot;
    if(bulge<0){ //g2
        p1_rot=rotate_3d(p1x_nor,p1y_nor,0,0,0,-90);
    }
    if(bulge>0){ //g3
        p1_rot=rotate_3d(p1x_nor,p1y_nor,0,0,0,90);
    }


    p1_rot.x+=m.x; //bring the rotated p1 back to orginal coordinates
    p1_rot.y+=m.y;

    POINT q=offset_point_on_line(m.x,m.y,p1_rot.x,p1_rot.y,d1);

    //std::cout<<"arc controlpoint x: "<<q.x<<" y: "<<q.y<<std::endl;
    return q; //arc controlpoint

}

std::vector<double> arc_bulge(std::vector<OBJECT>::iterator it){

    //https://github.com/jbuckmccready/CavalierContours/issues/17

    //bulge neg=g2
    //bulge pos=g3
    double pi_angle_arc=0;
    double pi_angle_start=0;
    double pi_angle_end=0;
    std::vector<double> bulge; //bulge[0]=bulge value, bulge[1]=0 (one arc), bulge[1]=1 (two arcs)
    double r=it->radius;
    POINT a=it->start;
    POINT b=it->end;
    POINT c=it->center;

    if(it->d>0){ //g2, d=determinant
        pi_angle_start = atan2(b.y-c.y, b.x-c.x);
        pi_angle_end = atan2(a.y-c.y, a.x-c.x);
    }
    if(it->d<0){ //g3
        pi_angle_start = atan2(a.y-c.y, a.x-c.x);
        pi_angle_end  = atan2(b.y-c.y, b.x-c.x);
    }
    if(it->d==0){
        bulge.push_back(0); //draw straight line
        bulge.push_back(0);
    }

    if(pi_angle_end<pi_angle_start){pi_angle_end+=2*M_PI;}
    pi_angle_arc=pi_angle_end-pi_angle_start;

    if(pi_angle_arc>M_PI){ //split up in 2 arcs
        double half_pi_angle_arc=pi_angle_arc/2;
        double bulges=tan(half_pi_angle_arc/4);
        if(it->d<0){
            bulge.push_back(abs(bulges));
            bulge.push_back(1);
        }
        if(it->d>0){
            bulge.push_back(-abs(bulges));
            bulge.push_back(1);
        }

    } else {
        if(it->d<0){
            bulge.push_back(abs(tan(pi_angle_arc/4))); //keep it as 1 arc
            bulge.push_back(0);
        }
        if(it->d>0){
            bulge.push_back(-abs(tan(pi_angle_arc/4))); //keep it as 1 arc
            bulge.push_back(0);
        }
    }

    return bulge;
}

std::vector<OBJECT> find_intersections(std::vector<OBJECT> array){
    std::vector<OBJECT>::iterator it,it1;
    for(it=array.begin(); it<array.end(); it++){

        for(it1=array.begin(); it1<array.end(); it1++){
            if(it!=it1){

                if(it->type=="line"&&it1->type=="line"){
                    line_line_intersect(it,it1);
                }
                if(it->type=="linestrip"&&it1->type=="linestrip"){
                    linestrip_linestrip_intersect(it,it1);
                }
                if(it->type=="arc"&&it1->type=="arc"){
                    arc_arc_intersect(it,it1);
                }
                if(it->type=="line"&&it1->type=="arc"){
                    line_arc_intersect(it,it1);
                }
                if(it->type=="linestrip"&&it1->type=="arc"){  //is oke
                    linestrip_arc_intersect(it,it1);
                }
                if(it->type=="linestrip"&&it1->type=="line"){
                    linestrip_line_intersect(it,it1);
                }
                if(it->type=="linestrip"){
                    linestrip_self_intersect(it);
                }
            }
        }
    }
    return array;
}

OBJECT construct_arc(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1, double d, double offset){
    OBJECT obj;
    obj.type="arc";
    obj.red=255; obj.green=0; obj.blue=255; obj.alpha=255;
    obj.start.x=it->end.x;
    obj.start.y=it->end.y;
    obj.end.x=it1->start.x;
    obj.end.y=it1->start.y;
    obj.radius=abs(offset);
    obj.control.resize(1);
    obj.d=d;

    double radsq=obj.radius*obj.radius;
    double q=sqrt(((obj.end.x-obj.start.x)*(obj.end.x-obj.start.x))+((obj.end.y-obj.start.y)*(obj.end.y-obj.start.y)));
    double cx=(obj.start.x+obj.end.x)/2; //center x
    double cy=(obj.start.y+obj.end.y)/2; //center y
    double pi_angle_start=0,pi_angle_end=0;

    if(d>0){ //G2
        obj.center.x=cx-sqrt(radsq-((q/2)*(q/2)))*((obj.start.y-obj.end.y)/q);
        obj.center.y=cy-sqrt(radsq-((q/2)*(q/2)))*((obj.end.x-obj.start.x)/q);

        pi_angle_end=atan2(obj.start.y-obj.center.y, obj.start.x-obj.center.x);
        pi_angle_start=atan2(obj.end.y-obj.center.y, obj.end.x-obj.center.x);
        if(pi_angle_start>pi_angle_end){pi_angle_end+=2*M_PI;}
        obj.control.front().x=obj.center.x+(cos((pi_angle_start+pi_angle_end)/2)*obj.radius);
        obj.control.front().y=obj.center.y+(sin((pi_angle_start+pi_angle_end)/2)*obj.radius);
    }
    if(d<0){ //G3
        obj.center.x=cx+sqrt(radsq-((q/2)*(q/2)))*((obj.start.y-obj.end.y)/q);
        obj.center.y=cy+sqrt(radsq-((q/2)*(q/2)))*((obj.end.x-obj.start.x)/q);

        pi_angle_end=atan2(obj.end.y-obj.center.y, obj.end.x-obj.center.x);
        pi_angle_start=atan2(obj.start.y-obj.center.y, obj.start.x-obj.center.x);
        if(pi_angle_start>pi_angle_end){pi_angle_end+=2*M_PI;}
        obj.control.front().x=obj.center.x+(cos((pi_angle_start+pi_angle_end)/2)*obj.radius);
        obj.control.front().y=obj.center.y+(sin((pi_angle_start+pi_angle_end)/2)*obj.radius);
    }
    return obj;
}

POINT arc_center1(POINT a, POINT b, POINT c){
    POINT center;
    double x1 = (b.x + a.x) / 2;
    double y1 = (b.y + a.y) / 2;
    double dy1 = b.x - a.x;
    double dx1 = -(b.y - a.y);

    //Get the perpendicular bisector of (x2, y2) and (x3, y3).
    double x2 = (c.x + b.x) / 2;
    double y2 = (c.y + b.y) / 2;
    double dy2 = c.x - b.x;
    double dx2 = -(c.y - b.y);

    double endpoint_x0 = x1 + dx1;
    double endpoint_y0 = y1 + dy1;
    double endpoint_x1 = x2 + dx2;
    double endpoint_y1 = y2 + dy2;

    //line 1
    double delta_y0 = endpoint_y0 - y1;
    double delta_x0 = x1 - endpoint_x0;
    double c0 = delta_y0 * x1 + delta_x0 * y1;
    //line 2
    double delta_y1 = endpoint_y1 - y2;
    double delta_x1 = x2 - endpoint_x1;
    double c1 = delta_y1 * x2 + delta_x1 * y2;

    double determinant = delta_y0*delta_x1 - delta_y1*delta_x0;
    double deg_start_angle=0;
    double deg_end_angle=0;
    double radius=0;
    double x_center=0;
    double y_center=0;

    if (determinant==0){
        // lines are parallel
    } else {
        center.x = (delta_x1*c0 - delta_x0*c1)/determinant;
        center.y = (delta_y0*c1 - delta_y1*c0)/determinant;
    }
    return center;
}
#endif





