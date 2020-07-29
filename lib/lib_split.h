#ifndef LIB_SPLIT_H
#define LIB_SPLIT_H
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
    DATE      : 24.6.2020
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
#include <vector>
#include <iterator>
void sort_line_intersections(std::vector<OBJECT>::iterator it);
void sort_arc_intersections(std::vector<OBJECT>::iterator it);
std::vector<OBJECT> sort_linestrip_intersections(std::vector<OBJECT>::iterator it);
bool contains_point(std::vector<POINT> container, POINT p);
POINT arc_control_point(double determinant, double radius, POINT arc_center, POINT arc_start, POINT arc_end);

struct ANGLE{
    double x,y,deg_angle;
};

std::vector<OBJECT> split_primitives(std::vector<OBJECT>array){

    std::vector<OBJECT>::iterator it;
    std::vector<POINT>::iterator it1;
    std::vector<OBJECT> output_array, lines, final_array;
    OBJECT obj;
    obj.control.reserve(1);
    obj.intersect.reserve(1);

    for(it=array.begin(); it<array.end(); it++){
        if(it->type=="line"){
            sort_line_intersections(it);
            obj=*it;
            obj.start.x=it->start.x;
            obj.start.y=it->start.y;
            obj.start.z=it->start.z;
            for(it1=it->intersect.begin(); it1<it->intersect.end(); it1++){
                obj.end.x=it1->x;
                obj.end.y=it1->y;
                obj.end.z=it1->z;
                obj.intersect.push_back({it1->x,it1->y,0});
                output_array.push_back(obj);
                obj.start.x=it1->x;
                obj.start.y=it1->y;
                obj.start.z=it1->z;
            }
            obj.end.x=it->end.x;
            obj.end.y=it->end.y;
            obj.end.z=it->end.z;
            output_array.push_back(obj);
        }
        if(it->type=="arc"){
            sort_arc_intersections(it); //tested ok
            obj=*it;
            obj.start.x=it->start.x;
            obj.start.y=it->start.y;
            obj.start.z=it->start.z;
            for(it1=it->intersect.begin(); it1<it->intersect.end(); it1++){
                obj.end.x=it1->x;
                obj.end.y=it1->y;
                obj.end.z=it1->z;
                obj.control.front()=arc_control_point(obj.d,obj.radius,obj.center,obj.start,obj.end);
                obj.intersect.push_back({it1->x,it1->y,0});
                output_array.push_back(obj);
                obj=*it;
                obj.start.x=it1->x;
                obj.start.y=it1->y;
                obj.start.z=it1->z;
            }
            obj.control.front()=arc_control_point(obj.d,obj.radius,obj.center,obj.start,obj.end);
            obj.end.x=it->end.x;
            obj.end.y=it->end.y;
            obj.end.z=it->end.z;
            output_array.push_back(obj);
        }
        if(it->type=="linestrip"){
            lines=sort_linestrip_intersections(it);
            output_array.insert(output_array.end(),lines.begin(),lines.end());
        }
    }


    //make a list of unique intersections. this is a preprocess for lib_pocket.h
    std::vector<POINT> unique_intersections;
    //collect unique intersection numbers.
    for(it=output_array.begin(); it<output_array.end(); it++){
        for(it1=it->intersect.begin(); it1<it->intersect.end(); it1++){

            bool contains=contains_point(unique_intersections,{it1->x,it1->y,0}); //only accept unique intersections
            if(contains){
                //do nothing, point is already stored
                //std::cout<<"duplicate x: "<<it1->x<<" y: "<<it1->y<<std::endl;
            }
            if(!contains){
                //save point
                unique_intersections.push_back(*it1);
                //std::cout<<"unique intersection saved x: "<<it1->x<<" y: "<<it1->y<<std::endl;
            }
        }
    }

    //attach the unique intersections to object endpoints. Compare wich endpoint is equal to a unique intersection and save this in the object.end_intersect parameter
    for(it=output_array.begin(); it<output_array.end(); it++){
        it->sector=0; //set at 0 here, not initialized in the struct as 0
        for(unsigned int i=0; i<unique_intersections.size(); i++){ //unique intersection is a numbering from 0 to size

            if(it->end.x<unique_intersections.at(i).x+0.01 && it->end.x>unique_intersections.at(i).x-0.01 && it->end.y<unique_intersections.at(i).y+0.01 && it->end.y>unique_intersections.at(i).y-0.01 ){
                it->end_intersection=int(i);
                //std::cout<<"object nr: "<<it-output_array.begin() <<"end intersection nr: "<<it1-unique_intersections.begin()<<std::endl;
            }
        }
    }

    //print result
    int no=0;
    for(it=output_array.begin(); it<output_array.end(); it++){
        std::cout<<"object nr: "<<no<<"end intersection nr: "<<it->end_intersection<<std::endl;
        std::cout<<"start x: "<<it->start.x<<" y: "<<it->start.y<<std::endl;
        std::cout<<"end x: "<<it->end.x<<" y: "<<it->end.y<<std::endl;
        no++;
    }

    return output_array; //change into output array later on
}

POINT arc_control_point(double determinant, double radius, POINT arc_center, POINT arc_start, POINT arc_end){
    POINT p;
    double pi_start=0,pi_end=0;
    if(determinant>0){
        pi_start=atan2(arc_end.y-arc_center.y, arc_end.x-arc_center.x);
        pi_end=atan2(arc_start.y-arc_center.y, arc_start.x-arc_center.x);
    }
    if(determinant<0){
        pi_start=atan2(arc_start.y-arc_center.y, arc_start.x-arc_center.x);
        pi_end=atan2(arc_end.y-arc_center.y, arc_end.x-arc_center.x);
    }
    if (pi_end<pi_start) //this to avoid the start angle is bigger then the end angle value
    {
        pi_end+=2*M_PI;
    }

    double pi_result=(pi_start+pi_end)/2;
    p.x = arc_center.x+(radius * cos(pi_result));
    p.y = arc_center.y+(radius * sin(pi_result));
    return  p;
}

void sort_arc_intersections(std::vector<OBJECT>::iterator it){ //simple and efficient solution

    std::vector<POINT>::iterator it1;
    std::vector<POINT> sorted;
    sorted.reserve(it->intersect.size()+2);

    //to simplify things we draw the arc and find intersections on the arc segments
    double x1 = (it->control.front().x + it->start.x) / 2;
    double y1 = (it->control.front().y + it->start.y) / 2;
    double dy1 = it->control.front().x - it->start.x;
    double dx1 = -(it->control.front().y - it->start.y);

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
    double deg_angle_start=0;
    double deg_angle_end=0;
    double radius=it->radius;
    double x_center=it->center.x;
    double y_center=it->center.y;

    if (determinant==0){
        // lines are parallel
    } else  {
        if(it->d > 0){
            deg_angle_start = 180/M_PI*atan2(it->end.y-y_center, it->end.x-x_center);    //start angle G2
            deg_angle_end = 180/M_PI*atan2(it->start.y-y_center, it->start.x-x_center);    //end angle G2
        }
        if(it->d < 0){
            deg_angle_start = 180/M_PI*atan2(it->start.y-y_center, it->start.x-x_center);    //start angle G3
            deg_angle_end  = 180/M_PI*atan2(it->end.y-y_center, it->end.x-x_center);    //end angle G3
        }
        if(determinant == 0){
            //draw straight line
        }
    }

    double pi_angle_start = (deg_angle_start/360)*(2*M_PI); //start angle
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

    for(unsigned int i = 0; i < segments-1; i++) //to get the second point no out of scope we do : segments-1
    {
        double px1=x+x_center; //first point
        double py1=y+y_center;

        double tx = -y;
        double ty = x;

        x += tx * tangetial_factor;
        y += ty * tangetial_factor;

        x *= radial_factor;
        y *= radial_factor;

        double px2=x+x_center; //next point
        double py2=y+y_center;

        //okey now find the intersections
        for(it1=it->intersect.begin(); it1<it->intersect.end(); it1++){
            double l1=sqrt(pow(px1-it1->x,2)+pow(py1-it1->y,2)); //check if intersection x,y is on the line segment
            double l2=sqrt(pow(px2-it1->x,2)+pow(py2-it1->y,2));
            double l3=sqrt(pow(px2-px1,2)+pow(py2-py1,2));
            if(l3>l1+l2-0.01 && l3<l1+l2+0.01){

                bool contains=contains_point(sorted,{it1->x,it1->y,0}); //only write unique members
                if(!contains){
                    sorted.push_back({it1->x,it1->y,0});
                }
            }
        }
    }

    if(it->d>0){ //opengl only draws g3's. for g2 we reverse the intersection points, tested ok.
        std::reverse(sorted.begin(),sorted.end());
    }

    it->intersect.clear();
    it->intersect=sorted;
    sorted.clear();

    //print output
    /*std::cout<<"arc start x: "<<it->start.x<<" y: "<<it->start.y<<std::endl;
    for(it1=it->intersect.begin(); it1<it->intersect.end(); it1++){
        std::cout<<"ix: "<<it1->x<<" iy: "<<it1->y<<std::endl;
    }
    std::cout<<"arc end x: "<<it->end.x<<" y: "<<it->end.y<<std::endl;*/
}

void sort_line_intersections(std::vector<OBJECT>::iterator it){ //sort from start to end

    //std::cout<<"line start x: "<<it->start.x<<" y: "<<it->start.y<<std::endl;
    for(unsigned int i=0; i<it->intersect.size(); i++){
        //std::cout<<"intersect x: "<<it->intersect.at(i).x<<" y: "<<it->intersect.at(i).y<<std::endl;
    }
    //std::cout<<"line end x: "<<it->end.x<<" y: "<<it->end.y<<std::endl;

    std::vector<POINT>::iterator it1;
    std::vector<POINT> points;

    for(double t=0; t<0.9; t+=0.1){ //line time
        double t1=t+0.1;
        //std::cout<<"t: "<<t<<std::endl;
        //std::cout<<"t1: "<<t1<<std::endl;
        double xt=(((1-t)*it->start.x)+(t*it->end.x));
        double yt=(((1-t)*it->start.y)+(t*it->end.y));
        double xt1=(((1-t1)*it->start.x)+(t1*it->end.x));
        double yt1=(((1-t1)*it->start.y)+(t1*it->end.y));

        //std::cout<<"xt: "<<xt<<" yt: "<<yt<<std::endl;
        //std::cout<<"xt1: "<<xt1<<" yt1: "<<yt1<<std::endl;

        for(it1=it->intersect.begin(); it1<it->intersect.end(); it1++){

            //check if intersection point is on the line segment
            double l1=sqrt(pow(xt-it1->x,2)+pow(yt-it1->y,2)); //check if intersection x,y is on the line segment
            double l2=sqrt(pow(xt1-it1->x,2)+pow(yt1-it1->y,2));
            double l3=sqrt(pow(xt-xt1,2)+pow(yt-yt1,2));
            if(l3>l1+l2-0.01 && l3<l1+l2+0.01){
                //point is on the line segment
                bool contains=contains_point(points,{it1->x,it1->y,0}); //only add unique members
                if(!contains){
                    points.push_back({it1->x,it1->y,0});
                    //std::cout<<"added to points vector x: "<<it1->x<<" y: "<<it1->y<<std::endl;
                }
            }
        }
    }
    it->intersect.clear();
    it->intersect=points; //add the sorted intersection points
}

std::vector<OBJECT> sort_linestrip_intersections(std::vector<OBJECT>::iterator it){ //sort from start to end

    std::vector<POINT>::iterator it1, it2,it3;
    std::vector<POINT> points; //dummy vector, used for unique
    std::vector<OBJECT> objects;
    OBJECT obj;
    obj.type="line";
    obj.red=255; obj.green=0; obj.blue=255; obj.alpha=255;
    obj.intersect.reserve(1);
    obj.start=it->start;

    //startpoint to first controlpoint
    for(double t=0; t<0.9; t+=0.01){ //line time
        double t1=t+0.01;

        double xt=(((1-t)*it->start.x)+(t*it->control.front().x));
        double yt=(((1-t)*it->start.y)+(t*it->control.front().y));
        double xt1=(((1-t1)*it->start.x)+(t1*it->control.front().x));
        double yt1=(((1-t1)*it->start.y)+(t1*it->control.front().y));

        for(it1=it->intersect.begin(); it1<it->intersect.end(); it1++){

            //check if intersection point is on the line segment
            double l1=sqrt(pow(xt-it1->x,2)+pow(yt-it1->y,2)); //check if intersection x,y is on the line segment
            double l2=sqrt(pow(xt1-it1->x,2)+pow(yt1-it1->y,2));obj.end;
            double l3=sqrt(pow(xt-xt1,2)+pow(yt-yt1,2));
            if(l3>l1+l2-0.01 && l3<l1+l2+0.01){
                //point is on the line segment
                bool contains=contains_point(points,{it1->x,it1->y,0}); //only add unique members
                if(!contains){
                    points.push_back({it1->x,it1->y,0});
                    obj.end={it1->x,it1->y,0};
                    obj.intersect.push_back({it1->x,it1->y,0});
                    objects.push_back(obj);
                    obj.start={it1->x,it1->y,0};
                }
            }
        }
    }
    points.clear();
    obj.end={it->control.front().x,it->control.front().y,0};
    objects.push_back(obj);
    obj.start={it->control.front().x,it->control.front().y,0};

    for(it2=it->control.begin(); it2<it->control.end()-1; it2++){
        it3=it2+1;
        //first controlpoint to last controlpoint
        for(double t=0; t<0.9; t+=0.01){ //line time
            double t1=t+0.01;

            double xt=(((1-t)*it2->x)+(t*it3->x));
            double yt=(((1-t)*it2->y)+(t*it3->y));
            double xt1=(((1-t1)*it2->x)+(t1*it3->x));
            double yt1=(((1-t1)*it2->y)+(t1*it3->y));

            for(it1=it->intersect.begin(); it1<it->intersect.end(); it1++){

                //check if intersection point is on the line segment
                double l1=sqrt(pow(xt-it1->x,2)+pow(yt-it1->y,2)); //check if intersection x,y is on the line segment
                double l2=sqrt(pow(xt1-it1->x,2)+pow(yt1-it1->y,2));
                double l3=sqrt(pow(xt-xt1,2)+pow(yt-yt1,2));
                if(l3>l1+l2-0.01 && l3<l1+l2+0.01){
                    //point is on the line segment
                    bool contains=contains_point(points,{it1->x,it1->y,0}); //only add unique members
                    if(!contains){
                        points.push_back({it1->x,it1->y,0});
                        obj.end={it1->x,it1->y,0};
                        obj.intersect.push_back({it1->x,it1->y,0});
                        objects.push_back(obj);
                        obj.start={it1->x,it1->y,0};
                    }
                }
            }
        }
        points.clear();
        obj.end={it3->x,it3->y,0};
        objects.push_back(obj);
        obj.start={it3->x,it3->y,0};
    }

    //last controlpoint to endpoint
    for(double t=0; t<0.9; t+=0.01){ //line time
        double t1=t+0.01;

        double xt=(((1-t)*it->control.back().x)+(t*it->end.x));
        double yt=(((1-t)*it->control.back().y)+(t*it->end.y));
        double xt1=(((1-t1)*it->control.back().x)+(t1*it->end.x));
        double yt1=(((1-t1)*it->control.back().y)+(t1*it->end.y));


        for(it1=it->intersect.begin(); it1<it->intersect.end(); it1++){

            //check if intersection point is on the line segment
            double l1=sqrt(pow(xt-it1->x,2)+pow(yt-it1->y,2)); //check if intersection x,y is on the line segment
            double l2=sqrt(pow(xt1-it1->x,2)+pow(yt1-it1->y,2));
            double l3=sqrt(pow(xt-xt1,2)+pow(yt-yt1,2));
            if(l3>l1+l2-0.01 && l3<l1+l2+0.01){
                //point is on the line segment
                bool contains=contains_point(points,{it1->x,it1->y,0}); //only add unique members
                if(!contains){
                    points.push_back({it1->x,it1->y,0});
                    obj.end={it1->x,it1->y,0};
                    obj.intersect.push_back({it1->x,it1->y,0});
                    objects.push_back(obj);
                    obj.start={it1->x,it1->y,0};
                }
            }
        }
    }
    points.clear();
    obj.end=it->end;
    objects.push_back(obj);

    return objects;
}

bool contains_point(std::vector<POINT> container, POINT p){
    std::vector<POINT>::iterator it1;
    for(it1=container.begin(); it1<container.end(); it1++){
        if(p.x>it1->x-0.01 && p.x<it1->x+0.01 && p.y>it1->y-0.01 && p.y<it1->y+0.01 ){
            return true;
        }
    }
    return false;
}

#endif // LIB_SPLIT_H























