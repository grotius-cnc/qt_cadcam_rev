#ifndef FILE_INTERSECT
#define FILE_INTERSECT
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
    DATE      : 12.6.2020
    REVISION  : --
    TODO      : --
    USAGE     : --
    PROTOCOL  : --
    NOTES     : --
*/
#include <iostream>
#include <algorithm>
#include <math.h>
#include <structs.h>
#include <variable.h>

//POINT line_time(POINT start,POINT end,double time);
void calculate_line_line_intersection(std::vector<OBJECT>::iterator it,std::vector<OBJECT>::iterator it1, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
bool check_if_point_is_on_arc(std::vector<OBJECT>::iterator it, POINT p);

void calculate_line_line_intersection(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4){

    //line 1 to compare :
    double startpoint_x0 = x1;
    double startpoint_y0 = y1;
    double endpoint_x0 = x2;
    double endpoint_y0 = y2;

    //line 2 to compare :
    double startpoint_x1 = x3;
    double startpoint_y1 = y3;
    double endpoint_x1 = x4;
    double endpoint_y1 = y4;

    //line 1
    double delta_y0 = endpoint_y0 - startpoint_y0;
    double delta_x0 = startpoint_x0 - endpoint_x0;
    double c0 = delta_y0 * startpoint_x0 + delta_x0 * startpoint_y0;

    //line 2
    double delta_y1 = endpoint_y1 - startpoint_y1;
    double delta_x1 = startpoint_x1 - endpoint_x1;
    double c1 = delta_y1 * startpoint_x1 + delta_x1 * startpoint_y1;

    double determinant = delta_y0*delta_x1 - delta_y1*delta_x0;

    if (determinant == 0){
        //std::cout<< "the lines are parallel"<<std::endl;
    } else  {
        double x = (delta_x1*c0 - delta_x0*c1)/determinant;
        double y = (delta_y0*c1 - delta_y1*c0)/determinant;

        double l1=sqrt(pow(x1-x,2)+pow(y1-y,2)); //check if intersection x,y is on the line1 and line2 segment.
        double l2=sqrt(pow(x2-x,2)+pow(y2-y,2));
        double l3=sqrt(pow(x2-x1,2)+pow(y2-y1,2));

        double l4=sqrt(pow(x3-x,2)+pow(y3-y,2));
        double l5=sqrt(pow(x4-x,2)+pow(y4-y,2));
        double l6=sqrt(pow(x4-x3,2)+pow(y4-y3,2));

        //avoid intersection on endpoint or startpoint of it. it1 is free and used for linestrips
        if((x>x1-0.01 && x<x1+0.01 && y<y1+0.01 && y>y1-0.01) || (x>x2-0.01 && x<x2+0.01 && y<y2+0.01 && y>y2-0.01) /*|| (x>x3-0.01 && x<x3+0.01 && y<y3+0.01 && y>y3-0.01) || (x>x4-0.01 && x<x4+0.01 && y<y4+0.01 && y>y4-0.01)*/ ){
            //do nothing
        } else

            if((l3>l1+l2-0.01 && l3<l1+l2+0.01) && (l6>l4+l5-0.01 && l6<l4+l5+0.01)){ //linestrip self intersection spotted
                it->intersect.push_back({x,y,0});
                it1->intersect.push_back({x,y,0});
                //std::cout<< " intersection point is x : " << x << " y : " << y<<std::endl;
            }
    }
}

bool check_if_point_is_on_arc(std::vector<OBJECT>::iterator it, POINT p){ //this function is quite difficult to design.

    double x1 = it->start.x;
    double y1 = it->start.y;
    double x2 = it->end.x;
    double y2 = it->end.y;
    double xp = it->control.front().x;
    double yp = it->control.front().y;
    double determinant = ((x2-x1)*(yp-y1))-((y2-y1)*(xp-x1));
    double deg_angle_start=0,deg_angle_end=0;

    if(determinant > 0){ //g2
        deg_angle_start = 180/M_PI*atan2(y2-it->center.y, x2-it->center.x);
        deg_angle_end = 180/M_PI*atan2(y1-it->center.y, x1-it->center.x);
    }
    if(determinant < 0){ //g3
        deg_angle_start = 180/M_PI*atan2(y1-it->center.y, x1-it->center.x);
        deg_angle_end  = 180/M_PI*atan2(y2-it->center.y, x2-it->center.x);
    }

    if(deg_angle_start<0){deg_angle_start=360-(deg_angle_start*-1);}
    if(deg_angle_end<0){deg_angle_end=360-(deg_angle_end*-1);}

    double pi_angle_p=atan2(p.y-it->center.y,p.x-it->center.x);
    double deg_angle_p=(180/M_PI)*pi_angle_p;
    //Angles defined from 0 to 360 degrees. if angle<0, make them positive
    if(deg_angle_p<0){deg_angle_p=360-(deg_angle_p*-1);}


    /*option 1
                                      |90
                                      |  *point
                                      |
                            *end      |         *start
                            __________|__________ 0
                           180        |           360
                                      |
                                      |
                                      |270               */

    if(deg_angle_start<deg_angle_end){
        if(deg_angle_p>deg_angle_start && deg_angle_p<deg_angle_end){
            return true;
        }
    }

    /*option 2
                                      |90
                                      |       *end
                                      |
                                      |         *point
                            __________|__________ 0
                           180        |           360
                                      |
                                      |       *start
                                      |270               */

    if(deg_angle_start>deg_angle_end){
        if(deg_angle_p<deg_angle_start && deg_angle_p<deg_angle_end){
            return true;
        }
    }

    /*option 3
                                      |90
                                      |       *end
                                      |
                                      |
                            __________|__________ 0
                           180        |           360
                                      |        *point
                                      |       *start
                                      |270               */

    if(deg_angle_start>deg_angle_end){
        if(deg_angle_p>deg_angle_start && deg_angle_p>deg_angle_end ){
            return true;
        }
    }
    return false;
}

void linestrip_arc_intersect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1){

    double x1 = it->start.x;  //intersect startpoint to first controlpoint
    double y1 = it->start.y;
    double x2 = it->control.front().x;
    double y2 = it->control.front().y;
    double x3 = it1->start.x;
    double y3 = it1->start.x;
    double x4 = it1->end.x;
    double y4 = it1->end.x;

    double circle_center_x = it1->center.x;
    double circle_center_y = it1->center.y;
    double radius = it1->radius;
    double dx, dy, A, B, C, det, t;
    double ix=0;
    double iy=0;
    double ix1=0;
    double iy1=0;
    dx = x2 - x1;
    dy = y2 - y1;
    A = dx * dx + dy * dy;
    B = 2 * (dx * (x1 - circle_center_x) + dy * (y1 - circle_center_y));
    C = (x1 - circle_center_x) * (x1 - circle_center_x) + (y1 - circle_center_y) * (y1 - circle_center_y) - radius * radius;
    det = B * B - 4 * A * C;
    if ((A <= 0.0000001) || (det < 0)){
        //no solutions
    } else if (det==0){ //one solution
        t = -B / (2 * A);
        ix = (x1 + t * dx);
        iy = (y1 + t * dy);
        double l0=sqrt(pow(x1-ix,2)+pow(y1-iy,2));  //check if intersection is on the linestrip segment
        double l1=sqrt(pow(x2-ix,2)+pow(y2-iy,2));
        double l2=sqrt(pow(x1-x2,2)+pow(y1-y2,2));
        if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
            if(check){
                //don't take the linestrip startpoint as intersection
                if(ix>x1-0.01 && ix<x1+0.01 && iy>y1-0.01 && iy<y1+0.01){ //arc start
                    //do nothing
                } else {
                    it->intersect.push_back({ix,iy,0});
                    it1->intersect.push_back({ix,iy,0});
                }
            }
        }
    } else { //two solutions
        t = (-B + sqrt(det)) / (2 * A);
        ix = (x1 + t * dx);
        iy = (y1 + t * dy);
        t = (-B - sqrt(det)) / (2 * A);
        ix1 = (x1 + t * dx);
        iy1 = (y1 + t * dy);
        double l0=sqrt(pow(x1-ix,2)+pow(y1-iy,2)); //check if intersection is on the linestrip segment
        double l1=sqrt(pow(x2-ix,2)+pow(y2-iy,2));
        double l2=sqrt(pow(x1-x2,2)+pow(y1-y2,2));
        if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
            if(check){
                //don't take the linestrip startpoint as intersection
                if(ix>x1-0.01 && ix<x1+0.01 && iy>y1-0.01 && iy<y1+0.01 ){
                    //do nothing
                } else {
                    it->intersect.push_back({ix,iy,0});
                    it1->intersect.push_back({ix,iy,0});
                }
            }
        }
        l0=sqrt(pow(x1-ix1,2)+pow(y1-iy1,2));
        l1=sqrt(pow(x2-ix1,2)+pow(y2-iy1,2));
        l2=sqrt(pow(x1-x2,2)+pow(y1-y2,2));
        if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix1,iy1,0});
            if(check){
                //don't take the linestrip startpoint as intersection
                if(ix1>x1-0.01 && ix1<x1+0.01 && iy1>y1-0.01 && iy1<y1+0.01 ){
                    //do nothing
                } else {
                    it->intersect.push_back({ix1,iy1,0});
                    it1->intersect.push_back({ix1,iy1,0});
                }
            }
        }
    }

    std::vector<POINT>::iterator it3,it4;
    for(it3=it->control.begin(); it3<it->control.end()-1; it3++){
        it4=it3+1;
        double x1 = it3->x;  //intersect controlpoints
        double y1 = it3->y;
        double x2 = it4->x;
        double y2 = it4->y;
        double circle_center_x = it1->center.x;
        double circle_center_y = it1->center.y;
        double radius = it1->radius;
        double dx, dy, A, B, C, det, t;
        double ix=0;
        double iy=0;
        double ix1=0;
        double iy1=0;
        dx = x2 - x1;
        dy = y2 - y1;
        A = dx * dx + dy * dy;
        B = 2 * (dx * (x1 - circle_center_x) + dy * (y1 - circle_center_y));
        C = (x1 - circle_center_x) * (x1 - circle_center_x) + (y1 - circle_center_y) * (y1 - circle_center_y) - radius * radius;
        det = B * B - 4 * A * C;
        if ((A <= 0.0000001) || (det < 0)){
            //no solutions
        } else if (det==0){ //one solution
            t = -B / (2 * A);
            ix = (x1 + t * dx);
            iy = (y1 + t * dy);
            double l0=sqrt(pow(x1-ix,2)+pow(y1-iy,2));  //check if intersection is on the linestrip segment
            double l1=sqrt(pow(x2-ix,2)+pow(y2-iy,2));
            double l2=sqrt(pow(x1-x2,2)+pow(y1-y2,2));
            if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
                bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
                if(check){
                    it->intersect.push_back({ix,iy,0});
                    it1->intersect.push_back({ix,iy,0});
                }
            }
        } else { //two solutions
            t = (-B + sqrt(det)) / (2 * A);
            ix = (x1 + t * dx);
            iy = (y1 + t * dy);
            t = (-B - sqrt(det)) / (2 * A);
            ix1 = (x1 + t * dx);
            iy1 = (y1 + t * dy);
            double l0=sqrt(pow(x1-ix,2)+pow(y1-iy,2)); //check if intersection is on the linestrip segment
            double l1=sqrt(pow(x2-ix,2)+pow(y2-iy,2));
            double l2=sqrt(pow(x1-x2,2)+pow(y1-y2,2));
            if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
                bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
                if(check){
                    it->intersect.push_back({ix,iy,0});
                    it1->intersect.push_back({ix,iy,0});
                }
            }
            l0=sqrt(pow(x1-ix1,2)+pow(y1-iy1,2));
            l1=sqrt(pow(x2-ix1,2)+pow(y2-iy1,2));
            l2=sqrt(pow(x1-x2,2)+pow(y1-y2,2));
            if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
                bool check=check_if_point_is_on_arc(it1,{ix1,iy1,0});
                if(check){
                    it->intersect.push_back({ix1,iy1,0});
                    it1->intersect.push_back({ix1,iy1,0});
                }
            }
        }
    }

    x1 = it->control.back().x;  //intersect last controlpoint to endpoint
    y1 = it->control.back().y;
    x2 = it->end.x;
    y2 = it->end.y;
    x3 = it1->start.x;
    y3 = it1->start.x;
    x4 = it1->end.x;
    y4 = it1->end.x;
    circle_center_x = it1->center.x;
    circle_center_y = it1->center.y;
    radius = it1->radius;
    dx=0; dy=0; A=0; B=0; C=0; det=0; t=0;
    ix=0;
    iy=0;
    ix1=0;
    iy1=0;
    dx = x2 - x1;
    dy = y2 - y1;
    A = dx * dx + dy * dy;
    B = 2 * (dx * (x1 - circle_center_x) + dy * (y1 - circle_center_y));
    C = (x1 - circle_center_x) * (x1 - circle_center_x) + (y1 - circle_center_y) * (y1 - circle_center_y) - radius * radius;
    det = B * B - 4 * A * C;
    if ((A <= 0.0000001) || (det < 0)){
        //no solutions
    } else if (det==0){ //one solution
        t = -B / (2 * A);
        ix = (x1 + t * dx);
        iy = (y1 + t * dy);
        double l0=sqrt(pow(x1-ix,2)+pow(y1-iy,2));  //check if intersection is on the linestrip segment
        double l1=sqrt(pow(x2-ix,2)+pow(y2-iy,2));
        double l2=sqrt(pow(x1-x2,2)+pow(y1-y2,2));
        if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
            if(check){
                //don't take the linestrip endpoint as intersection
                if(ix>x2-0.01 && ix<x2+0.01 && iy>y2-0.01 && iy<y2+0.01 ){
                    //do nothing
                } else {
                    it->intersect.push_back({ix,iy,0});
                    it1->intersect.push_back({ix,iy,0});
                }
            }
        }
    } else { //two solutions
        t = (-B + sqrt(det)) / (2 * A);
        ix = (x1 + t * dx);
        iy = (y1 + t * dy);
        t = (-B - sqrt(det)) / (2 * A);
        ix1 = (x1 + t * dx);
        iy1 = (y1 + t * dy);
        double l0=sqrt(pow(x1-ix,2)+pow(y1-iy,2)); //check if intersection is on the linestrip segment
        double l1=sqrt(pow(x2-ix,2)+pow(y2-iy,2));
        double l2=sqrt(pow(x1-x2,2)+pow(y1-y2,2));
        if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
            if(check){
                //don't take the linestrip endpoint as intersection
                if(ix>x2-0.01 && ix<x2+0.01 && iy>y2-0.01 && iy<y2+0.01 ){
                    //do nothing
                } else {
                    it->intersect.push_back({ix,iy,0});
                    it1->intersect.push_back({ix,iy,0});
                }
            }
        }
        l0=sqrt(pow(x1-ix1,2)+pow(y1-iy1,2));
        l1=sqrt(pow(x2-ix1,2)+pow(y2-iy1,2));
        l2=sqrt(pow(x1-x2,2)+pow(y1-y2,2));
        if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix1,iy1,0});
            if(check){
                //don't take the linestrip endpoint as intersection
                if(ix1>x2-0.01 && ix1<x2+0.01 && iy1>y2-0.01 && iy1<y2+0.01 ){
                    //do nothing
                } else {
                    it->intersect.push_back({ix1,iy1,0});
                    it1->intersect.push_back({ix1,iy1,0});
                }
            }
        }
    }
}

void line_arc_intersect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1) //http://csharphelper.com/blog/2014/09/determine-where-a-line-intersects-a-circle-in-c/
{
    double start_line_x1 = it->start.x;
    double start_line_y1 = it->start.y;
    double end_line_x2 = it->end.x;
    double end_line_y2 = it->end.y;
    double circle_center_x = it1->center.x;
    double circle_center_y = it1->center.y;
    double radius = it1->radius;
    double dx, dy, A, B, C, det, t;
    double ix=0;
    double iy=0;
    double ix1=0;
    double iy1=0;
    dx = end_line_x2 - start_line_x1;
    dy = end_line_y2 - start_line_y1;
    A = dx * dx + dy * dy;
    B = 2 * (dx * (start_line_x1 - circle_center_x) + dy * (start_line_y1 - circle_center_y));
    C = (start_line_x1 - circle_center_x) * (start_line_x1 - circle_center_x) + (start_line_y1 - circle_center_y) * (start_line_y1 - circle_center_y) - radius * radius;
    det = B * B - 4 * A * C;
    if ((A <= 0.0000001) || (det < 0)){
        //no solutions
    } else if (det==0){
        //one solution
        t = -B / (2 * A);
        ix = (start_line_x1 + t * dx);
        iy = (start_line_y1 + t * dy);

        //check if intersection is on the line segment and inside the arc section
        double l0=sqrt(pow(it->start.x-ix,2)+pow(it->start.y-iy,2));
        double l1=sqrt(pow(it->end.x-ix,2)+pow(it->end.y-iy,2));
        double ltot=sqrt(pow(it->start.x-it->end.x,2)+pow(it->start.y-it->end.y,2));
        if(ltot>(l0+l1)-0.01 && ltot<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
            //check if points are next or previous attached line start,endpoint
            if((it1->end.x<it->start.x+0.01 && it1->end.x>it->start.x-0.01 && it1->end.y<it->start.y+0.01 && it1->end.y>it->start.y-0.01 ) ||
                    (it1->start.x<it->end.x+0.01 && it1->start.x>it->end.x-0.01 && it1->start.y<it->end.y+0.01 &&  it1->start.y>it->end.y-0.01)){
                check=false;
            }
            if(check){it->intersect.push_back({ix,iy,0});}
            if(check){it1->intersect.push_back({ix,iy,0});}
            //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
        }
    } else {
        t = (-B + sqrt(det)) / (2 * A);
        ix = (start_line_x1 + t * dx);
        iy = (start_line_y1 + t * dy);

        t = (-B - sqrt(det)) / (2 * A);
        ix1 = (start_line_x1 + t * dx);
        iy1 = (start_line_y1 + t * dy);

        //check if intersection is on the line segment and inside the arc section
        double l0=sqrt(pow(it->start.x-ix,2)+pow(it->start.y-iy,2));
        double l1=sqrt(pow(it->end.x-ix,2)+pow(it->end.y-iy,2));
        double ltot=sqrt(pow(it->start.x-it->end.x,2)+pow(it->start.y-it->end.y,2));
        if(ltot>(l0+l1)-0.01 && ltot<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
            //check if points are next or previous attached line start,endpoint
            if((it1->end.x<it->start.x+0.01 && it1->end.x>it->start.x-0.01 && it1->end.y<it->start.y+0.01 && it1->end.y>it->start.y-0.01 ) ||
                    (it1->start.x<it->end.x+0.01 && it1->start.x>it->end.x-0.01 && it1->start.y<it->end.y+0.01 &&  it1->start.y>it->end.y-0.01)){
                check=false;
            }
            if(check){
                it->intersect.push_back({ix,iy,0});
                it1->intersect.push_back({ix,iy,0});
                //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
            }

        }

        double l2=sqrt(pow(it->start.x-ix1,2)+pow(it->start.y-iy1,2));
        double l3=sqrt(pow(it->end.x-ix1,2)+pow(it->end.y-iy1,2));
        double ltot1=sqrt(pow(it->start.x-it->end.x,2)+pow(it->start.y-it->end.y,2));
        if(ltot1>(l2+l3)-0.01 && ltot1<(l2+l3)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix1,iy1,0});
            //check if points are next or previous attached line start,endpoint
            if((it1->end.x<it->start.x+0.01 && it1->end.x>it->start.x-0.01 && it1->end.y<it->start.y+0.01 && it1->end.y>it->start.y-0.01 ) ||
                    (it1->start.x<it->end.x+0.01 && it1->start.x>it->end.x-0.01 && it1->start.y<it->end.y+0.01 &&  it1->start.y>it->end.y-0.01)){
                check=false;
            }
            if(check){
                it->intersect.push_back({ix1,iy1,0});
                it1->intersect.push_back({ix1,iy1,0});
                //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
            }
        }
    }
}

void arc_line_intersect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1) //http://csharphelper.com/blog/2014/09/determine-where-a-line-intersects-a-circle-in-c/
{
    double start_line_x1 = it1->start.x;
    double start_line_y1 = it1->start.y;
    double end_line_x2 = it1->end.x;
    double end_line_y2 = it1->end.y;
    double circle_center_x = it->center.x;
    double circle_center_y = it->center.y;
    double radius = it->radius;
    double dx, dy, A, B, C, det, t;
    double ix=0;
    double iy=0;
    double ix1=0;
    double iy1=0;
    dx = end_line_x2 - start_line_x1;
    dy = end_line_y2 - start_line_y1;
    A = dx * dx + dy * dy;
    B = 2 * (dx * (start_line_x1 - circle_center_x) + dy * (start_line_y1 - circle_center_y));
    C = (start_line_x1 - circle_center_x) * (start_line_x1 - circle_center_x) + (start_line_y1 - circle_center_y) * (start_line_y1 - circle_center_y) - radius * radius;
    det = B * B - 4 * A * C;
    if ((A <= 0.0000001) || (det < 0)){
        //no solutions
    } else if (det==0){
        //one solution
        t = -B / (2 * A);
        ix = (start_line_x1 + t * dx);
        iy = (start_line_y1 + t * dy);

        //check if intersection is on the line segment and inside the arc section
        double l0=sqrt(pow(it1->start.x-ix,2)+pow(it1->start.y-iy,2));
        double l1=sqrt(pow(it1->end.x-ix,2)+pow(it1->end.y-iy,2));
        double ltot=sqrt(pow(it1->start.x-it1->end.x,2)+pow(it1->start.y-it1->end.y,2));
        if(ltot>(l0+l1)-0.01 && ltot<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it,{ix,iy,0});
            //check if points are next or previous attached line start,endpoint
            if((it->end.x<it1->start.x+0.01 && it->end.x>it1->start.x-0.01 && it->end.y<it1->start.y+0.01 && it->end.y>it1->start.y-0.01 ) ||
                    (it->start.x<it1->end.x+0.01 && it->start.x>it1->end.x-0.01 && it->start.y<it1->end.y+0.01 &&  it->start.y>it1->end.y-0.01)){
                check=false;
            }
            if(check){it->intersect.push_back({ix,iy,0});}
            if(check){it1->intersect.push_back({ix,iy,0});}
            //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
        }
    } else {
        t = (-B + sqrt(det)) / (2 * A);
        ix = (start_line_x1 + t * dx);
        iy = (start_line_y1 + t * dy);

        t = (-B - sqrt(det)) / (2 * A);
        ix1 = (start_line_x1 + t * dx);
        iy1 = (start_line_y1 + t * dy);

        //check if intersection is on the line segment and inside the arc section
        double l0=sqrt(pow(it1->start.x-ix,2)+pow(it1->start.y-iy,2));
        double l1=sqrt(pow(it1->end.x-ix,2)+pow(it1->end.y-iy,2));
        double ltot=sqrt(pow(it1->start.x-it1->end.x,2)+pow(it1->start.y-it1->end.y,2));
        if(ltot>(l0+l1)-0.01 && ltot<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it,{ix,iy,0});
            //check if points are next or previous attached line start,endpoint
            if((it->end.x<it1->start.x+0.01 && it->end.x>it1->start.x-0.01 && it->end.y<it1->start.y+0.01 && it->end.y>it1->start.y-0.01 ) ||
                    (it->start.x<it1->end.x+0.01 && it->start.x>it1->end.x-0.01 && it->start.y<it1->end.y+0.01 &&  it->start.y>it1->end.y-0.01)){
                check=false;
            }
            if(check){
                it->intersect.push_back({ix,iy,0});
                it1->intersect.push_back({ix,iy,0});
                //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
            }

        }

        double l2=sqrt(pow(it1->start.x-ix1,2)+pow(it1->start.y-iy1,2));
        double l3=sqrt(pow(it1->end.x-ix1,2)+pow(it1->end.y-iy1,2));
        double ltot1=sqrt(pow(it1->start.x-it1->end.x,2)+pow(it1->start.y-it1->end.y,2));
        if(ltot1>(l2+l3)-0.01 && ltot1<(l2+l3)+0.01){
            bool check=check_if_point_is_on_arc(it,{ix1,iy1,0});
            //check if points are next or previous attached line start,endpoint
            if((it->end.x<it1->start.x+0.01 && it->end.x>it1->start.x-0.01 && it->end.y<it1->start.y+0.01 && it->end.y>it1->start.y-0.01 ) ||
                    (it->start.x<it1->end.x+0.01 && it->start.x>it1->end.x-0.01 && it->start.y<it1->end.y+0.01 &&  it->start.y>it1->end.y-0.01)){
                check=false;
            }
            if(check){
                it->intersect.push_back({ix1,iy1,0});
                it1->intersect.push_back({ix1,iy1,0});
                //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
            }
        }
    }
}

void linestrip_line_intersect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1)
{
    std::vector<POINT>::iterator it2,it3;

    //1.it startline with it1 line
    //2.it endline with it1 line
    //3.it controllines to it1 line

    //1.it startline with it1 line
    //line1
    double x1=it1->start.x;
    double y1=it1->start.y;
    double x2=it1->end.x;
    double y2=it1->end.y;
    //line2
    double x3=it->start.x;
    double y3=it->start.y;
    double x4=it->control.front().x;
    double y4=it->control.front().y;
    calculate_line_line_intersection(it,it1,x1,y1,x2,y2,x3,y3,x4,y4); //on it1 there is no check for start or endpoint

    //2.it endline with it1 line
    //line2
    x1=it1->start.x;
    y1=it1->start.y;
    x2=it1->end.x;
    y2=it1->end.y;
    //line1
    x3=it->control.back().x;
    y3=it->control.back().y;
    x4=it->end.x;
    y4=it->end.y;
    calculate_line_line_intersection(it,it1,x1,y1,x2,y2,x3,y3,x4,y4);

    //3.it controllines to it1 line
    for(it2=it->control.begin(); it2<it->control.end()-1; it2++){
        it3=it2+1;
        //line1
        double x1=it1->start.x;
        double y1=it1->start.y;
        double x2=it1->end.x;
        double y2=it1->end.y;
        //line2
        double x3=it2->x;
        double y3=it2->y;
        double x4=it3->x;
        double y4=it3->y;
        calculate_line_line_intersection(it,it1,x1,y1,x2,y2,x3,y3,x4,y4);
    }
}

void line_line_intersect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1)
{

    //line 1
    double x1=it->start.x;
    double y1=it->start.y;
    double x2=it->end.x;
    double y2=it->end.y;
    //line 2
    double x3=it1->start.x;
    double y3=it1->start.y;
    double x4=it1->end.x;
    double y4=it1->end.y;

    POINT p;
    p.x=((((x1*y2)-(y1*x2))*(x3-x4))-((x1-x2)*(x3*y4-y3*x4))) / (((x1-x2)*(y3-y4))-((y1-y2)*(x3-x4))); //wikipedia line-line intersection
    p.y=((((x1*y2)-(y1*x2))*(y3-y4))-((y1-y2)*(x3*y4-y3*x4))) / (((x1-x2)*(y3-y4))-((y1-y2)*(x3-x4)));

    //check if intersection px,py is in the line1 and line2 segment.
    double l1_0=sqrt(pow(x1-p.x,2)+pow(y1-p.y,2));
    double l1_1=sqrt(pow(x2-p.x,2)+pow(y2-p.y,2));
    double l1_tot=sqrt(pow(x2-x1,2)+pow(y2-y1,2));

    double l2_0=sqrt(pow(x3-p.x,2)+pow(y3-p.y,2));
    double l2_1=sqrt(pow(x4-p.x,2)+pow(y4-p.y,2));
    double l2_tot=sqrt(pow(x4-x3,2)+pow(y4-y3,2));

    //avoid intersection on endpoint or startpoint
    if((p.x>x1-0.01 && p.x<x1+0.01 && p.y<y1+0.01 && p.y>y1-0.01) || (p.x>x2-0.01 && p.x<x2+0.01 && p.y<y2+0.01 && p.y>y2-0.01) || (p.x>x3-0.01 && p.x<x3+0.01 && p.y<y3+0.01 && p.y>y3-0.01) || (p.x>x4-0.01 && p.x<x4+0.01 && p.y<y4+0.01 && p.y>y4-0.01) ){
        //do nothing
    } else

        if((l1_tot>l1_0+l1_1-0.01 && l1_tot<l1_0+l1_1+0.01) && (l2_tot>l2_0+l2_1-0.01 && l2_tot<l2_0+l2_1+0.01)){ //linestrip self intersection spotted
            //std::cout<<"self intersecton found at x: "<<p.x<<" y: "<<p.y<<std::endl;
            it->intersect.push_back(p);
        }
}

void linestrip_linestrip_intersect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1)
{
    std::vector<POINT>::iterator it2,it3,it4,it5;

    //this is a huge one:
    //1.it startline with it1 startline
    //2.it startline with it1 controllines
    //3.it startline with it1 endline
    //4.it endine to it1 startline
    //5.it endline to it1 controllines
    //6.it endline to it1 endline
    //7.it controllines to it1 controllines

    //1.it startline with it1 startline
    //line1
    double x1=it->start.x;
    double y1=it->start.y;
    double x2=it->control.front().x;
    double y2=it->control.front().y;
    //line2
    double x3=it1->start.x;
    double y3=it1->start.y;
    double x4=it1->control.front().x;
    double y4=it1->control.front().y;
    calculate_line_line_intersection(it,it1,x1,y1,x2,y2,x3,y3,x4,y4);

    //2.it startline with it1 controllines
    for(it2=it1->control.begin(); it2<it1->control.end()-1; it2++){
        it3=it2+1;
        //line1
        double x1=it->start.x;
        double y1=it->start.y;
        double x2=it->control.front().x;
        double y2=it->control.front().y;
        //line2
        double x3=it2->x;
        double y3=it2->y;
        double x4=it3->x;
        double y4=it3->y;
        calculate_line_line_intersection(it,it1,x1,y1,x2,y2,x3,y3,x4,y4);
    }

    //3.it startline with it1 endline
    //line1
    x1=it->start.x;
    y1=it->start.y;
    x2=it->control.front().x;
    y2=it->control.front().y;
    //line2
    x3=it1->control.back().x;
    y3=it1->control.back().x;
    x4=it1->end.x;
    y4=it1->end.y;
    calculate_line_line_intersection(it,it1,x1,y1,x2,y2,x3,y3,x4,y4);

    //4.it endline with it1 startline
    //line1
    x1=it->control.back().x;
    y1=it->control.back().y;
    x2=it->end.x;
    y2=it->end.y;
    //line2
    x3=it1->start.x;
    y3=it1->start.y;
    x4=it1->control.front().x;
    y4=it1->control.front().y;
    calculate_line_line_intersection(it,it1,x1,y1,x2,y2,x3,y3,x4,y4);

    //5.it endline with it1 controllines
    for(it2=it1->control.begin(); it2<it1->control.end()-1; it2++){
        it3=it2+1;
        //line1
        double x1=it->control.back().x;
        double y1=it->control.back().y;
        double x2=it->end.x;
        double y2=it->end.y;
        //line2
        double x3=it2->x;
        double y3=it2->y;
        double x4=it3->x;
        double y4=it3->y;
        calculate_line_line_intersection(it,it1,x1,y1,x2,y2,x3,y3,x4,y4);
    }

    //6.it endline with it1 endline
    //line1
    x1=it->control.back().x;
    y1=it->control.back().y;
    x2=it->end.x;
    y2=it->end.y;
    //line2
    x3=it1->control.back().x;
    y3=it1->control.back().y;
    x4=it1->end.x;
    y4=it1->end.y;
    calculate_line_line_intersection(it,it1,x1,y1,x2,y2,x3,y3,x4,y4);

    //7.it controllines to it1 controllines
    for(it2=it->control.begin(); it2<it->control.end()-1; it2++){
        it3=it2+1;
        for(it4=it1->control.begin(); it4<it1->control.end()-1; it4++){
            it5=it4+1;
            if(it2!=it4 /*&& it!=it4 && it1!=it3*/){ //avoid comparing the same line
                //line1
                double x1=it2->x;
                double y1=it2->y;
                double x2=it3->x;
                double y2=it3->y;
                //line2
                double x3=it4->x;
                double y3=it4->y;
                double x4=it5->x;
                double y4=it5->y;
                calculate_line_line_intersection(it,it1,x1,y1,x2,y2,x3,y3,x4,y4);
            }
        }
    }
}

void arc_arc_intersect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1)
{
    // Find the distance between the centers.
    double dx = it->center.x - it1->center.x;
    double dy = it->center.y - it1->center.y;
    double dist = sqrt(dx * dx + dy * dy);
    double ix;
    double iy;
    double ix1;
    double ix2;

    // See how many solutions there are.
    if (dist > it->radius + it1->radius)
    {
        //mo solution
    }
    else if (dist < abs(it->radius - it1->radius))
    {
        //no solutions, one circle contains the other
    }
    else if ((dist == 0) && (it->radius == it1->radius))
    {
        //no solutions, the circles coincide
    }
    else
    {
        // Find a and h.
        double a = (it->radius * it->radius -
                    it1->radius * it1->radius + dist * dist) / (2 * dist);
        double h = sqrt(it->radius * it->radius - a * a);

        // Find P2.
        double cx2 = it->center.x + a * (it1->center.x - it->center.x) / dist;
        double cy2 = it->center.y + a * (it1->center.y - it->center.y) / dist;

        if(dist<(it->radius+it1->radius+0.01) && dist>(it->radius+it1->radius-0.01)){ // 1 solution
            // Get the points P3.
            ix = ((cx2 + h * (it1->center.y - it->center.y) / dist));
            iy = ((cy2 - h * (it1->center.x - it->center.x) / dist));
            bool check=check_if_point_is_on_arc(it,{ix,iy,0});
            bool check1=check_if_point_is_on_arc(it1,{ix,iy,0});
            if(check && check1){
                //check if points are next or previous attached line start,endpoint
                bool check=true;
                if((it1->end.x<it->start.x+0.01 && it1->end.x>it->start.x-0.01 && it1->end.y<it->start.y+0.01 && it1->end.y>it->start.y-0.01 ) ||
                        (it1->start.x<it->end.x+0.01 && it1->start.x>it->end.x-0.01 && it1->start.y<it->end.y+0.01 &&  it1->start.y>it->end.y-0.01)){
                    check=false;
                }
                if(check){
                    it->intersect.push_back({ix,iy,0});
                    it1->intersect.push_back({ix,iy,0});
                    //std::cout<<"arc_arc_intersect x: "<<ix<<" y: " << iy<<std::endl;
                }
            }

        } else { // 2 solutions

            // Get the points P3.
            double ix = ((cx2 + h * (it1->center.y - it->center.y) / dist));
            double iy = ((cy2 - h * (it1->center.x - it->center.x) / dist));
            double ix1 = (cx2 - h * (it1->center.y - it->center.y) / dist);
            double iy1 = ((cy2 + h * (it1->center.x - it->center.x) / dist));

            bool check0=check_if_point_is_on_arc(it,{ix,iy,0});
            bool check1=check_if_point_is_on_arc(it1,{ix,iy,0});
            if(check0 && check1){
                //check if points are next or previous attached line start,endpoint
                bool check=true;
                if((it1->end.x<it->start.x+0.01 && it1->end.x>it->start.x-0.01 && it1->end.y<it->start.y+0.01 && it1->end.y>it->start.y-0.01 ) ||
                        (it1->start.x<it->end.x+0.01 && it1->start.x>it->end.x-0.01 && it1->start.y<it->end.y+0.01 &&  it1->start.y>it->end.y-0.01)){
                    check=false;
                }
                if(check){
                    it->intersect.push_back({ix,iy,0});
                    it1->intersect.push_back({ix,iy,0});
                    //std::cout<<"arc_arc_intersect x: "<<ix<<" y: " << iy<<std::endl;
                }
            }

            bool check2=check_if_point_is_on_arc(it,{ix1,iy1,0});
            bool check3=check_if_point_is_on_arc(it1,{ix1,iy1,0});
            if(check2 && check3){
                //check if points are next or previous attached line start,endpoint
                bool check=true;
                if((it1->end.x<it->start.x+0.01 && it1->end.x>it->start.x-0.01 && it1->end.y<it->start.y+0.01 && it1->end.y>it->start.y-0.01 ) ||
                        (it1->start.x<it->end.x+0.01 && it1->start.x>it->end.x-0.01 && it1->start.y<it->end.y+0.01 &&  it1->start.y>it->end.y-0.01)){
                    check=false;
                }
                if(check){
                    it->intersect.push_back({ix1,iy1,0});
                    it1->intersect.push_back({ix1,iy1,0});
                    //std::cout<<"arc_arc_intersect x: "<<ix1<<" y: " << iy1<<std::endl;
                }
            }
        }
    }
}

void linestrip_self_intersect(std::vector<OBJECT>::iterator it){

    OBJECT obj=*it;
    double x1=0,y1=0,x2=0,y2=0,x3=0,y3=0,x4=0,y4=0;

    for(unsigned int i=0; i<obj.control.size()-1; i++){
        x1=obj.control.at(i).x;
        y1=obj.control.at(i).y;
        x2=obj.control.at(i+1).x;
        y2=obj.control.at(i+1).y;

        for(unsigned int j=i+2; j<obj.control.size()-1; j++){
            x3=obj.control.at(j).x;
            y3=obj.control.at(j).y;
            x4=obj.control.at(j+1).x;
            y4=obj.control.at(j+1).y;

            //find intersect
            //line 1 to compare :
            double startpoint_x0 = x1;
            double startpoint_y0 = y1;
            double endpoint_x0 = x2;
            double endpoint_y0 = y2;

            //line 2 to compare :
            double startpoint_x1 = x3;
            double startpoint_y1 = y3;
            double endpoint_x1 = x4;
            double endpoint_y1 = y4;

            //line 1
            double delta_y0 = endpoint_y0 - startpoint_y0;
            double delta_x0 = startpoint_x0 - endpoint_x0;
            double c0 = delta_y0 * startpoint_x0 + delta_x0 * startpoint_y0;

            //line 2
            double delta_y1 = endpoint_y1 - startpoint_y1;
            double delta_x1 = startpoint_x1 - endpoint_x1;
            double c1 = delta_y1 * startpoint_x1 + delta_x1 * startpoint_y1;

            double determinant = delta_y0*delta_x1 - delta_y1*delta_x0;
            //qDebug() << "determinant is : " << determinant;
            if (determinant == 0) // The lines are parallel.
            {
                std::cout<< "the lines are parallel"<<std::endl;
            }
            else  {
                double ix = (delta_x1*c0 - delta_x0*c1)/determinant;
                double iy = (delta_y0*c1 - delta_y1*c0)/determinant;
                std::cout<<"ix: "<<ix<<" iy: "<<iy<<std::endl;

                bool check0=false,check1=false;
                //check if intersection xy is on first segment
                double l0=sqrt(pow(x1-ix,2)+pow(y1-iy,2));
                double l1=sqrt(pow(x2-ix,2)+pow(y2-iy,2));
                double l2=sqrt(pow(x1-x2,2)+pow(y1-y2,2));
                if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
                    check0=true;
                }
                //check if intersection xy is on second segment
                double l3=sqrt(pow(x3-ix,2)+pow(y3-iy,2));
                double l4=sqrt(pow(x4-ix,2)+pow(y4-iy,2));
                double l5=sqrt(pow(x3-x4,2)+pow(y3-y4,2));
                if(l5>(l3+l4)-0.01 && l5<(l3+l4)+0.01){
                    check1=true;
                }

                if(check0 && check1){
                    it->intersect.push_back({ix,iy,0});
                }
            }
        }
    }
}

#endif


































