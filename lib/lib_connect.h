#ifndef LIB_CONNECT_H
#define LIB_CONNECT_H
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
#include <lib/lib_intersect.h>

//void calculate_line_line_connect(std::vector<OBJECT>::iterator it,std::vector<OBJECT>::iterator it1, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
void update_arc_controlpoint(std::vector<OBJECT>::iterator it);
bool point_is_point(POINT a, POINT b, double res);
bool val_is_val(double a, double b, double res);

std::vector<OBJECT> linestrip_remove_small_segments(std::vector<OBJECT> array){

    std::vector<OBJECT>::iterator it;
    std::vector<POINT>::iterator it1, it2;
    std::vector<POINT> p;

    for(it=array.begin(); it<array.end(); it++){
        if(it->type=="linestrip"){

            it1=it->control.begin();
            for(it2=it->control.begin()+1; it2<it->control.end(); it2++){
                double l=sqrt(pow(it1->x-it2->x,2)+pow(it1->y-it2->y,2));
                if(l>0.1){ //when lenght reaches the value, write points
                    p.push_back(*it2);
                    it1=it2;
                }
            }

            //int size_before=it->control.size();
            //int size_after=p.size();
            //std::cout<<"for a spline, the deleted spline points : "<<size_before-size_after<<std::endl;

            it->control.clear();
            it->control.reserve(p.size()+2);
            it->control=p; //replace with filtered controlpoints
            p.clear();
        }
    }
    return array;
}

void line_line_connect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1)
{
    //line 1 to compare :
    double startpoint_x0 = it->start.x;
    double startpoint_y0 = it->start.y;
    double endpoint_x0 = it->end.x;
    double endpoint_y0 = it->end.y;

    //line 2 to compare :
    double startpoint_x1 = it1->start.x;
    double startpoint_y1 = it1->start.y;
    double endpoint_x1 = it1->end.x;
    double endpoint_y1 = it1->end.y;

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

    if (determinant == 0){
        //std::cout<< "the lines are parallel"<<std::endl;
    } else {
        double x = (delta_x1*c0 - delta_x0*c1)/determinant;
        double y = (delta_y0*c1 - delta_y1*c0)/determinant;
        //std::cout<< " intersection point is x : " << x << " y : " << y<<std::endl;

        it->end.x = x;
        it->end.y = y;
        it1->start.x = x;
        it1->start.y = y;
    }
}

void calculate_line_line_connect(std::vector<OBJECT>::iterator it,std::vector<OBJECT>::iterator it1, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4){

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
        //std::cout<< "the lines are parallel"<<std::endl;
        //connect with line
    }
    else  {

        double x = (delta_x1*c0 - delta_x0*c1)/determinant;
        double y = (delta_y0*c1 - delta_y1*c0)/determinant;
        //std::cout<< " intersection point is x : " << x << " y : " << y<<std::endl;

        it->end.x = x;
        it->end.y = y;
        it1->start.x = x;
        it1->start.y = y;

    }
}

void linestrip_arc_connect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1){

    bool trigger=false;
    //arc to linestrip front segment
    double start_line_x1 = it->start.x;
    double start_line_y1 = it->start.y;
    double end_line_x2 = it->control.front().x;
    double end_line_y2 = it->control.front().y;
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
    } else if (det==0){ //one solution
        t = -B / (2 * A);
        ix = (start_line_x1 + t * dx);
        iy = (start_line_y1 + t * dy);

        //check if intersection is on the line segment and inside the arc section
        double l0=sqrt(pow(it->start.x-ix,2)+pow(it->start.y-iy,2));
        double l1=sqrt(pow(it->control.front().x-ix,2)+pow(it->control.front().y-iy,2));
        double ltot=sqrt(pow(it->start.x-it->control.front().x,2)+pow(it->start.y-it->control.front().y,2));
        if(ltot>(l0+l1)-0.01 && ltot<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
            if(check){
                it->end.x=ix;
                it->end.y=iy;
                it->control.clear();
                it->type="line";
                it1->start.x=ix;
                it1->start.y=iy;
                update_arc_controlpoint(it1);
                trigger=true;
                //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
            }
        }
    } else { //two solutions
        t = (-B + sqrt(det)) / (2 * A);
        ix = (start_line_x1 + t * dx);
        iy = (start_line_y1 + t * dy);

        t = (-B - sqrt(det)) / (2 * A);
        ix1 = (start_line_x1 + t * dx);
        iy1 = (start_line_y1 + t * dy);

        //check if intersection is on the line segment and inside the arc section
        double l0=sqrt(pow(it->start.x-ix,2)+pow(it->start.y-iy,2));
        double l1=sqrt(pow(it->control.front().x-ix,2)+pow(it->control.front().y-iy,2));
        double l2=sqrt(pow(it->start.x-it->control.front().x,2)+pow(it->start.y-it->control.front().y,2));
        if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
            if(check){
                it->end.x=ix;
                it->end.y=iy;
                it->control.clear();
                it->type="line";
                it1->start.x=ix;
                it1->start.y=iy;
                update_arc_controlpoint(it1);
                trigger=true;
                //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
            }
        }

        double l3=sqrt(pow(it->start.x-ix1,2)+pow(it->start.y-iy1,2));
        double l4=sqrt(pow(it->control.front().x-ix1,2)+pow(it->control.front().y-iy1,2));
        double l5=sqrt(pow(it->start.x-it->control.front().x,2)+pow(it->start.y-it->control.front().y,2));
        if(l5>(l3+l4)-0.01 && l5<(l3+l4)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix1,iy1,0});
            if(check){
                it->end.x=ix1;
                it->end.y=iy1;
                it->control.clear();
                it->type="line";
                it1->start.x=ix1;
                it1->start.y=iy1;
                update_arc_controlpoint(it1);
                trigger=true;
                //std::cout<<"arc_line_intersect x: "<<ix1<<" y: " << iy1<<std::endl;
            }
        }
    }

    //arc to linestrip controlpoints segments
    if(!trigger){
        std::vector<POINT>::iterator it2,it3;
        for(it2=it->control.begin(); it2<it->control.end()-1; it2++){
            it3=it2+1;
            start_line_x1 = it2->x;
            start_line_y1 = it2->y;
            end_line_x2 = it3->x;
            end_line_y2 = it3->y;
            circle_center_x = it1->center.x;
            circle_center_y = it1->center.y;
            radius = it1->radius;
            ix=0;
            iy=0;
            ix1=0;
            iy1=0;
            dx = end_line_x2 - start_line_x1;
            dy = end_line_y2 - start_line_y1;
            A = dx * dx + dy * dy;
            B = 2 * (dx * (start_line_x1 - circle_center_x) + dy * (start_line_y1 - circle_center_y));
            C = (start_line_x1 - circle_center_x) * (start_line_x1 - circle_center_x) + (start_line_y1 - circle_center_y) * (start_line_y1 - circle_center_y) - radius * radius;
            det = B * B - 4 * A * C;
            if ((A <= 0.0000001) || (det < 0)){
                //no solutions
            } else if (det==0){ //one solution
                t = -B / (2 * A);
                ix = (start_line_x1 + t * dx);
                iy = (start_line_y1 + t * dy);

                //check if intersection is on the line segment and inside the arc section
                double l0=sqrt(pow(it2->x-ix,2)+pow(it2->y-iy,2));
                double l1=sqrt(pow(it3->x-ix,2)+pow(it3->y-iy,2));
                double ltot=sqrt(pow(it2->x-it3->x,2)+pow(it2->y-it3->y,2));
                if(ltot>(l0+l1)-0.01 && ltot<(l0+l1)+0.01){
                    bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
                    if(check){

                        it->end.x=ix;
                        it->end.y=iy;
                        it->control.erase(it2,it->control.end());
                        it1->start.x=ix;
                        it1->start.y=iy;
                        update_arc_controlpoint(it1);
                        trigger=true;
                        //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
                        break;
                    }
                }
            } else { //two solutions
                t = (-B + sqrt(det)) / (2 * A);
                ix = (start_line_x1 + t * dx);
                iy = (start_line_y1 + t * dy);

                t = (-B - sqrt(det)) / (2 * A);
                ix1 = (start_line_x1 + t * dx);
                iy1 = (start_line_y1 + t * dy);

                //check if intersection is on the line segment and inside the arc section
                double l0=sqrt(pow(it2->x-ix,2)+pow(it2->y-iy,2));
                double l1=sqrt(pow(it3->x-ix,2)+pow(it3->y-iy,2));
                double l2=sqrt(pow(it2->x-it3->x,2)+pow(it2->y-it3->y,2));
                if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
                    bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
                    if(check){
                        it->end.x=ix;
                        it->end.y=iy;
                        it->control.erase(it2,it->control.end());
                        it1->start.x=ix;
                        it1->start.y=iy;
                        update_arc_controlpoint(it1);
                        trigger=true;
                        //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
                        break;
                    }
                }

                double l3=sqrt(pow(it2->x-ix1,2)+pow(it2->y-iy1,2));
                double l4=sqrt(pow(it3->x-ix1,2)+pow(it3->y-iy1,2));
                double l5=sqrt(pow(it2->x-it3->x,2)+pow(it2->y-it3->y,2));
                if(l5>(l3+l4)-0.01 && l5<(l3+l4)+0.01){
                    bool check=check_if_point_is_on_arc(it1,{ix1,iy1,0});
                    if(check){
                        it->end.x=ix1;
                        it->end.y=iy1;
                        it->control.erase(it2,it->control.end());
                        it1->start.x=ix1;
                        it1->start.y=iy1;
                        update_arc_controlpoint(it1);
                        trigger=true;
                        //std::cout<<"arc_line_intersect x: "<<ix1<<" y: " << iy1<<std::endl;
                        break;
                    }
                }
            }
        }
    }


    //arc to linestrip end segment
    if(!trigger){
        start_line_x1 = it->control.back().x;
        start_line_y1 = it->control.back().y;
        end_line_x2 = it->end.x;
        end_line_y2 = it->end.y;
        circle_center_x = it1->center.x;
        circle_center_y = it1->center.y;
        radius = it1->radius;
        ix=0;
        iy=0;
        ix1=0;
        iy1=0;
        dx = end_line_x2 - start_line_x1;
        dy = end_line_y2 - start_line_y1;
        A = dx * dx + dy * dy;
        B = 2 * (dx * (start_line_x1 - circle_center_x) + dy * (start_line_y1 - circle_center_y));
        C = (start_line_x1 - circle_center_x) * (start_line_x1 - circle_center_x) + (start_line_y1 - circle_center_y) * (start_line_y1 - circle_center_y) - radius * radius;
        det = B * B - 4 * A * C;
        if ((A <= 0.0000001) || (det < 0)){
            //no solutions
        } else if (det==0){ //one solution
            t = -B / (2 * A);
            ix = (start_line_x1 + t * dx);
            iy = (start_line_y1 + t * dy);

            //check if intersection is on the line segment and inside the arc section
            double l0=sqrt(pow(it->control.back().x-ix,2)+pow(it->control.back().y-iy,2));
            double l1=sqrt(pow(it->end.x-ix,2)+pow(it->end.y-iy,2));
            double ltot=sqrt(pow(it->control.back().x-it->end.x,2)+pow(it->control.back().y-it->end.y,2));
            if(ltot>(l0+l1)-0.01 && ltot<(l0+l1)+0.01){
                bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
                if(check){
                    it->end.x=ix;
                    it->end.y=iy;
                    it->control.pop_back();
                    it1->start.x=ix;
                    it1->start.y=iy;
                    update_arc_controlpoint(it1);
                    trigger=true;
                    std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
                }
            }
        } else { //two solutions
            t = (-B + sqrt(det)) / (2 * A);
            ix = (start_line_x1 + t * dx);
            iy = (start_line_y1 + t * dy);

            t = (-B - sqrt(det)) / (2 * A);
            ix1 = (start_line_x1 + t * dx);
            iy1 = (start_line_y1 + t * dy);

            //check if intersection is on the line segment and inside the arc section
            double l0=sqrt(pow(it->control.back().x-ix,2)+pow(it->control.back().y-iy,2));
            double l1=sqrt(pow(it->end.x-ix,2)+pow(it->end.y-iy,2));
            double l2=sqrt(pow(it->control.back().x-it->end.x,2)+pow(it->control.back().y-it->end.y,2));
            if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
                bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
                if(check){
                    it->end.x=ix;
                    it->end.y=iy;
                    it->control.pop_back();
                    it1->start.x=ix;
                    it1->start.y=iy;
                    update_arc_controlpoint(it1);
                    trigger=true;
                    std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
                }
            }

            double l3=sqrt(pow(it->control.back().x-ix1,2)+pow(it->control.back().y-iy1,2));
            double l4=sqrt(pow(it->end.x-ix1,2)+pow(it->end.y-iy1,2));
            double l5=sqrt(pow(it->control.back().x-it->end.x,2)+pow(it->control.back().y-it->end.y,2));
            if(l5>(l3+l4)-0.01 && l5<(l3+l4)+0.01){
                bool check=check_if_point_is_on_arc(it1,{ix1,iy1,0});
                if(check){
                    it->end.x=ix1;
                    it->end.y=iy1;
                    it->control.pop_back();
                    it1->start.x=ix1;
                    it1->start.y=iy1;
                    update_arc_controlpoint(it1);
                    trigger=true;
                    std::cout<<"arc_line_intersect x: "<<ix1<<" y: " << iy1<<std::endl;
                }
            }
        }
    }
}

void arc_linestrip_connect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1)
{
    bool trigger=false;
    //arc to linestrip front segment
    double start_line_x1 = it1->start.x;
    double start_line_y1 = it1->start.y;
    double end_line_x2 = it1->control.front().x;
    double end_line_y2 = it1->control.front().y;
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
    } else if (det==0){ //one solution
        t = -B / (2 * A);
        ix = (start_line_x1 + t * dx);
        iy = (start_line_y1 + t * dy);

        //check if intersection is on the line segment and inside the arc section
        double l0=sqrt(pow(it1->start.x-ix,2)+pow(it1->start.y-iy,2));
        double l1=sqrt(pow(it1->control.front().x-ix,2)+pow(it1->control.front().y-iy,2));
        double ltot=sqrt(pow(it1->start.x-it1->control.front().x,2)+pow(it1->start.y-it1->control.front().y,2));
        if(ltot>(l0+l1)-0.01 && ltot<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it,{ix,iy,0});
            if(check){
                it->end.x=ix;
                it->end.y=iy;
                it1->start.x=ix;
                it1->start.y=iy;
                it1->control.erase(it->control.begin());
                update_arc_controlpoint(it);
                trigger=true;
                //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
            }
        }
    } else { //two solutions
        t = (-B + sqrt(det)) / (2 * A);
        ix = (start_line_x1 + t * dx);
        iy = (start_line_y1 + t * dy);

        t = (-B - sqrt(det)) / (2 * A);
        ix1 = (start_line_x1 + t * dx);
        iy1 = (start_line_y1 + t * dy);

        //check if intersection is on the line segment and inside the arc section
        double l0=sqrt(pow(it1->start.x-ix,2)+pow(it1->start.y-iy,2));
        double l1=sqrt(pow(it1->control.front().x-ix,2)+pow(it1->control.front().y-iy,2));
        double l2=sqrt(pow(it1->start.x-it1->control.front().x,2)+pow(it1->start.y-it1->control.front().y,2));
        if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it,{ix,iy,0});
            if(check){
                it->end.x=ix;
                it->end.y=iy;
                it1->start.x=ix;
                it1->start.y=iy;
                it1->control.erase(it->control.begin());
                update_arc_controlpoint(it);
                trigger=true;
                //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
            }
        }

        double l3=sqrt(pow(it1->start.x-ix1,2)+pow(it1->start.y-iy1,2));
        double l4=sqrt(pow(it1->control.front().x-ix1,2)+pow(it1->control.front().y-iy1,2));
        double l5=sqrt(pow(it1->start.x-it1->control.front().x,2)+pow(it1->start.y-it1->control.front().y,2));
        if(l5>(l3+l4)-0.01 && l5<(l3+l4)+0.01){
            bool check=check_if_point_is_on_arc(it,{ix1,iy1,0});
            if(check){
                it->end.x=ix1;
                it->end.y=iy1;
                it1->start.x=ix1;
                it1->start.y=iy1;
                it1->control.erase(it->control.begin());
                update_arc_controlpoint(it);
                trigger=true;
                //std::cout<<"arc_line_intersect x: "<<ix1<<" y: " << iy1<<std::endl;
            }
        }
    }

    //arc to linestrip controlpoints segments
    if(!trigger){
        std::vector<POINT>::iterator it2,it3;
        for(it2=it1->control.begin(); it2<it1->control.end()-1; it2++){
            it3=it2+1;
            start_line_x1 = it2->x;
            start_line_y1 = it2->y;
            end_line_x2 = it3->x;
            end_line_y2 = it3->y;
            circle_center_x = it->center.x;
            circle_center_y = it->center.y;
            radius = it->radius;
            ix=0;
            iy=0;
            ix1=0;
            iy1=0;
            dx = end_line_x2 - start_line_x1;
            dy = end_line_y2 - start_line_y1;
            A = dx * dx + dy * dy;
            B = 2 * (dx * (start_line_x1 - circle_center_x) + dy * (start_line_y1 - circle_center_y));
            C = (start_line_x1 - circle_center_x) * (start_line_x1 - circle_center_x) + (start_line_y1 - circle_center_y) * (start_line_y1 - circle_center_y) - radius * radius;
            det = B * B - 4 * A * C;
            if ((A <= 0.0000001) || (det < 0)){
                //no solutions
            } else if (det==0){ //one solution
                t = -B / (2 * A);
                ix = (start_line_x1 + t * dx);
                iy = (start_line_y1 + t * dy);

                //check if intersection is on the line segment and inside the arc section
                double l0=sqrt(pow(it2->x-ix,2)+pow(it2->y-iy,2));
                double l1=sqrt(pow(it3->x-ix,2)+pow(it3->y-iy,2));
                double ltot=sqrt(pow(it2->x-it3->x,2)+pow(it2->y-it3->y,2));
                if(ltot>(l0+l1)-0.01 && ltot<(l0+l1)+0.01){
                    bool check=check_if_point_is_on_arc(it,{ix,iy,0});
                    if(check){
                        it->end.x=ix;
                        it->end.y=iy;
                        it1->start.x=ix;
                        it1->start.y=iy;
                        it1->control.erase(it1->control.begin(),it2+1);
                        update_arc_controlpoint(it);
                        trigger=true;
                        //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
                        break;
                    }
                }
            } else { //two solutions
                t = (-B + sqrt(det)) / (2 * A);
                ix = (start_line_x1 + t * dx);
                iy = (start_line_y1 + t * dy);

                t = (-B - sqrt(det)) / (2 * A);
                ix1 = (start_line_x1 + t * dx);
                iy1 = (start_line_y1 + t * dy);

                //check if intersection is on the line segment and inside the arc section
                double l0=sqrt(pow(it2->x-ix,2)+pow(it2->y-iy,2));
                double l1=sqrt(pow(it3->x-ix,2)+pow(it3->y-iy,2));
                double l2=sqrt(pow(it2->x-it3->x,2)+pow(it2->y-it3->y,2));
                if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
                    bool check=check_if_point_is_on_arc(it,{ix,iy,0});
                    if(check){
                        it->end.x=ix;
                        it->end.y=iy;
                        it1->start.x=ix;
                        it1->start.y=iy;
                        it1->control.erase(it1->control.begin(),it2+1);
                        update_arc_controlpoint(it);
                        trigger=true;
                        //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
                        break;
                    }
                }

                double l3=sqrt(pow(it2->x-ix1,2)+pow(it2->y-iy1,2));
                double l4=sqrt(pow(it3->x-ix1,2)+pow(it3->y-iy1,2));
                double l5=sqrt(pow(it2->x-it3->x,2)+pow(it2->y-it3->y,2));
                if(l5>(l3+l4)-0.01 && l5<(l3+l4)+0.01){
                    bool check=check_if_point_is_on_arc(it,{ix1,iy1,0});
                    if(check){
                        it->end.x=ix1;
                        it->end.y=iy1;
                        it1->start.x=ix1;
                        it1->start.y=iy1;
                        it1->control.erase(it1->control.begin(),it2+1);
                        update_arc_controlpoint(it);
                        trigger=true;
                        //std::cout<<"arc_line_intersect x: "<<ix1<<" y: " << iy1<<std::endl;
                        break;
                    }
                }
            }
        }
    }


    //arc to linestrip end segment
    if(!trigger){
        start_line_x1 = it1->control.back().x;
        start_line_y1 = it1->control.back().y;
        end_line_x2 = it1->end.x;
        end_line_y2 = it1->end.y;
        circle_center_x = it->center.x;
        circle_center_y = it->center.y;
        radius = it->radius;
        ix=0;
        iy=0;
        ix1=0;
        iy1=0;
        dx = end_line_x2 - start_line_x1;
        dy = end_line_y2 - start_line_y1;
        A = dx * dx + dy * dy;
        B = 2 * (dx * (start_line_x1 - circle_center_x) + dy * (start_line_y1 - circle_center_y));
        C = (start_line_x1 - circle_center_x) * (start_line_x1 - circle_center_x) + (start_line_y1 - circle_center_y) * (start_line_y1 - circle_center_y) - radius * radius;
        det = B * B - 4 * A * C;
        if ((A <= 0.0000001) || (det < 0)){
            //no solutions
        } else if (det==0){ //one solution
            t = -B / (2 * A);
            ix = (start_line_x1 + t * dx);
            iy = (start_line_y1 + t * dy);

            //check if intersection is on the line segment and inside the arc section
            double l0=sqrt(pow(it1->control.back().x-ix,2)+pow(it1->control.back().y-iy,2));
            double l1=sqrt(pow(it1->end.x-ix,2)+pow(it1->end.y-iy,2));
            double ltot=sqrt(pow(it1->control.back().x-it1->end.x,2)+pow(it1->control.back().y-it1->end.y,2));
            if(ltot>(l0+l1)-0.01 && ltot<(l0+l1)+0.01){
                bool check=check_if_point_is_on_arc(it,{ix,iy,0});
                if(check){
                    it->end.x=ix;
                    it->end.y=iy;
                    it1->start.x=ix;
                    it1->start.y=iy;
                    it1->control.clear();
                    it1->type="line";
                    update_arc_controlpoint(it);
                    trigger=true;
                    //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
                }
            }
        } else { //two solutions
            t = (-B + sqrt(det)) / (2 * A);
            ix = (start_line_x1 + t * dx);
            iy = (start_line_y1 + t * dy);

            t = (-B - sqrt(det)) / (2 * A);
            ix1 = (start_line_x1 + t * dx);
            iy1 = (start_line_y1 + t * dy);

            //check if intersection is on the line segment and inside the arc section
            double l0=sqrt(pow(it1->control.back().x-ix,2)+pow(it1->control.back().y-iy,2));
            double l1=sqrt(pow(it1->end.x-ix,2)+pow(it1->end.y-iy,2));
            double l2=sqrt(pow(it1->control.back().x-it1->end.x,2)+pow(it1->control.back().y-it1->end.y,2));
            if(l2>(l0+l1)-0.01 && l2<(l0+l1)+0.01){
                bool check=check_if_point_is_on_arc(it,{ix,iy,0});
                if(check){
                    it->end.x=ix;
                    it->end.y=iy;
                    it1->start.x=ix;
                    it1->start.y=iy;
                    it1->control.clear();
                    it1->type="line";
                    update_arc_controlpoint(it);
                    trigger=true;
                    //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
                }
            }

            double l3=sqrt(pow(it1->control.back().x-ix1,2)+pow(it1->control.back().y-iy1,2));
            double l4=sqrt(pow(it1->end.x-ix1,2)+pow(it1->end.y-iy1,2));
            double l5=sqrt(pow(it1->control.back().x-it1->end.x,2)+pow(it1->control.back().y-it1->end.y,2));
            if(l5>(l3+l4)-0.01 && l5<(l3+l4)+0.01){
                bool check=check_if_point_is_on_arc(it,{ix1,iy1,0});
                if(check){
                    it->end.x=ix1;
                    it->end.y=iy1;
                    it1->start.x=ix1;
                    it1->start.y=iy1;
                    it1->control.clear();
                    it1->type="line";
                    update_arc_controlpoint(it);
                    trigger=true;
                    //std::cout<<"arc_line_intersect x: "<<ix1<<" y: " << iy1<<std::endl;
                }
            }
        }
    }
}

void line_arc_connect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1) //http://csharphelper.com/blog/2014/09/determine-where-a-line-intersects-a-circle-in-c/
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
    } else if (det==0){ //one solution
        t = -B / (2 * A);
        ix = (start_line_x1 + t * dx);
        iy = (start_line_y1 + t * dy);

        //check if intersection is on the line segment and inside the arc section
        double l0=sqrt(pow(it->start.x-ix,2)+pow(it->start.y-iy,2));
        double l1=sqrt(pow(it->end.x-ix,2)+pow(it->end.y-iy,2));
        double ltot=sqrt(pow(it->start.x-it->end.x,2)+pow(it->start.y-it->end.y,2));
        if(ltot>(l0+l1)-0.01 && ltot<(l0+l1)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix,iy,0});
            if(check){
                it->end.x=ix;
                it->end.y=iy;
                it1->start.x=ix;
                it1->start.y=iy;
                update_arc_controlpoint(it1);
                //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
            }
        }
    } else { //two solutions
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
            if(check){
                it->end.x=ix;
                it->end.y=iy;
                it1->start.x=ix;
                it1->start.y=iy;
                update_arc_controlpoint(it1);
                //std::cout<<"arc_line_intersect x: "<<ix<<" y: " << iy<<std::endl;
            }
        }

        double l2=sqrt(pow(it->start.x-ix1,2)+pow(it->start.y-iy1,2));
        double l3=sqrt(pow(it->end.x-ix1,2)+pow(it->end.y-iy1,2));
        double ltot1=sqrt(pow(it->start.x-it->end.x,2)+pow(it->start.y-it->end.y,2));
        if(ltot1>(l2+l3)-0.01 && ltot1<(l2+l3)+0.01){
            bool check=check_if_point_is_on_arc(it1,{ix1,iy1,0});
            if(check){
                it->end.x=ix1;
                it->end.y=iy1;
                it1->start.x=ix1;
                it1->start.y=iy1;
                update_arc_controlpoint(it1);
                //std::cout<<"arc_line_intersect x: "<<ix1<<" y: " << iy1<<std::endl;
            }
        }
    }
}

void line_linestrip_connect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1)
{
    //1.find intersection
    bool trigger=false;
    //linestrip start segment to compare
    double startpoint_x0 = it1->start.x;
    double startpoint_y0 = it1->start.y;
    double endpoint_x0 = it1->control.front().x;
    double endpoint_y0 = it1->control.front().y;

    //line 2 to compare :
    double startpoint_x1 = it->start.x;
    double startpoint_y1 = it->start.y;
    double endpoint_x1 = it->end.x;
    double endpoint_y1 = it->end.y;

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

    if(determinant == 0){
        //std::cout<< "the lines are parallel"<<std::endl;
    } else {
        double x = (delta_x1*c0 - delta_x0*c1)/determinant;
        double y = (delta_y0*c1 - delta_y1*c0)/determinant;
        //std::cout<< " intersection point is x : " << x << " y : " << y<<std::endl;

        //check if x,y is on linestrip segment
        double l0=sqrt(pow(it1->start.x-x,2)+pow(it1->start.y-y,2));
        double l1=sqrt(pow(it1->control.front().x-x,2)+pow(it1->control.front().y-y,2));
        double l2=sqrt(pow(it1->start.x-it1->control.front().x,2)+pow(it1->start.y-it1->control.front().y,2));
        bool hit0=val_is_val(l0+l1,l2,0.01);

        //check if x,y is on line segment
        double l3=sqrt(pow(it->start.x-x,2)+pow(it->start.y-y,2));
        double l4=sqrt(pow(it->end.x-x,2)+pow(it->end.y-y,2));
        double l5=sqrt(pow(it->start.x-it->end.x,2)+pow(it->start.y-it->end.y,2));
        bool hit1=val_is_val(l3+l4,l5,0.01);

        if(hit0 && hit1){ //connect segments
            it->end={x,y,0};
            it1->start={x,y,0};
            trigger=true;
        }
    }

    if(!trigger){
        //linestrip controlpoints segments to compare
        std::vector<POINT>::iterator it2,it3,it4;
        for(it2=it1->control.begin(); it2<it1->control.end()-1; it2++){
            it3=it2+1;
            //linestrip controlpoints
            startpoint_x0 = it2->x;
            startpoint_y0 = it2->y;
            endpoint_x0 = it3->x;
            endpoint_y0 = it3->y;

            //line 2 to compare :
            startpoint_x1 = it->start.x;
            startpoint_y1 = it->start.y;
            endpoint_x1 = it->end.x;
            endpoint_y1 = it->end.y;

            //line 1
            delta_y0 = endpoint_y0 - startpoint_y0;
            delta_x0 = startpoint_x0 - endpoint_x0;
            c0 = delta_y0 * startpoint_x0 + delta_x0 * startpoint_y0;

            //line 2
            delta_y1 = endpoint_y1 - startpoint_y1;
            delta_x1 = startpoint_x1 - endpoint_x1;
            c1 = delta_y1 * startpoint_x1 + delta_x1 * startpoint_y1;

            determinant = delta_y0*delta_x1 - delta_y1*delta_x0;
            //qDebug() << "determinant is : " << determinant;

            if(determinant == 0){
                //std::cout<< "the lines are parallel"<<std::endl;
            } else {
                double x = (delta_x1*c0 - delta_x0*c1)/determinant;
                double y = (delta_y0*c1 - delta_y1*c0)/determinant;
                //std::cout<< " intersection point is x : " << x << " y : " << y<<std::endl;

                //check if x,y is on linestrip segment
                double l0=sqrt(pow(it2->x-x,2)+pow(it2->y-y,2));
                double l1=sqrt(pow(it3->x-x,2)+pow(it3->y-y,2));
                double l2=sqrt(pow(it2->x-it3->x,2)+pow(it2->y-it3->y,2));
                bool hit0=val_is_val(l0+l1,l2,0.01);

                //check if x,y is on line segment
                double l3=sqrt(pow(it->start.x-x,2)+pow(it->start.y-y,2));
                double l4=sqrt(pow(it->end.x-x,2)+pow(it->end.y-y,2));
                double l5=sqrt(pow(it->start.x-it->end.x,2)+pow(it->start.y-it->end.y,2));
                bool hit1=val_is_val(l3+l4,l5,0.01);

                if(hit0 && hit1){ //connect segments
                    it->end={x,y,0};
                    it1->start={x,y,0};
                    it1->control.erase(it1->control.begin(),it2+1);
                    trigger=true;
                    break;
                }
            }
        }
    }

    if(!trigger){
        //linestrip end segment to compare
        startpoint_x0 = it1->control.back().x;
        startpoint_y0 = it1->control.back().y;
        endpoint_x0 = it1->end.x;
        endpoint_y0 = it1->end.y;

        //line 2 to compare :
        startpoint_x1 = it->start.x;
        startpoint_y1 = it->start.y;
        endpoint_x1 = it->end.x;
        endpoint_y1 = it->end.y;

        //line 1
        delta_y0 = endpoint_y0 - startpoint_y0;
        delta_x0 = startpoint_x0 - endpoint_x0;
        c0 = delta_y0 * startpoint_x0 + delta_x0 * startpoint_y0;

        //line 2
        delta_y1 = endpoint_y1 - startpoint_y1;
        delta_x1 = startpoint_x1 - endpoint_x1;
        c1 = delta_y1 * startpoint_x1 + delta_x1 * startpoint_y1;

        determinant = delta_y0*delta_x1 - delta_y1*delta_x0;
        //qDebug() << "determinant is : " << determinant;

        if(determinant == 0){
            //std::cout<< "the lines are parallel"<<std::endl;
        } else {
            double x = (delta_x1*c0 - delta_x0*c1)/determinant;
            double y = (delta_y0*c1 - delta_y1*c0)/determinant;
            //std::cout<< " intersection point is x : " << x << " y : " << y<<std::endl;

            //check if x,y is on linestrip segment
            double l0=sqrt(pow(it1->control.back().x-x,2)+pow(it1->control.back().y-y,2));
            double l1=sqrt(pow(it1->end.x-x,2)+pow(it1->end.y-y,2));
            double l2=sqrt(pow(it1->control.back().x-it1->end.x,2)+pow(it1->control.back().y-it1->end.y,2));
            bool hit0=val_is_val(l0+l1,l2,0.01);

            //check if x,y is on line segment
            double l3=sqrt(pow(it->start.x-x,2)+pow(it->start.y-y,2));
            double l4=sqrt(pow(it->end.x-x,2)+pow(it->end.y-y,2));
            double l5=sqrt(pow(it->start.x-it->end.x,2)+pow(it->start.y-it->end.y,2));
            bool hit1=val_is_val(l3+l4,l5,0.01);

            if(hit0 && hit1){ //connect segments
                it->end={x,y,0};
                it1->start={x,y,0};
                it1->type="line";
                it1->control.clear();
            }
        }
    }
}

void linestrip_line_connect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1)
{
    //1.find intersection
    bool trigger=false; //if there is a intersection, skip some code

    //linestrip start segment to compare
    double startpoint_x0 = it->start.x;
    double startpoint_y0 = it->start.y;
    double endpoint_x0 = it->control.front().x;
    double endpoint_y0 = it->control.front().y;

    //line 2 to compare :
    double startpoint_x1 = it1->start.x;
    double startpoint_y1 = it1->start.y;
    double endpoint_x1 = it1->end.x;
    double endpoint_y1 = it1->end.y;

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

    if(determinant == 0){
        //std::cout<< "the lines are parallel"<<std::endl;
    } else {
        double x = (delta_x1*c0 - delta_x0*c1)/determinant;
        double y = (delta_y0*c1 - delta_y1*c0)/determinant;
        //std::cout<< " intersection point is x : " << x << " y : " << y<<std::endl;

        //check if x,y is on linestrip segment
        double l0=sqrt(pow(it->start.x-x,2)+pow(it->start.y-y,2));
        double l1=sqrt(pow(it->control.front().x-x,2)+pow(it->control.front().y-y,2));
        double l2=sqrt(pow(it->start.x-it->control.front().x,2)+pow(it->start.y-it->control.front().y,2));
        bool hit0=val_is_val(l0+l1,l2,0.01);

        //check if x,y is on line segment
        double l3=sqrt(pow(it1->start.x-x,2)+pow(it1->start.y-y,2));
        double l4=sqrt(pow(it1->end.x-x,2)+pow(it1->end.y-y,2));
        double l5=sqrt(pow(it1->start.x-it1->end.x,2)+pow(it1->start.y-it1->end.y,2));
        bool hit1=val_is_val(l3+l4,l5,0.01);

        if(hit0 && hit1){ //connect segments
            it->end={x,y,0};
            it->type="line";
            it->control.clear();
            it1->start={x,y,0};
            trigger=true;
        }
    }

    if(!trigger){
        //linestrip controlpoints segments to compare
        std::vector<POINT>::iterator it2,it3;
        for(it2=it->control.begin(); it2<it->control.end()-1; it2++){
            it3=it2+1;

            //linestrip controlpoints
            startpoint_x0 = it2->x;
            startpoint_y0 = it2->y;
            endpoint_x0 = it3->x;
            endpoint_y0 = it3->y;

            //line 2 to compare :
            startpoint_x1 = it1->start.x;
            startpoint_y1 = it1->start.y;
            endpoint_x1 = it1->end.x;
            endpoint_y1 = it1->end.y;

            //line 1
            delta_y0 = endpoint_y0 - startpoint_y0;
            delta_x0 = startpoint_x0 - endpoint_x0;
            c0 = delta_y0 * startpoint_x0 + delta_x0 * startpoint_y0;

            //line 2
            delta_y1 = endpoint_y1 - startpoint_y1;
            delta_x1 = startpoint_x1 - endpoint_x1;
            c1 = delta_y1 * startpoint_x1 + delta_x1 * startpoint_y1;

            determinant = delta_y0*delta_x1 - delta_y1*delta_x0;
            //qDebug() << "determinant is : " << determinant;

            if(determinant == 0){
                //std::cout<< "the lines are parallel"<<std::endl;
            } else {
                double x = (delta_x1*c0 - delta_x0*c1)/determinant;
                double y = (delta_y0*c1 - delta_y1*c0)/determinant;
                //std::cout<< " intersection point is x : " << x << " y : " << y<<std::endl;

                //check if x,y is on linestrip segment
                double l0=sqrt(pow(it2->x-x,2)+pow(it2->y-y,2));
                double l1=sqrt(pow(it3->x-x,2)+pow(it3->y-y,2));
                double l2=sqrt(pow(it2->x-it3->x,2)+pow(it2->y-it3->y,2));
                bool hit0=val_is_val(l0+l1,l2,0.01);

                //check if x,y is on line segment
                double l3=sqrt(pow(it1->start.x-x,2)+pow(it1->start.y-y,2));
                double l4=sqrt(pow(it1->end.x-x,2)+pow(it1->end.y-y,2));
                double l5=sqrt(pow(it1->start.x-it1->end.x,2)+pow(it1->start.y-it1->end.y,2));
                bool hit1=val_is_val(l3+l4,l5,0.01);

                if(hit0 && hit1){ //connect segments

                    it->end={x,y,0};
                    it->control.erase(it2,it->control.end());
                    it1->start={x,y,0};
                    trigger=true;
                    break;
                }
            }
        }
    }

    if(!trigger){
        //linestrip end segment to compare
        startpoint_x0 = it->control.back().x;
        startpoint_y0 = it->control.back().y;
        endpoint_x0 = it->end.x;
        endpoint_y0 = it->end.y;

        //line 2 to compare :
        startpoint_x1 = it1->start.x;
        startpoint_y1 = it1->start.y;
        endpoint_x1 = it1->end.x;
        endpoint_y1 = it1->end.y;

        //line 1
        delta_y0 = endpoint_y0 - startpoint_y0;
        delta_x0 = startpoint_x0 - endpoint_x0;
        c0 = delta_y0 * startpoint_x0 + delta_x0 * startpoint_y0;

        //line 2
        delta_y1 = endpoint_y1 - startpoint_y1;
        delta_x1 = startpoint_x1 - endpoint_x1;
        c1 = delta_y1 * startpoint_x1 + delta_x1 * startpoint_y1;

        determinant = delta_y0*delta_x1 - delta_y1*delta_x0;
        //qDebug() << "determinant is : " << determinant;

        if(determinant == 0){
            //std::cout<< "the lines are parallel"<<std::endl;
        } else {
            double x = (delta_x1*c0 - delta_x0*c1)/determinant;
            double y = (delta_y0*c1 - delta_y1*c0)/determinant;
            //std::cout<< " intersection point is x : " << x << " y : " << y<<std::endl;

            //check if x,y is on linestrip segment
            double l0=sqrt(pow(it->control.back().x-x,2)+pow(it->control.back().y-y,2));
            double l1=sqrt(pow(it->end.x-x,2)+pow(it->end.y-y,2));
            double l2=sqrt(pow(it->control.back().x-it->end.x,2)+pow(it->control.back().y-it->end.y,2));
            bool hit0=val_is_val(l0+l1,l2,0.01);

            //check if x,y is on line segment
            double l3=sqrt(pow(it1->start.x-x,2)+pow(it1->start.y-y,2));
            double l4=sqrt(pow(it1->end.x-x,2)+pow(it1->end.y-y,2));
            double l5=sqrt(pow(it1->start.x-it1->end.x,2)+pow(it1->start.y-it1->end.y,2));
            bool hit1=val_is_val(l3+l4,l5,0.01);

            if(hit0 && hit1){ //connect segments
                it->start={x,y,0};
                it->type="line";
                it->control.clear();
                it1->start={x,y,0};
            }
        }
    }
}

void linestrip_linestrip_connect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1)
{

}

void arc_arc_connect(std::vector<OBJECT>::iterator it, std::vector<OBJECT>::iterator it1)
{
    POINT p;
    // Find the distance between the centers.
    double dx = it->center.x - it1->center.x;
    double dy = it->center.y - it1->center.y;
    double dist = sqrt(dx * dx + dy * dy);

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
            double ix = ((cx2 + h * (it1->center.y - it->center.y) / dist));
            double iy = ((cy2 - h * (it1->center.x - it->center.x) / dist));
            bool check=check_if_point_is_on_arc(it,{ix,iy,0});
            if(check){
                it->end.x=ix;
                it->end.y=iy;
                it1->start.x=ix;
                it1->start.y=iy;
                // p={ix,iy,0};
                //it->intersect.push_back({ix,iy,0});
                // std::cout<<"arc_arc_intersect x: "<<ix<<" y: " << iy<<std::endl;
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
                it->end.x=ix;
                it->end.y=iy;
                it1->start.x=ix;
                it1->start.y=iy;
                // p={ix,iy,0};
                //it->intersect.push_back({ix,iy,0});
                // std::cout<<"arc_arc_intersect x: "<<ix<<" y: " << iy<<std::endl;
            }

            bool check2=check_if_point_is_on_arc(it,{ix1,iy1,0});
            bool check3=check_if_point_is_on_arc(it1,{ix1,iy1,0});
            if(check2 && check3){
                it->end.x=ix1;
                it->end.y=iy1;
                it1->start.x=ix1;
                it1->start.y=iy1;
                //p={ix1,ix2,0};
                // it->intersect.push_back({ix1,iy1,0});
                // std::cout<<"arc_arc_intersect x: "<<ix1<<" y: " << iy1<<std::endl;
            }
        }
    }
}

void update_arc_controlpoint(std::vector<OBJECT>::iterator it){

    double x1 = (it->control.front().x + it->start.x) / 2;
    double y1 = (it->control.front().y + it->start.y) / 2;
    double dy1 = it->control.front().x - it->start.x;
    double dx1 = -(it->control.front().y - it->start.y);

    // Get the perpendicular bisector of (x2, y2) and (x3, y3).
    double x2 = (it->end.x + it->control.front().x) / 2;
    double y2 = (it->end.y + it->control.front().y) / 2;
    double dy2 = it->end.x - it->control.front().x;
    double dx2 = -(it->end.y - it->control.front().y);

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
    double pi_start_angle=0;
    double pi_end_angle=0;
    double radius=0;
    double x_center=0;
    double y_center=0;

    if (determinant == 0) // The lines are parallel.
    {
        // lines are parallel
    }
    else  {
        x_center = (delta_x1*c0 - delta_x0*c1)/determinant;
        y_center = (delta_y0*c1 - delta_y1*c0)/determinant;
        double dx = x_center - it->start.x;
        double dy = y_center - it->start.y;
        radius = sqrt(dx * dx + dy * dy);

        //formula if arc is cw or ccw.. this is nice..
        //To determine which side of the line from A=(x1,y1) to B=(x2,y2) a point P=(x,y) falls on you need to compute the value:-
        //d=(x−x1)(y2−y1)−(y−y1)(x2−x1) //take the arc controlpoint for x,y
        //If d<0 then the point lies on one side of the line, and if d>0 then it lies on the other side. If d=0 then the point lies exactly line.

//        double xp = it->end.x;
//        double yp = it->end.y;
//        double x1 = it->start.x;
//        double y1 = it->start.y;
//        double x2 = it->control.front().x;
//        double y2 = it->control.front().y;
//        double d = ((xp-x1)*(y2-y1))-((yp-y1)*(x2-x1));

        if(it->d > 0){ //we take the determinant from object memory.
            pi_start_angle = atan2(it->end.y-y_center, it->end.x-x_center);    //start angle G2
            pi_end_angle = atan2(it->start.y-y_center, it->start.x-x_center);    //end angle G2

            if(pi_start_angle>pi_end_angle){
                pi_end_angle+=2*M_PI;
            }
            it->control.front().x = x_center + (cos((pi_start_angle+pi_end_angle)/2)*radius);
            it->control.front().y = y_center + (sin((pi_start_angle+pi_end_angle)/2)*radius);
        }
        if(it->d < 0){
            pi_start_angle = atan2(it->start.y-y_center, it->start.x-x_center);    //start angle G3
            pi_end_angle  = atan2(it->end.y-y_center, it->end.x-x_center);    //end angle G3
            if(pi_start_angle>pi_end_angle){
                pi_end_angle+=2*M_PI;
            }
            it->control.front().x = x_center + (cos((pi_start_angle+pi_end_angle)/2)*radius);
            it->control.front().y = y_center + (sin((pi_start_angle+pi_end_angle)/2)*radius);
        }
        if(it->d == 0){
            //draw straight line
        }
    }
}

bool point_is_point(POINT a, POINT b, double res){
    if(a.x<b.x+res && a.x>b.x-res && a.y<b.y+res && a.y>b.y-res){
        return true;
    }
    return false;
}

bool val_is_val(double a, double b, double res){
    if(a<b+res && a>b-res){
        return true;
    }
    return false;
}

#endif // LIB_CONNECT_H












