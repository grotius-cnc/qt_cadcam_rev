#ifndef ZOOM_EXTEND_H
#define ZOOM_EXTEND_H

#include <structs.h>
#include <variable.h>

//opengl when we start up:
//zl=-100;
//zr=100;
//zb=-100;
//zt=100;

void zoom_extend(){ //results in pure rectangular

    double x_min=0,x_max=0,y_min=0,y_max=0;

    std::vector<OBJECT>::iterator it;
    for(it=cad.begin(); it<cad.end(); it++){
        if(x_min>it->start.x){
            x_min=it->start.x;
        }
        if(x_max<it->start.x){
            x_max=it->start.x;
        }

        if(x_min>it->end.x){
            x_min=it->end.x;
        }
        if(x_max<it->end.x){
            x_max=it->end.x;
        }

        if(y_min>it->start.y){
            y_min=it->start.y;
        }
        if(y_max<it->start.y){
            y_max=it->start.y;
        }

        if(y_min>it->end.y){
            y_min=it->end.y;
        }
        if(y_max<it->end.y){
            y_max=it->end.y;
        }

        double space_x=x_max-x_min;
        double space_y=y_max-y_min;
        double cx=(x_min+x_max)/2;
        double cy=(y_min+y_max)/2;

        if(space_x>space_y){
            zl=cx-(space_x);
            zr=cx+(space_x);
            zt=cy+(space_x);
            zb=cy-(space_x);
        } else {

            zt=cy+(space_y);
            zb=cy-(space_y);
            zl=cx-(space_y);
            zr=cx+(space_y);
        }
    }
}


#endif // ZOOM_EXTEND_H
