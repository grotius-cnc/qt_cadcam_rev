#ifndef FILE_CAM_CONTOURS
#define FILE_CAM_CONTOURS
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
    REVISION  : 5.6.2020     Cleaned up code, improved algoritme, structurized functions.
              : 6.6.2020     Checked code
    TODO      : 1. If contour contains 3 or more matching start-, cq endpoints, this can be calculated/defined as open contour, expand the algoritme for like "star shaped contours".
                2. Drawing repair tool/function to repair tiny open spaces in contours
                3. Starting at depth function, replace code with std::iterators it, it1 instead of integers i and j.
    USAGE     : To use this header only library/file, include also structs.h to your project.

                Example of usage sequence, see calculate();

    PROTOCOL  : 1. Find open contours, before closed contours.
                2. Minimize the use of QT libraries to stay compatible with other program's.
    NOTES     : --
*/

#include <iostream>
#include <algorithm>
#include <structs.h>
#include <math.h>
#include <chrono> //high_resolution_clock

CONTOUR contour;
std::vector<CONTOUR> contours;

double tol=0.01;
int contournumber=0;
int object_startsize=0;

void find_contours();

std::vector<OBJECT>::iterator find_non_selected();
std::vector<OBJECT>::iterator find_next(std::vector<OBJECT>::iterator it);
std::vector<OBJECT>::iterator find_one_side_open_object();
std::vector<POINT> high_resolution_spline_points(std::vector<std::vector<double>> C);

void swap_object_it(std::vector<OBJECT>::iterator it);
void print_start_info();
void erase_dots();
void erase_object_duplicates();
void clean_objects();
void single_closed_objects();
void single_open_objects();
void dual_closed_objects();
void multi_open_objects();
void multi_closed_objects();
void print_contour_end_info();

void swap_contour_it(std::vector<CONTOUR>::iterator it);
void swap_contour_i(int i);
void swap_object_i(int i);

void point_in_polygon();
int pip(std::vector<std::vector<double>> pos, int x, int y);
std::vector<int> full_contourlist();
std::vector<int> insidelist(std::vector<int> contourlist);
std::vector<int> depthlist(std::vector<int>contourlist, std::vector<int>insidelist);
std::vector<int> contourlist_min_depthlist(std::vector<int> contourlist, std::vector<int> depthlist);
void depth();
void write_depth_to_contours(std::vector<int> depthlist, int depth);

void swap_contour_i(int i){
    std::reverse(contours.at(i).sequence.begin(), contours.at(i).sequence.end());

    //swap objects if the contour is swapped
    for(int j=0; j<contours.at(i).sequence.size(); j++){
        swap_object_i(j);
    }
}

void swap_object_i(int i){
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

void find_contours(){

    auto start = std::chrono::high_resolution_clock::now();
    //print_start_info();
    erase_dots();
    //erase_object_duplicates();
    clean_objects();
    single_closed_objects();
    single_open_objects();
    dual_closed_objects();
    multi_open_objects();
    multi_closed_objects();
    //print_contour_end_info();
    //area();
    point_in_polygon();
    depth();

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout<<""<<std::endl;
    std::cout<<"time: "<<elapsed.count()<<" sec"<<std::endl;
    std::cout<<"contour reqognize algoritme done!"<<std::endl;
}

std::vector<OBJECT>::iterator find_non_selected(){
    std::vector<OBJECT>::iterator it;
    for(it=cad.begin(); it<cad.end(); it++){
        if(it->select==0){
            return it;
        }
    }
    //std::cout<<"no non selected found, returned -1"<<std::endl;
    return cad.begin()-1;
}

std::vector<OBJECT>::iterator find_next(std::vector<OBJECT>::iterator it){
    std::vector<OBJECT>::iterator it1;
    //std::cout<<"source:"<<it-objects.begin()<<std::endl;

    for(it1=cad.begin(); it1<cad.end(); it1++){
        if(it!=it1 && it->end.x<it1->end.x+tol && it->end.x>it1->end.x-tol && it->end.y<it1->end.y+tol && it->end.y>it1->end.y-tol){
            swap_object_it(it1);
            //std::cout<<"next swapped:"<<it1-objects.begin()<<std::endl;
            return it1;
        } else
            if(it!=it1 && it->end.x<it1->start.x+tol && it->end.x>it1->start.x-tol && it->end.y<it1->start.y+tol && it->end.y>it1->start.y-tol){
                //std::cout<<"next:"<<it1-objects.begin()<<std::endl;
                return it1;
            }
    }
    //std::cout<<"no next found, returned -1"<<std::endl;
    return cad.begin()-1; //-1 value for no end or startmatch found !
}

std::vector<OBJECT>::iterator find_one_side_open_object(){
    std::vector<OBJECT>::iterator it,it1;
    int xs=0; int xe=0;
    for(it=cad.begin(); it<cad.end(); it++){
        xs=0; xe=0;
        if(it->select==false){
            for(it1=cad.begin(); it1<cad.end(); it1++){
                if(it1->select==false){
                    if(it-cad.begin()!=it1-cad.begin()){
                        if(it->start.x<it1->end.x+tol && it->start.x>it1->end.x-tol && it->start.y<it1->end.y+tol && it->start.y>it1->end.y-tol){
                            xs++;
                        } else
                            if(it->start.x<it1->start.x+tol && it->start.x>it1->start.x-tol && it->start.y<it1->start.y+tol && it->start.y>it1->start.y-tol){
                                xs++;
                            } else
                                if(it->end.x<it1->start.x+tol && it->end.x>it1->start.x-tol && it->end.y<it1->start.y+tol && it->end.y>it1->start.y-tol){
                                    xe++;
                                } else
                                    if(it->end.x<it1->end.x+tol && it->end.x>it1->end.x-tol && it->end.y<it1->end.y+tol && it->end.y>it1->end.y-tol){
                                        xe++;
                                    }
                    }
                }
            }
            if(xs==0 && xe==1){ //no start match, only end match, object direction ok.
                //std::cout<<"one side open object found, no swap:"<<it-objects.begin()<<std::endl;
                return it;
            }
            if(xs==1 && xe==0){ //no end match, only start match, swap direction, then object direction ok.
                swap_object_it(it);
                //std::cout<<"one side open object found, swap done:"<<it-objects.begin()<<std::endl;
                return it;
            }
        }
    }
    //std::cout<<"no one side open object found, returned -1"<<std::endl;
    return cad.begin()-1; //no one side open objects found!
}

void swap_object_it(std::vector<OBJECT>::iterator it){
    double start_x = it->start.x;
    double start_y = it->start.y;
    double start_z = it->start.z;
    it->start.x=it->end.x;
    it->start.y=it->end.y;
    it->start.z=it->end.z;
    it->end.x=start_x;
    it->end.y=start_y;
    it->end.z=start_z;
    std::reverse(it->control.begin(), it->control.end());
}

void print_start_info(){
    std::vector<OBJECT>::iterator it;
    std::cout<<""<<std::endl;
    std::cout<<"object types:"<<std::endl;

    int count=0;
    for(it=cad.begin(); it<cad.end(); it++){
        if(it->type=="line"){
            count++;}
    }
    std::cout<<"line:"<<count<<std::endl;

    count=0;
    for(it=cad.begin(); it<cad.end(); it++){
        if(it->type=="linestrip"){count++;}
    }
    std::cout<<"linestrip:"<<count<<std::endl;

    count=0;
    for(it=cad.begin(); it<cad.end(); it++){
        if(it->type=="circle"){count++;}
    }
    std::cout<<"cirlce:"<<count<<std::endl;

    count=0;
    for(it=cad.begin(); it<cad.end(); it++){
        if(it->type=="arc"){count++;}
    }
    std::cout<<"arc:"<<count<<std::endl;

    count=0;
    for(it=cad.begin(); it<cad.end(); it++){
        if(it->type=="spline"){
            count++;}
    }
    std::cout<<"spline:"<<count<<std::endl;
    std::cout<<"objects size:"<<cad.size()<<std::endl;

}

void print_contour_end_info(){
    std::cout<<"colormap"<<std::endl;
    std::cout<<"red        : closed contour,  1 objects"<<std::endl;
    std::cout<<"yellow     : open contour,    1 objects"<<std::endl;
    std::cout<<"purple     : closed contour,  2 objects"<<std::endl;
    std::cout<<"blue       : closed contour, >2 objects"<<std::endl;
    std::cout<<"grey       : open contour,   >2 objects"<<std::endl;

    std::cout<<"total contours :"<<contournumber<<std::endl;
}

void erase_dots(){
    std::vector<OBJECT>::iterator it;
    bool stop=0;
    int count=0;
    while(!stop){
        //erase dots with brute force!!
        stop=1;
        for(it=cad.begin(); it<cad.end(); it++){
            if(it->type=="line"){
                it->lenght=sqrt(pow(it->end.x-it->start.x,2)+pow(it->end.y-it->start.y,2));
                if(it->lenght<0.01){
                    cad.erase(it);
                    count++;
                    stop=0;
                }
            }
        }
    }
    cad.shrink_to_fit();
    //std::cout<<"erased dots:"<<count<<std::endl;
}

void clean_objects(){
    std::vector<OBJECT>::iterator it;
    for(it=cad.begin(); it<cad.end(); it++){
        it->select=0;
        it->red=255;
        it->green=255;
        it->blue=255;
        it->alpha=255;
    }
    contournumber=0;
    contour.sequence.clear();
    contours.clear();
    //std::cout<<"objects cleaned up for new contour calculation"<<std::endl;
    //std::cout<<"objects size:"<<objects.size()<<std::endl;
}

void single_closed_objects(){
    std::vector<OBJECT>::iterator it;
    for(it=cad.begin(); it<cad.end(); it++){
        if(it->select==false){
            if(it->start.x<it->end.x+tol && it->start.x>it->end.x-tol && it->start.y<it->end.y+tol && it->start.y>it->end.y-tol){
                it->select=true;
                it->red=255;
                it->green=0;
                it->blue=0;
                it->alpha=255;
                contournumber++;
                //std::cout<<"closed contour, 1 objects, contournumber: "<<contournumber<<std::endl;
                contour.contourtype=2; //0=open, 1=closed, 2=single object closed
                contour.sequence.push_back(it-cad.begin());
                contours.push_back(contour);
                contour.sequence.clear();
            }
        }
    }
}

void single_open_objects(){
    std::vector<OBJECT>::iterator it,it1;
    int x=0;
    for(it=cad.begin(); it<cad.end(); it++){
        x=0;
        if(it->select==false){
            for(it1=cad.begin(); it1<cad.end(); it1++){
                if(it1->select==false){
                    if(it-cad.begin()!=it1-cad.begin()){
                        if(it->start.x<it1->end.x+tol && it->start.x>it1->end.x-tol && it->start.y<it1->end.y+tol && it->start.y>it1->end.y-tol){
                            x++;
                        } else
                            if(it->start.x<it1->start.x+tol && it->start.x>it1->start.x-tol && it->start.y<it1->start.y+tol && it->start.y>it1->start.y-tol){
                                x++;
                            } else
                                if(it->end.x<it1->start.x+tol && it->end.x>it1->start.x-tol && it->end.y<it1->start.y+tol && it->end.y>it1->start.y-tol){
                                    x++;
                                } else
                                    if(it->end.x<it1->end.x+tol && it->end.x>it1->end.x-tol && it->end.y<it1->end.y+tol && it->end.y>it1->end.y-tol){
                                        x++;
                                    }
                    }
                }
            }
            if(x==0){
                it->select=true;
                it->red=255;
                it->green=255;
                it->blue=0;
                it->alpha=255;
                contournumber++;
                //std::cout<<"open contour, 1 objects, contournumber: "<<contournumber<<std::endl;
                contour.contourtype=0; //0=open, 1=closed, 2=single object closed
                contour.sequence.push_back(it-cad.begin());
                contours.push_back(contour);
                contour.sequence.clear();
            }
        }
    }

}

void dual_closed_objects(){
    std::vector<OBJECT>::iterator it,it1;
    for(it=cad.begin(); it<cad.end(); it++){
        if(it->select==false){
            for(it1=cad.begin(); it1<cad.end(); it1++){
                if(it1->select==false){
                    if(it!=it1 && it->start.x<it1->start.x+tol && it->start.x>it1->start.x-tol && it->start.y<it1->start.y+tol && it->start.y>it1->start.y-tol &&
                            it->end.x<it1->end.x+tol && it->end.x>it1->end.x-tol && it->end.y<it1->end.y+tol && it->end.y>it1->end.y-tol){
                        swap_object_it(it1);
                    }
                    if(it!=it1 && it->end.x<it1->start.x+tol && it->end.x>it1->start.x-tol && it->end.y<it1->start.y+tol && it->end.y>it1->start.y-tol &&
                            it->start.x<it1->end.x+tol && it->start.x>it1->end.x-tol && it->start.y<it1->end.y+tol && it->start.y>it1->end.y-tol){
                        it->select=true;
                        it1->select=true;
                        it->red=255;
                        it->green=0;
                        it->blue=255;
                        it->alpha=255;
                        it1->red=255;
                        it1->green=0;
                        it1->blue=255;
                        it1->alpha=255;
                        contournumber++;
                        //std::cout<<"closed contour, 2 objects, contournumber: "<<contournumber<<std::endl;
                        contour.contourtype=1; //0=open, 1=closed, 2=single object closed
                        contour.sequence.push_back(it-cad.begin());
                        contour.sequence.push_back(it1-cad.begin());
                        contours.push_back(contour);
                        contour.sequence.clear();
                    }
                }
            }
        }
    }
}

void multi_closed_objects(){
    std::vector<OBJECT>::iterator it,it1,it_source,it_next;
    it=find_non_selected();
    bool stop=0;
    while(!stop){
        if(it!=cad.begin()-1){ //object found
            it->select=1; it->red=0; it->green=0; it->blue=255; it->alpha=255;
            contour.contourtype=1; //0=open, 1=closed, 2=single object closed
            contour.sequence.push_back(it-cad.begin());
            it_source=it;
            it_next=cad.begin()-1;

            while(it_next!=it_source){
                it_next=find_next(it);
                if(it_next->select==false && it_next!=cad.begin()-1){
                    it_next->select=1; it_next->red=0; it_next->green=0; it_next->blue=255; it_next->alpha=255;
                    contour.sequence.push_back(it_next-cad.begin());
                    it=it_next;
                } else {
                    break;
                }
            }
            contournumber++;
            //std::cout<<"closed contour >2 objects, contournumber: "<<contournumber<<std::endl;
            contours.push_back(contour);
            contour.sequence.clear();
        }

        it1=find_non_selected();
        if(it1!=cad.begin()-1){
            it=it1;
            stop=0;
        } else {
            stop=1;
        }
    }
}

void multi_open_objects(){
    std::vector<OBJECT>::iterator it,it1,it_source,it_next;
    it=find_one_side_open_object();
    bool stop=0;
    while(!stop){
        if(it!=cad.begin()-1){ //object found
            it->select=1; it->red=106; it->green=106; it->blue=106; it->alpha=255;
            contour.contourtype=0; //0=open, 1=closed, 2=single object closed
            contour.sequence.push_back(it-cad.begin());
            it_source=it;
            it_next=cad.begin()-1;

            while(it_next!=it_source){
                it_next=find_next(it);
                if(it_next->select==false && it_next!=cad.begin()-1){
                    it_next->select=1; it_next->red=106; it_next->green=106; it_next->blue=106; it_next->alpha=255;
                    contour.sequence.push_back(it_next-cad.begin());
                    it=it_next;
                } else {
                    break;
                }
            }
            contournumber++;
            //std::cout<<"open contour >2 objects, contournumber: "<<contournumber<<std::endl;
            contours.push_back(contour);
            contour.sequence.clear();
        }

        it1=find_one_side_open_object();
        if(it1!=cad.begin()-1){
            it=it1;
            stop=0;
        } else {stop=1;}
    }
}

void erase_object_duplicates(){

    //std::cout<<"find duplicates.."<<std::endl;
    std::vector<OBJECT>::iterator it,it1;

    //add object lenghts to objects
    for(it=cad.begin(); it<cad.end(); it++){
        //std::cout<<"object type:"<<it->type<<std::endl;
        if(it->type=="line"){
            it->lenght=sqrt(pow(it->end.x-it->start.x,2)+pow(it->end.y-it->start.y,2));
            //std::cout<<"object lenght:"<<it->lenght<<std::endl;
        }
    }

    for(it=cad.begin(); it<cad.end(); it++){
        for(it1=cad.begin(); it1<cad.end(); it1++){
            if(it!=it1){

                //search for line duplicates
                if(it->type=="line" && it1->type=="line"){
                    int count=0;

                    //endpoint check
                    double l1=sqrt(pow(it1->end.x - it->end.x,2) + pow(it1->end.y - it->end.y,2));
                    double l2=sqrt(pow(it1->end.x - it->start.x,2) + pow(it1->end.y - it->start.y,2));
                    if(it->lenght<l1+l2+tol && it->lenght>l1+l2-tol){
                        //std::cout<<"endpoint object:"<<it1-objects.begin()<<" is on the line of object:"<<it-objects.begin()<<std::endl;
                        count++;

                    }

                    //startpoint check
                    l1=sqrt(pow(it1->start.x - it->end.x,2) + pow(it1->start.y - it->end.y,2));
                    l2=sqrt(pow(it1->start.x - it->start.x,2) + pow(it1->start.y - it->start.y,2));
                    if(it->lenght<l1+l2+tol && it->lenght>l1+l2-tol){
                        //std::cout<<"endpoint object:"<<it1-objects.begin()<<" is on the line of object:"<<it-objects.begin()<<std::endl;
                        count++;
                    }

                    //erase duplicate object
                    if(count==2){
                        it1=cad.erase(it1);
                        //std::cout<<"duplicate lines erased at:"<<it1-objects.begin()<<std::endl;
                    }
                }

                //search for circle duplicates
                if(it->type=="circle" && it1->type=="circle"){
                    if(it->radius < it1->radius+tol && it->radius > it1->radius-tol &&
                            it->center.x < it1->center.x+tol && it->center.x > it1->center.x-tol &&
                            it->center.y < it1->center.y+tol && it->center.y > it1->center.y-tol ){
                        it1=cad.erase(it1);
                        //std::cout<<"duplicate circle erased at:"<<it1-objects.begin()<<std::endl;
                    }
                }

                //search for arc duplicates
                if(it->type=="arc" && it1->type=="arc"){
                    if(it->radius < it1->radius+tol && it->radius > it1->radius-tol &&
                            it->center.x < it1->center.x+tol && it->center.x > it1->center.x-tol &&
                            it->center.y < it1->center.y+tol && it->center.y > it1->center.y-tol &&
                            it1->pi_angle_start >= it->pi_angle_start && it1->pi_angle_end <= it->pi_angle_end ){
                        it1=cad.erase(it1);
                        //std::cout<<"duplicate arc erased at:"<<it1-objects.begin()<<std::endl;
                    }
                }

                //search for line on linestrip duplicates
                if(it->type=="linestrip" && it1->type=="line"){

                    //--- SECTION --- linestrip start to first controlpoint, duplicate endpoint check
                    int count=0;
                    double l1=sqrt(pow(it1->end.x - it->control.at(0).x,2) + pow(it1->end.y - it->control.at(0).y,2));
                    double l2=sqrt(pow(it1->end.x - it->start.x,2) + pow(it1->end.y - it->start.y,2));
                    if(it->lenght<l1+l2+tol && it->lenght>l1+l2-tol){
                        count++;
                    }

                    //linestrip start to first controlpoint, duplicate startpoint check
                    l1=sqrt(pow(it1->start.x - it->control.at(0).x,2) + pow(it1->start.y - it->control.at(0).y,2));
                    l2=sqrt(pow(it1->start.x - it->start.x,2) + pow(it1->start.y - it->start.y,2));
                    if(it->lenght<l1+l2+tol && it->lenght>l1+l2-tol){
                        count++;
                    }

                    //erase duplicate object
                    if(count==2){
                        it1=cad.erase(it1);
                        //std::cout<<"duplicate line on linestrip start->controlpoint erased at:"<<it1-objects.begin()<<std::endl;
                    }

                    //--- SECTION --- linestrip between controlpoints
                    count=0; //reset
                    for(int i=0; i<it->control.size()-1; i++){
                        //line startpoint check
                        l1=sqrt(pow(it1->start.x - it->control.at(i).x,2) + pow(it1->start.y - it->control.at(i).y,2));
                        l2=sqrt(pow(it1->start.x - it->control.at(i+1).x,2) + pow(it1->start.y - it->control.at(i+1).y,2));
                        if(it->lenght<l1+l2+tol && it->lenght>l1+l2-tol){
                            count++;
                        }
                        //line endpoint check
                        l1=sqrt(pow(it1->end.x - it->control.at(i).x,2) + pow(it1->end.y - it->control.at(i).y,2));
                        l2=sqrt(pow(it1->end.x - it->control.at(i+1).x,2) + pow(it1->end.y - it->control.at(i+1).y,2));
                        if(it->lenght<l1+l2+tol && it->lenght>l1+l2-tol){
                            count++;
                        }
                        //erase duplicate object
                        if(count==2){
                            it1=cad.erase(it1);
                            //std::cout<<"duplicate line on linestrip controlpoint->controlpoint erased at:"<<it1-objects.begin()<<std::endl;
                        }

                    }

                    //--- SECTION --- linestrip last controlpoint to linestrip endpoint, duplicate endpoint check
                    count=0;
                    l1=sqrt(pow(it1->end.x - it->control.back().x,2) + pow(it1->end.y - it->control.back().y,2));
                    l2=sqrt(pow(it1->end.x - it->end.x,2) + pow(it1->end.y - it->end.y,2));
                    if(it->lenght<l1+l2+tol && it->lenght>l1+l2-tol){
                        count++;

                    }

                    //linestrip last controlpoint to linestrip endpoint, duplicate startpoint check
                    l1=sqrt(pow(it1->start.x - it->control.back().x,2) + pow(it1->start.y - it->control.back().y,2));
                    l2=sqrt(pow(it1->start.x - it->end.x,2) + pow(it1->start.y - it->end.y,2));
                    if(it->lenght<l1+l2+tol && it->lenght>l1+l2-tol){
                        count++;
                    }

                    //erase duplicate object
                    if(count==2){
                        it1=cad.erase(it1);
                        //std::cout<<"duplicate line on linestrip controlpoint->endpoint erased at:"<<it1-objects.begin()<<std::endl;
                    }
                }

                //search for line on splines duplicates
                if(it->type=="spline" && it1->type=="line"){

                    std::vector<std::vector<double>> C; //2d array
                    std::vector<POINT> points;
                    std::vector<POINT>::iterator it2,it3;
                    int count=0;

                    C.push_back({it->start.x, it->start.y}); //spline startpoint
                    for(int j=0; j<it->control.size(); j++){
                        C.push_back({it->control.at(j).x, it->control.at(j).y}); //spline controlpoints
                    }
                    C.push_back({it->end.x, it->end.y}); //spline endpoint
                    points=high_resolution_spline_points(C);

                    //imagine a line from 2 spline points, calculate line lenght
                    //for each spline segment
                    for(it2=points.begin(); it2<points.end()-1; it2++){ //first spline point
                        it3=it2+1; //second spline point
                        double lenght=sqrt(pow(it3->x - it2->x,2)+pow(it3->y - it2->y,2)); //spline segment lenght

                        //check if line endpoint is on the spline segment
                        double l1=sqrt(pow(it1->end.x - it2->x,2) + pow(it1->end.y - it2->y,2));
                        double l2=sqrt(pow(it1->end.x - it3->x,2) + pow(it1->end.y - it3->y,2));
                        if(lenght<l1+l2+tol && lenght>l1+l2-tol){
                            count++;
                            //std::cout<<"line endpoint is on the spline"<<std::endl;
                        }

                        //check if line startpoint is on the spline segment
                        l1=sqrt(pow(it1->start.x - it2->x,2) + pow(it1->start.y - it2->y,2));
                        l2=sqrt(pow(it1->start.x - it3->x,2) + pow(it1->start.y - it3->y,2));
                        if(lenght<l1+l2+tol && lenght>l1+l2-tol){
                            count++;
                            //std::cout<<"line startpoint is on the spline"<<std::endl;
                        }

                    }

                    if(count==2){
                        it1=cad.erase(it1);
                        //std::cout<<"duplicate line on spline erased at:"<<it1-objects.begin()<<std::endl;
                    }
                }

            }
        }
    }
}

std::vector<POINT> high_resolution_spline_points(std::vector<std::vector<double>> C){

    POINT point;
    std::vector<POINT> points;

    double S[C.size()-1][2][4];
    double w, b[C.size()], d[C.size()], x[C.size()];
    int i, dim;
    int n = C.size()-1; // number of splines

    for(dim=0; dim<2; dim++)
    {
        // define d[]:
        d[0]=3.0*(C[1][dim]-C[0][dim]);
        for(i=1; i<n; i++)
            d[i]=3.0*(C[i+1][dim]-C[i-1][dim]);
        d[n]=3.0*(C[n][dim]-C[n-1][dim]);
        // forward sweep: (simplified tridiagonal solution: all a[i]=1 and all c[i]=1)
        b[0]=2.0;
        for(i=1; i<C.size(); i++)
        {
            w = 1.0/b[i-1];
            b[i] = 4.0 - w;
            d[i] -= (w*d[i-1]);
        }
        // calculate solution vector x[i] = D[i]:
        // (Wikipedia x[] = Wolfram D[])
        x[n]=d[n]/b[n];
        for(i=n-1; i>=0; i--)
            x[i]=(d[i]-x[i+1])/b[i];
        // calculate spline S[i][dim] a, b, c and d:
        for(i=0; i<n; i++)
        {
            S[i][dim][0]=C[i][dim];
            S[i][dim][1]=x[i];
            S[i][dim][2]=3.0*(C[i+1][dim]-C[i][dim]) - 2.0*x[i] - x[i+1];
            S[i][dim][3]=2.0*(C[i][dim]-C[i+1][dim]) + x[i] + x[i+1];
        }
    }

    //glBegin(GL_LINE_STRIP);
    for(int p=0; p<C.size()-1; p++)  //spline points
    {
        for(double t=0; t<1; t+=0.2)  //time 0-1 for each spline
        {
            point.x = ((S[p][0][3]*t+S[p][0][2])*t+S[p][0][1])*t+S[p][0][0];
            point.y = ((S[p][1][3]*t+S[p][1][2])*t+S[p][1][1])*t+S[p][1][0];
            //std::cout<< "x:" << px << " y:" << py << std::endl;
            // glVertex2d(px,py);
            points.push_back(point);
        }
    }
    return points;
}

void swap_contour_it(std::vector<CONTOUR>::iterator it){
    std::vector<int>::iterator it1;

    std::reverse(it->sequence.begin(), it->sequence.end());

    //swap objects if the contour is swapped
    for(it1=it->sequence.begin(); it1<it->sequence.end(); it1++){
        swap_object_i(*it1);
    }
}

void depth(){

    int depth=0;

    std::vector<int>contour_list;
    std::vector<int>inside_list;
    std::vector<int>depth_list;

    //initialize full contourlist to start with
    contour_list=full_contourlist();
    inside_list=insidelist(full_contourlist());
    depth_list=depthlist(full_contourlist(),insidelist(full_contourlist()));
    write_depth_to_contours(depth_list, depth);

    while(depth_list.size()!=0){
        contour_list=contourlist_min_depthlist(contour_list,depth_list);
        inside_list=insidelist(contour_list);
        depth_list=depthlist(contour_list,inside_list);

        depth++;
        write_depth_to_contours(depth_list, depth);
    }

    //print depth output for each contour
    //    for(int i=0; i<contours.size(); i++){
    //        std::cout<<"contour nr:"<<i<<" depth:"<<contours.at(i).depth<<std::endl;
    //    }

    //test for output sequence simulation
    for(int i=0; i<cad.size(); i++){
        cad.at(i).select=true;
    }
}

void write_depth_to_contours(std::vector<int> depth_list, int depth){

    for(int i=0; i<contours.size(); i++){
        for(int j=0; j<depth_list.size(); j++){
            int id0=i;
            int id1=depth_list.at(j);

            if(id0==id1){
                contours.at(i).depth=depth;

                //colorize the contour sequence
                if(depth %2==0 && contours.at(i).contourtype!=0){ //even depth numbers are outside contour, color red
                    for(int k=0; k<contours.at(i).sequence.size(); k++){
                        cad.at(contours.at(i).sequence.at(k)).red=255;
                        cad.at(contours.at(i).sequence.at(k)).green=0;
                        cad.at(contours.at(i).sequence.at(k)).blue=0;
                        cad.at(contours.at(i).sequence.at(k)).alpha=255;
                    }
                }
                if(depth %2!=0 && contours.at(i).contourtype!=0){ //odd depth numbers are inside offset, color yellow
                    for(int k=0; k<contours.at(i).sequence.size(); k++){
                        cad.at(contours.at(i).sequence.at(k)).red=255;
                        cad.at(contours.at(i).sequence.at(k)).green=255;
                        cad.at(contours.at(i).sequence.at(k)).blue=0;
                        cad.at(contours.at(i).sequence.at(k)).alpha=255;
                    }
                }
                if(contours.at(i).contourtype==0){ //0=open, 1=closed, 2=single object closed
                    for(int k=0; k<contours.at(i).sequence.size(); k++){
                        cad.at(contours.at(i).sequence.at(k)).red=106;
                        cad.at(contours.at(i).sequence.at(k)).green=106;
                        cad.at(contours.at(i).sequence.at(k)).blue=106;
                        cad.at(contours.at(i).sequence.at(k)).alpha=255;
                    }
                }
            }
        }
    }
}

std::vector<int> contourlist_min_depthlist(std::vector<int> contourlist, std::vector<int> depthlist){

    std::vector<int> contourlist_min_depthlist;
    std::vector<int>::iterator it,it1;
    contourlist_min_depthlist=contourlist;

    for(it=contourlist_min_depthlist.begin(); it<contourlist_min_depthlist.end(); it++){
        for(it1=depthlist.begin(); it1<depthlist.end(); it1++){
            if(*it==*it1){
                contourlist_min_depthlist.erase(it);
            }
        }
    }
    //print output
    //    std::cout<<"contourlist min depthlist:"<<std::endl;
    //    for(int i=0; i<contourlist_min_depthlist.size(); i++){
    //        std::cout<<contourlist_min_depthlist.at(i)<<std::endl;
    //    }
    return contourlist_min_depthlist;
}

std::vector<int> full_contourlist(){

    //full contourlist
    std::vector<int>contourlist;
    for(int i=0; i<contours.size(); i++){
        contourlist.push_back(i);
    }
    //print output
    //    std::cout<<"full contourlist:"<<std::endl;
    //    for(int i=0; i<contourlist.size(); i++){
    //        std::cout<<contourlist.at(i)<<std::endl;
    //    }
    return contourlist;
}

std::vector<int> insidelist(std::vector<int> contourlist){

    std::vector<int>insidelist;
    for(int i=0; i<contourlist.size(); i++){
        for(int j=0; j<contours.size(); j++){

            int id1=contourlist.at(i);
            int id2=j;

            if(id1==id2){
                for(int k=0; k<contours.at(j).inside.size(); k++){

                    //no duplicates
                    if(std::find(insidelist.begin(), insidelist.end(),contours.at(j).inside.at(k))!=insidelist.end()){
                        //do nothing
                    } else {
                        insidelist.push_back(contours.at(j).inside.at(k));
                    }

                }
            }
        }
    }
    //print output
    //    std::cout<<"insidelist:"<<std::endl;
    //    for(int i=0; i<insidelist.size(); i++){
    //        std::cout<<insidelist.at(i)<<std::endl;
    //    }
    return insidelist;
}

std::vector<int> depthlist(std::vector<int>contourlist, std::vector<int>insidelist){

    //depthlist=contourlist-insidelist
    std::vector<int> depthlist;
    std::vector<int> duplicatelist;
    std::vector<int>::iterator it, it1;

    //copy contourlist to depthlist;
    depthlist=contourlist;

    //find all duplicates between contourlist and insidelist
    for(it=contourlist.begin(); it<contourlist.end(); it++){
        for(it1=insidelist.begin(); it1<insidelist.end(); it1++){
            if(*it==*it1){
                duplicatelist.push_back(*it1);
                //std::cout<<"duplicate push back :"<<*it<<std::endl;
            }
        }
    }
    duplicatelist.push_back(-1); //contours with no insides are marked as -1

    //remove duplicates from depthlist
    for(it=depthlist.begin(); it<depthlist.end(); it++){
        for(it1=duplicatelist.begin(); it1<duplicatelist.end(); it1++){
            if(*it==*it1){
                depthlist.erase(it);
            }
        }
    }

    //print output
    //    std::cout<<"depthlist"<<std::endl;
    //    for(int i=0; i<depthlist.size(); i++){
    //        std::cout<<depthlist.at(i)<<std::endl;
    //    }
    return depthlist;
}

void point_in_polygon(){

    std::vector<std::vector<double>> contourpoints;
    std::vector<int>inside_list;
    //fill the position vector with one contour sequence
    for(int i=0; i<contours.size(); i++){
        for(int j=0; j<contours.at(i).sequence.size(); j++){

            if(contours.at(i).contourtype!=0){ //added as test, prevent an open contour involving the depth sequence
                //add startpoint
                int id=contours.at(i).sequence.at(j);
                double start_x= cad.at(id).start.x;
                double start_y= cad.at(id).start.y;
                //save startpoint
                contourpoints.push_back({start_x,start_y});

                //add controlpoints
                for(int k=0; k<cad.at(id).control.size(); k++){

                    double control_x=cad.at(id).control.at(k).x;
                    double control_y=cad.at(id).control.at(k).y;
                    contourpoints.push_back({control_x,control_y});

                }

                //add endpoint
                double end_x= cad.at(id).end.x;
                double end_y= cad.at(id).end.y;
                contourpoints.push_back({end_x,end_y});
            }
        }

        //pip check
        for(int l=0; l<contours.size(); l++){
            if(i!=l){ //don't look at the same contour

                int id=contours.at(l).sequence.at(0); //only look at startpoint of sequence 0 to minimize calculation time
                double x= cad.at(id).start.x;
                double y= cad.at(id).start.y;

                int result=pip(contourpoints,x,y);

                if(result==1){
                    //std::library variant for qt::contains
                    if(std::find(contours.at(i).inside.begin(), contours.at(i).inside.end(), l)!= contours.at(i).inside.end()){
                        // do nothing
                    } else {
                        contours.at(i).inside.push_back(l);
                    }
                }
            }
        }
        contourpoints.clear();
    }
}

int pip(std::vector<std::vector<double>> pos, int x, int y)
{
    int n = pos.size();
    //pos = array of x,y positions of the polygon to investegate
    //x,y = point to check inside the polygon
    //c = 1 if true

    int i, j, c = 0;
    for (i = 0, j = n-1; i < n; j = i++) {
        if ( ((pos[i][1]>y) != (pos[j][1]>y)) && (x < (pos[j][0]-pos[i][0]) * (y-pos[i][1]) / (pos[j][1]-pos[i][1]) + pos[i][0]) ){
            c = !c;
        }
    }
    return c;
}

void colorize_grey(int i){
    cad.at(i).red = 106;
    cad.at(i).green = 106;
    cad.at(i).blue = 106;
    cad.at(i).alpha = 255;
}

void colorize_blue(int i){
    cad.at(i).red = 0;
    cad.at(i).green = 0;
    cad.at(i).blue = 1;
    cad.at(i).alpha = 1;
}

void colorize_yellow(int i){
    cad.at(i).red = 255;
    cad.at(i).green = 255;
    cad.at(i).blue = 0;
    cad.at(i).alpha = 255;
}

#endif




































