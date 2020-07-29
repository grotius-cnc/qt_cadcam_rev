#ifndef FILE_CAM_DEPTH_SEQUENCE
#define FILE_CAM_DEPTH_SEQUENCE
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
    REVISION  : 6.6.2020
    TODO      :
    USAGE     :
    PROTOCOL  : 1. Find lowest depth contour.
                2. Find father of lowest depth contour, select father.
                3. Find brothers of lowest depth contour, select brothers.
                4. Loop trough non selected contours.
    NOTES     : --
*/

#include <iostream>
#include <algorithm>
#include <structs.h>
#include <math.h>
#include <vector>
#include <chrono> //high_resolution_clock

std::vector<int> sequence; //contour sequence, based on keep parts together algoritme

void keep_parts_together();
std::vector<int> find_first_non_selected_autodepth(int depth);
int find_father(int son, int depth);
void find_sons(int father);
void find_brothers(int brother);

void keep_parts_together(){

    auto start = std::chrono::high_resolution_clock::now();

    //reset
    for(int i=0; i<contours.size(); i++){
        contours.at(i).select=false;
    }
    sequence.clear();

    //calculate max depth
    int depth=0;
    for(int i=0; i<contours.size(); i++){
        if(depth<contours.at(i).depth){
            depth=contours.at(i).depth;
        }
    }

    //if total depths is equal, we can not cut a contour set sequence (outside+inside=set)
    if(depth%2==0){
        for(int i=0; i<contours.size(); i++){
            if(contours.at(i).depth==depth){
                contours.at(i).select=true;
                sequence.push_back(i);
                //std::cout<<"contour cut maxdepth: "<<i<<std::endl;
            }
        }
        depth--;
    }

    //class mainloop
    bool stop=0;
    while(!stop){

        //find first non selected at max depth
        std::vector<int> source=find_first_non_selected_autodepth(depth);
        depth=source.at(1);
        if(source.at(0)==-1){break;}

        int father=find_father(source.at(0),depth);
        if(father==-1){
            //no father found, find brothers
            find_brothers(depth);
        }
        if(father!=-1){
            //father found, find sons
            find_sons(father);
            sequence.push_back(father);
            //std::cout<<"father cut: "<<father<<std::endl;
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout<<""<<std::endl;
    std::cout<<"time: "<<elapsed.count()<<" sec"<<std::endl;
    std::cout<<"keep parts together algoritme done!"<<std::endl;
}

std::vector<int> find_first_non_selected_autodepth(int depth){
    std::vector<int> contournumber_depth;
    bool trigger=0;
    while(depth!=-1){
        for(int i=0; i<contours.size(); i++){
            if(contours.at(i).depth==depth && contours.at(i).select==false){
                trigger=1;
                contournumber_depth.push_back(i);
                contournumber_depth.push_back(depth);
                return contournumber_depth;
            }
        }

        if(!trigger){
            depth--;
        }
    }

    contournumber_depth.push_back(-1);
    contournumber_depth.push_back(0);
    return contournumber_depth;
}

int find_father(int son, int depth){
    depth--;
    if(depth==-1){return -1;}

    for(int i=0; i<contours.size(); i++){
        for(int j=0; j<contours.at(i).inside.size(); j++){

            if(contours.at(i).depth==depth && contours.at(i).inside.at(j)==son){
                contours.at(i).select=true;
                return i;
            }
        }
    }
    //std::cout<<"find father: "<<father<<std::endl;
    return -1;
}

void find_sons(int father){
    for(int i=0; i<contours.at(father).inside.size(); i++){
        int x= contours.at(father).inside.at(i);
        if(contours.at(x).select==false){ //no duplicate outputs
            contours.at(x).select=true;
            sequence.push_back(x);
            //std::cout<<"sons cut: "<<contours.at(father).inside.at(i)<<std::endl;
        }
    }
    contours.at(father).select=true;
    //std::cout<<"father cut: "<<father<<std::endl;

}

void find_brothers(int depth){
    for(int i=0; i<contours.size(); i++){
        if(contours.at(i).depth==depth && contours.at(i).select==false){
            contours.at(i).select=true;
            sequence.push_back(i);
            //std::cout<<"brothers cut: "<<i<<std::endl;
        }
    }
}

#endif





















