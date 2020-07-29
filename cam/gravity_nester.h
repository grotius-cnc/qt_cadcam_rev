#ifndef GRAVITY_NESTER_H
#define GRAVITY_NESTER_H
#include <structs.h>
#include <variable.h>
#include <lib/lib_matrix.h>
#include <iostream>
#include <stdio.h>
#include <opengl.h>

#include <lib/lib_intersect.h>
#include <lib/lib_offset.h>
#include <lib/lib_split.h>
#include <lib/lib_pocket.h>
#include <lib/lib_connect.h>
#include <lib/lib_matrix.h>
#include <lib/lib_convert.h>
#include <cam/cam_offset.h>
#include <CavalierContours/include/cavc/polylineoffset.hpp>
#include <CavalierContours/include/cavc/polylineoffsetislands.hpp>
//calculate polygon xy center

struct GRAV{
    int contour;
    double area;
    double poly_cx;
    double poly_cy;
};

int count=0;
int count1=0;



int pips(int a, int b);
POINT gravity_point(int i);

void fit_contour(){

    std::vector<OBJECT> cads;
    int trigger=0;
    bool stop=0;
    double x_pos=0,y_pos=0;
    double x0=0,y0=0,x1=0,y1=0,x_dist=0,y_dist=0,dist=0,min_dist=100000;
    int a=0;
    int b=1;

    //while(!stop){

        x0=cad.at(contours.at(a).sequence.at(count)).start.x;
        y0=cad.at(contours.at(a).sequence.at(count)).start.y;

        x1=cad.at(contours.at(b).sequence.at(count1)).start.x;
        y1=cad.at(contours.at(b).sequence.at(count1)).start.y;

        x_dist=x1-x0;
        y_dist=y1-y0;


        for(unsigned int j=0; j<contours.at(b).sequence.size(); j++){
            cad.at(contours.at(b).sequence.at(j)).start.x-=x_dist;
            cad.at(contours.at(b).sequence.at(j)).start.y-=y_dist;
            cad.at(contours.at(b).sequence.at(j)).end.x-=x_dist;
            cad.at(contours.at(b).sequence.at(j)).end.y-=y_dist;
        }

        bool ok=pips(a,b);
        if(ok==1){
            std::cout<<"not free"<<std::endl;
        } else {
            //cads=cad;
           std::cout<<"free"<<std::endl;
//            POINT ga=gravity_point(a);
//            POINT gb=gravity_point(b);

//            dist=sqrt(pow(ga.x-gb.x,2)+pow(ga.y-gb.y,2));
//            if(min_dist>dist && dist>0){
//                min_dist=dist;
//                std::cout<<"min dist:"<<min_dist<<std::endl;




//            }
        }



        count++;
        if(count==contours.at(a).sequence.size()){
            count=0;
            count1++;
            if(count1==contours.at(b).sequence.size()){
                count1=0;

                std::cout<<"end"<<std::endl;
                std::cout<<"best fit polygon"<<std::endl;
                //move polygon to best fit:
                //break;
            }
        }
    //}
    //cad=cads;
}

POINT gravity_point(int i){
    POINT p;
    double x=0,y=0;
    for(unsigned int j=0; j<contours.at(i).sequence.size(); j++){
        int object=contours.at(i).sequence.at(j);
        x+=cad.at(object).start.x;
        y+=cad.at(object).start.y;
    }
    p.x=x/contours.at(i).sequence.size();
    p.y=y/contours.at(i).sequence.size();
    return p;
}

int pips(int a, int b){

    //pos = array of x,y positions of the polygon to investegate
    //x,y = point to check inside the polygon
    //c = 1 if true
    int i, j,c=0;

    std::vector<std::vector<double>> pos;
    for(unsigned int k=0; k<contours.at(a).sequence.size(); k++){ //a
        double xa = cad.at(contours.at(a).sequence.at(k)).start.x;
        double ya = cad.at(contours.at(a).sequence.at(k)).start.y;
        pos.push_back({xa,ya});
    }
    int n = pos.size();
    int trigger=0;

    for(unsigned int l=0; l<contours.at(b).sequence.size(); l++){ //b

        double x = cad.at(contours.at(b).sequence.at(l)).start.x;
        double y = cad.at(contours.at(b).sequence.at(l)).start.y;

        for (i = 0, j = n-1; i < n; j = i++) {
            if ( ((pos[i][1]>=y) != (pos[j][1]>=y)) && (x < (pos[j][0]-pos[i][0]) * (y-pos[i][1]) / (pos[j][1]-pos[i][1]) + pos[i][0]) ){
                c = !c;
                if(c==1){
                    trigger=1;
                    break;
                }
            }

        }
    }
    return trigger;


}

#endif // GRAVITY_NESTER_H




//        double dist0=sqrt(pow(x0-x,2)+pow(y0-y,2));
//        double dist1=sqrt(pow(x1-x,2)+pow(y1-y,2));
//        double dist2=sqrt(pow(x1-x0,2)+pow(y1-y0,2));
