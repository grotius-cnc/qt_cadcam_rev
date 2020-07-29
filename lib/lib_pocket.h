#ifndef LIB_POCKET_H
#define LIB_POCKET_H
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
    DATE      : 25.6.2020
    REVISION  : 27.6.2020   made compatible for data input from lib_split.h
    TODO      : This code can be expanded with xy coordinate input to calculate the area's
    USAGE     : See the online pdf documentation of this file at: https://forum.linuxcnc.org/41-guis/36768-qt-c-code-samples?start=160
    PROTOCOL  : --
    NOTES     : This code took me about 7 hours.
*/
#include <iostream>
#include <algorithm>
#include <math.h>
#include <vector>
#include <structs.h>

//data containers
std::vector<SECTOR> sectors;
int current_sector=0;
int sectorsize=0;
std::vector<int> closed_sectors;
std::vector<int> used_intersections;

bool check_if_this_intersection_is_used_before(std::vector<OBJECT>::iterator it);
int check_for_previous_closed_sectors_numbers_take_the_first_free_number(bool increase, bool decrease, int current_sector);
std::vector<OBJECT> calculate_sector_area(std::vector<OBJECT> array);
double sum_plus_controlpoints(int i);

std::vector<OBJECT> pocket_algoritme(std::vector<OBJECT> array){
    std::vector<OBJECT>::iterator it;

    sectorsize=0; //reset
    current_sector=0; //reset
    closed_sectors.clear();
    used_intersections.clear();
    sectors.clear();

    for(it=array.begin(); it<array.end(); it++){

        //startup
        if(it==array.begin()){
            it->sector=current_sector;
            if(it->end_intersection==-1){ //-1 means, no end intersection
                //do nothing
            } else {
                //store more data
                used_intersections.push_back(it->end_intersection); //store already spotted intersections in a separate list.
                current_sector++; //increase for next primitive
            }
        }

        //cyclus
        if(it>array.begin()){ //it is now increased
            //store the current sector
            it->sector=current_sector;

            if(it->end_intersection==-1){ //-1 means, no end intersection
                //do nothing


            } else {
                //we have a end intersection at this point, look if this intersection is used before. Look in the used_intersection container.
                bool check=check_if_this_intersection_is_used_before(it);
                //std::cout<<"check: "<<check<<std::endl;
                if(check){
                    closed_sectors.push_back(it->sector);
                    //decrease, look how much we have to decrease.
                    int free_sector=check_for_previous_closed_sectors_numbers_take_the_first_free_number(0,1,it->sector);
                    //std::cout<<"decreased free_sector: "<<free_sector<<std::endl;

                    current_sector=free_sector;

                    //std::cout<<"closed sector stored: "<< closed_sectors.back()<<std::endl;
                }
                if(!check){
                    //increase
                    int free_sector=check_for_previous_closed_sectors_numbers_take_the_first_free_number(1,0,it->sector);
                    //std::cout<<"increase free_number: "<<free_sector<<std::endl;

                    current_sector=free_sector;
                }

                //save the current end intersection
                used_intersections.push_back(it->end_intersection);
            }

        }
    }

    //print data
    int no=0;
    for(it=array.begin(); it<array.end(); it++){
        //        std::cout<<"primitive no: "<<no<<std::endl;
        //        std::cout<<"end intersection: "<<it->end_intersection<<std::endl;
        //        std::cout<<"sector: "<<it->sector<<std::endl;
        //        std::cout<<""<<std::endl;
        no++;
    }

    //get the sectorsize
    for(it=array.begin(); it<array.end(); it++){
        if(it->sector>sectorsize){
            sectorsize=it->sector;
        }
    }
    sectors.resize(sectorsize+1);
    std::cout<<"sectorsize"<<sectorsize<<std::endl;

    std::vector<OBJECT> ccw_array;
    ccw_array=calculate_sector_area(array);
    return ccw_array;
}

std::vector<OBJECT> calculate_sector_area(std::vector<OBJECT> array){

    std::vector<OBJECT>::iterator it;

    for(int i=0; i<sectorsize+1; i++){
        double sum=0;
        for(it=array.begin(); it<array.end(); it++){

            if(it->sector==i){
                if(it->type=="line"){
                    sum+= (it->end.x-it->start.x) * (it->end.y+it->start.y);
                }
                if(it->type=="arc"){
                    sum+= (it->control.front().x-it->start.x) * (it->control.front().y+it->start.y);
                    sum+= (it->end.x-it->control.front().x) * (it->end.y+it->control.front().y);
                }
            }

        }
        //std::cout<<"sum: "<<sum/2<<std::endl;
        sectors.at(i).area=sum/2;
    }

    for(int i=0; i<sectorsize+1; i++){ //colorize sectors that we need to green
        for(it=array.begin(); it<array.end(); it++){
            if(sectors.at(i).area<0 && it->sector==i){

                it->red=0; it->green=255; it->blue=0; it->alpha=0;

            }

        }
    }
    return array;
}

std::vector<OBJECT> clean_up_the_pocket(std::vector<OBJECT> cam){ //delete what we don't need

    std::vector<OBJECT> ccw_out;
    std::vector<OBJECT>::iterator it;
    for(it=cam.begin(); it<cam.end(); it++){
        if(it->red==0 && it->green==255 && it->blue==0 && it->alpha==0){
            ccw_out.push_back(*it);
        }
    }
    return ccw_out;
}


bool check_if_this_intersection_is_used_before(std::vector<OBJECT>::iterator it){

    auto result = find (used_intersections.begin(), used_intersections.end(), it->end_intersection);
    if (result != used_intersections.end()){
        return true;
    } else { return false;}
}

int check_for_previous_closed_sectors_numbers_take_the_first_free_number(bool increase, bool decrease, int current_sector){

    if(decrease){
        int search_sector=current_sector-1; //begin the search with one decrement
        bool found=false;
        do {
            auto it = find (closed_sectors.begin(), closed_sectors.end(), search_sector);
            if (it != closed_sectors.end()){
                //value found
                //do nothing, decrease until we found nothing
                search_sector--;

            } else {
                //value not found, this is oke. we found a free sector
                found=true;
                return search_sector;
            }
        } while (!found);
    }


    if(increase){
        int search_sector=current_sector+1; //begin the search with one increment
        bool found=false;
        do {
            auto it = find (closed_sectors.begin(), closed_sectors.end(), search_sector);
            if (it != closed_sectors.end()){
                //value found
                //do nothing, decrease until we found nothing
                search_sector++;

            } else {
                //value not found, this is oke. we found a free sector
                found=true;
                return search_sector;
            }
        } while (!found);
    }

    //return -1; //error
}

#endif // _LIBPOCKET_H






























