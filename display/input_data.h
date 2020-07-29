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
    REVISION  : 13-06-2020  added calculation for arc start and arc end angles in degree, for user input, in dxf the values are already available.
    TODO      : --
    USAGE     : To use this header only library/file, include also structs.h to your project.
    PROTOCOL  : --
    NOTES     : --
*/

#include <variable.h>
#include <structs.h>
#include <math.h>
#include <iostream>
#include <algorithm>

OBJECT object;
std::vector<OBJECT> cad,cam,restore_cad;
POINT arc_center(POINT a, POINT b, POINT c);
std::vector<double> arc_deg_start_deg_end_by_controlpoint(double xs, double ys, double xcon, double ycon, double xe, double ye);
POINT update_controlpoint(double xs, double ys, double xcon, double ycon, double xe, double ye);
void controlpoints_circle();
void controlpoints_ellipse();
void clear();
void input_data(){

    //input data
    if(mode=="line"){
        if(click==0){
            message = "click startpoint";
        }
        if(click==1){
            object.type="line";
            object.layer=current_layer;
            object.red=0; //todo : connect to gui
            object.green=1;
            object.blue=1;
            object.alpha=1;
            object.start.x=mx;
            object.start.y=my;
            object.start.z=mz;
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
            click=2;
        }
        if(click==2){ //preview
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
        }
        if(click==3){ //write data
            cad.push_back(object);
            clear();
        }
        if(int_key==16777216){//key escape, clean preview data
            clear();
        }
    }

    if(mode=="linestrip"){
        if(click==0){
            message = "click startpoint";
        }
        if(click==1){
            object.type="line";
            object.layer=current_layer;
            object.red=0; //todo : connect to gui
            object.green=1;
            object.blue=1;
            object.alpha=1;
            object.start.x=mx;
            object.start.y=my;
            object.start.z=mz;
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
            click=2;
        }
        if(click==2){ //preview
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
        }
        if(click==3){ //morph into linestrip
            object.type="linestrip";
            object.control.push_back({mx,my,mz});
            click=2;
        }
        if(int_key==16777216){//key escape, clean preview data
            clear();
        }
        if(int_key==16777220){//key enter
            object.end.x=object.control.back().x; //save including the last control point
            object.end.y=object.control.back().y;
            object.end.z=object.control.back().z;
            object.control.erase(object.control.end()-1);

            //print output
            //for(int i=0; i<object.control.size(); i++){
            //    std::cout<<"control x:"<<object.control.at(i).x<<" control y:"<<object.control.at(i).y<<std::endl;
            //}

            cad.push_back(object);
            clear();
        }
    }

    if(mode=="rectangular"){
        if(click==0){
            message = "click startpoint";
        }
        if(click==1){
            object.type="rectangular";
            object.layer=current_layer;
            object.red=0; //todo : connect to gui
            object.green=1;
            object.blue=1;
            object.alpha=1;
            object.start.x=mx; //lower left corner
            object.start.y=my;
            object.start.z=mz;
            object.control.push_back({mx,my,mz}); //upper left corner
            object.control.push_back({mx,my,mz}); //upper right corner
            object.control.push_back({mx,my,mz}); //lower right corner
            object.end.x=mx; //back to start
            object.end.y=my;
            object.end.z=mz;

            click=2;
        }
        if(click==2){ //preview
            //lower left is unchanged.
            object.control.at(0).y=my; //upper left corner
            object.control.at(1).x=mx; //upper right corner
            object.control.at(1).y=my;
            object.control.at(2).x=mx; //lower right corner
            object.end.x=object.start.x; //endpoint is equal to startpoint
            object.end.y=object.start.y;
            object.end.z=object.start.z;
        }
        if(click==3){ //write data

            //            print output
            //            std::cout<<"start x:"<<object.start.x<<" start y:"<<object.start.y<<std::endl;
            //            for(int i=0; i<object.control.size(); i++){
            //                std::cout<<"control x:"<<object.control.at(i).x<<" control y:"<<object.control.at(i).y<<std::endl;
            //            }
            //            std::cout<<"end x:"<<object.end.x<<" end y:"<<object.end.y<<std::endl;

            cad.push_back(object);
            clear();
        }
        if(int_key==16777216){//key escape, clean preview data
            clear();
        }
    }

    if(mode=="circle"){
        if(click==0){
            message = "click centerpoint";
        }
        if(click==1){
            object.type="circle";
            object.layer=current_layer;
            object.red=0; //todo : connect to gui
            object.green=1;
            object.blue=1;
            object.alpha=1;
            object.center.x=mx;
            object.center.y=my;
            object.center.z=mz;
            click=2;
        }
        if(click==2){ //preview
            object.start.x=mx; //dummy
            object.start.y=my;
            object.start.z=mz;
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
        }
        if(click==3){ //write data
            controlpoints_circle();
            cad.push_back(object);
            clear();
        }
        if(int_key==16777216){//key escape, clean preview data
            clear();
        }
    }

    if(mode=="ellipse"){
        if(click==0){
            message = "click centerpoint";
        }
        if(click==1){
            object.type="line";
            object.layer=current_layer;
            object.red=0; //todo : connect to gui
            object.green=1;
            object.blue=1;
            object.alpha=1;
            object.start.x=mx;
            object.start.y=my;
            object.start.z=mz;
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
            object.center.x=mx;
            object.center.y=my;
            object.center.z=mz;
            object.pi_angle_start = 0;
            object.pi_angle_end = 2*M_PI;
            click=2;
        }
        if(click==2){
            object.end.x=mx; //preview as line
            object.end.y=my;
            object.end.z=mz;
            message = "click endpoint mayor axis";
        }
        if(click==3){
            object.type="ellipse"; //morph into ellipse
            object.start.x=object.end.x;
            object.start.y=object.end.y;
            object.start.z=object.end.z;

            object.input.x=mx; //ellipse height preview
            object.input.y=my;
            object.input.z=mz;

            message = "click endpoint minor axis";
        }
        if(click==4){
            controlpoints_ellipse();
            cad.push_back(object); //write data to program
            clear();
        }
        if(int_key==16777216){//key escape, clean preview data
            clear();
        }
    }

    if(mode=="arc"){
        if(click==0){
            message = "click startpoint";
        }
        if(click==1){
            object.type="line";
            object.layer=current_layer;
            object.red=0; //todo : connect to gui
            object.green=1;
            object.blue=1;
            object.alpha=1;
            object.start.x=mx;
            object.start.y=my;
            object.start.z=mz;
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
            click=2;
        }
        if(click==2){ //preview as line
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
        }
        if(click==3){ //morph into arc
            object.type="arc";
            object.control.push_back({object.end.x,object.end.y,object.end.z});
            click=4;
        }
        if(click==4){ //preview arc endpoint
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
        }
        if(click==5){ //write arc
            //calculate arc center
            POINT center = arc_center({object.start.x,object.start.y,object.start.z},{object.control.front().x,object.control.front().y,object.control.front().z},{object.end.x,object.end.y,object.end.z});
            //calculate deg_angle_start, deg_angle_end
            std::vector<double> deg=arc_deg_start_deg_end_by_controlpoint(object.start.x,object.start.y,object.control.front().x,object.control.front().y,object.end.x,object.end.y);
            POINT control = update_controlpoint(object.start.x,object.start.y,object.control.front().x,object.control.front().y,object.end.x,object.end.y);
            object.control.front()=control;
            //object.deg_angle_start=deg.at(0);
            //object.deg_angle_end=deg.at(1);
            //object.d=deg.at(2); //add the determinant for cam offset
            object.center.x=center.x;
            object.center.y=center.y;
            object.center.z=center.z;
            object.radius=sqrt(pow(object.end.x-object.center.x,2)+pow(object.end.y-object.center.y,2));
            cad.push_back(object);
            clear();
        }

        if(int_key==16777216){//key escape, clean preview data
            clear();
        }
    }

    if(mode=="spline"){
        if(click==0){
            message = "click startpoint";
        }
        if(click==1){
            object.type="line";
            object.layer=current_layer;
            object.red=0; //todo : connect to gui
            object.green=1;
            object.blue=1;
            object.alpha=1;
            object.start.x=mx;
            object.start.y=my;
            object.start.z=mz;
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
            click=2;
        }
        if(click==2){ //preview as line
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
        }
        if(click==3){ //morph into spline
            object.type="spline";
            object.control.push_back({object.end.x,object.end.y,object.end.z});
            click=4;
        }
        if(click==4){ //preview spline endpoint
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
        }

        if(click>4){ //add new controlpoint
            click=3;
        }

        if(int_key==16777216){//key escape, clean preview data
            clear();
        }

        if(int_key==16777220){//key enter
            if(object.control.size()>1){
                object.end.x=object.control.back().x; //finish spline endpoint to last clicked control point
                object.end.y=object.control.back().y;
                object.end.z=object.control.back().z;
                object.control.pop_back(); //remove last control point.
            }

            object.red=0; //todo : connect to gui
            object.green=1;
            object.blue=1;
            object.alpha=1;
            cad.push_back(object); //add spline to program data, clean preview data
            clear();
        }
    }

    if(mode=="bezier"){

        if(click==0){
            message = "click startpoint";
        }
        if(click==1){
            object.type="line";
            object.layer=current_layer;
            object.red=0; //todo : connect to gui
            object.green=1;
            object.blue=1;
            object.alpha=1;
            object.start.x=mx;
            object.start.y=my;
            object.start.z=mz;
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
            click=2;
        }
        if(click==2){ //preview
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
        }
        if(click==3){ //morph into spline
            object.type="spline";
            object.control.push_back({mx,my,mz}); //first control point
            click=4;
        }
        if(click==4){ //preview
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
        }
        if(click==5){
            object.control.push_back({mx,my,mz}); //second control point
            click=6;
        }
        if(click==6){ //preview
            object.type="bezier"; //morph into bezier
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
        }
        if(click==7){
            cad.push_back(object);
            clear();
        }
        if(int_key==16777216){//key escape, clean preview data
            clear();
        }
    }

    if(mode=="polygon"){
        if(click==0){
            message = "click centerpoint";
        }
        if(click==1){
            object.type="polygon";
            object.layer=current_layer;
            object.red=0; //todo : connect to gui
            object.green=1;
            object.blue=1;
            object.alpha=1;
            object.center.x=mx;
            object.center.y=my;
            object.center.z=mz;
            object.sides=10;
            click=2;
        }
        if(click==2){ //preview
            object.end.x=mx;
            object.end.y=my;
            object.end.z=mz;
        }
        if(click==3){ //write data
            object.type="linestrip"; //morph into linestrip
            cad.push_back(object);
            clear();
        }
        if(int_key==16777216){//key escape, clean preview data
            clear();
        }
    }
}

void clear(){
    object.type="";
    object.layer="";
    object.red=0;
    object.green=0;
    object.blue=0;
    object.alpha=0;
    object.start.x=0;
    object.start.y=0;
    object.start.z=0;
    object.end.x=0;
    object.end.y=0;
    object.end.z=0;
    object.control.clear();
    click=0;
    int_key=-1; //reset key
    update_flag=1; //flag to update cad layers in mainwindow
}

POINT arc_center(POINT a, POINT b, POINT c){
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

void controlpoints_circle(){
    int segments = 14;
    double radius = sqrt(pow(object.end.x-object.center.x,2)+pow(object.end.y-object.center.y,2));
    for (int j = 1; j<segments; j++){ //ccw direction
        double theta = 2.0 * M_PI * int(j) / int(segments);
        double x = radius * cos(theta);
        double y = radius * sin(theta);
        object.control.push_back({x+object.center.x,y+object.center.y,object.center.z});
         std::cout<<"segment nr: "<<j<<std::endl;
        std::cout<<"control x:"<<x+object.center.x<<" control y"<<y+object.center.y<<std::endl;
    }
    //morph circle into cw
    std::reverse(object.control.begin(),object.control.end());

    object.radius=radius;
}

void conrolpoints_ellipse(){
    //center point to endpoint mayor axis, center point to inputpoint minor axis
    double radius_x = sqrt(pow(object.end.x-object.center.x,2) + pow(object.end.y-object.center.y,2));
    double radius_y = sqrt(pow(object.input.x-object.center.x,2) + pow(object.input.y-object.center.y,2));

    //calculate angle of ellipse
    double x1=object.end.x;
    double y1=object.end.y;
    double radian = atan2(y1 - object.center.y, x1 - object.center.x);
    double angle = radian * (180 / M_PI);
    if (angle < 0.0)
        angle += 360.0;

    for(double j = object.pi_angle_start+M_PI/4; j<object.pi_angle_end; j+=M_PI/4) //ccw direction
    {
        //non rotated ellipse..
        double non_rotated_x = /*center_x +*/ cos(j)*radius_x;
        double non_rotated_y = /*center_y +*/ sin(j)*radius_y;
        //glVertex2d(center_x + cos(j)*radius_x, center_y + sin(j)*radius_y);

        //rotated ellipse..
        double rotate_x = object.center.x  + cos(angle * M_PI / 180.0 )* non_rotated_x - sin(angle * M_PI / 180.0 ) * non_rotated_y;
        double rotate_y = object.center.y  + sin(angle * M_PI / 180.0 )* non_rotated_x + cos(angle * M_PI / 180.0 ) * non_rotated_y;
        object.control.push_back({rotate_x,rotate_y,0});
        //std::cout<<"control x:"<<rotate_x<<" control y"<<rotate_y<<std::endl;
    }

    //morph ellipse into cw by inversing the control points
    std::reverse(object.control.begin(),object.control.end());
}

std::vector<double> arc_deg_start_deg_end_by_controlpoint(double xs, double ys, double xcon, double ycon, double xe, double ye){

    std::vector<double> deg_angles;
    double x1 = (xcon + xs) / 2;
    double y1 = (ycon + ys) / 2;
    double dy1 = xcon - xs;
    double dx1 = -(ycon - ys);

    // Get the perpendicular bisector of (x2, y2) and (x3, y3).
    double x2 = (xe + xcon) / 2;
    double y2 = (ye + ycon) / 2;
    double dy2 = xe - xcon;
    double dx2 = -(ye - ycon);

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

    if (determinant == 0) // The lines are parallel.
    {
        // lines are parallel
    }
    else  {
        x_center = (delta_x1*c0 - delta_x0*c1)/determinant;
        y_center = (delta_y0*c1 - delta_y1*c0)/determinant;
        double dx = x_center - xs;
        double dy = y_center - ys;
        radius = sqrt(dx * dx + dy * dy);

        //formula if arc is cw or ccw.. this is nice..
        //To determine which side of the line from A=(x1,y1) to B=(x2,y2) a point P=(x,y) falls on you need to compute the value:-
        //d=(x−x1)(y2−y1)−(y−y1)(x2−x1) //take the arc controlpoint for x,y
        //If d<0 then the point lies on one side of the line, and if d>0 then it lies on the other side. If d=0 then the point lies exactly line.

        double xp = xe;
        double yp = ye;
        double x1 = xs;
        double y1 = ys;
        double x2 = xcon;
        double y2 = ycon;

        double d = ((xp-x1)*(y2-y1))-((yp-y1)*(x2-x1));
        //qDebug() << "d :" << d;

        if(d > 0){
            deg_start_angle = 180/M_PI*atan2(ye-y_center, xe-x_center);    //start angle G2
            deg_end_angle = 180/M_PI*atan2(ys-y_center, xs-x_center);    //end angle G2
            deg_angles.push_back(deg_start_angle);
            deg_angles.push_back(deg_end_angle);

            //add the determinant:
            deg_angles.push_back(d);

        }
        if(d < 0){
            deg_start_angle = 180/M_PI*atan2(ys-y_center, xs-x_center);    //start angle G3
            deg_end_angle  = 180/M_PI*atan2(ye-y_center, xe-x_center);    //end angle G3
            deg_angles.push_back(deg_start_angle);
            deg_angles.push_back(deg_end_angle);

            //add the determinant:
            deg_angles.push_back(d);
        }
        if(d == 0){
            //draw straight line
        }
    }
    return deg_angles;
}

POINT update_controlpoint(double xs, double ys, double xcon, double ycon, double xe, double ye){

    POINT p;
    double x1 = (xcon + xs) / 2;
    double y1 = (ycon + ys) / 2;
    double dy1 = xcon - xs;
    double dx1 = -(ycon - ys);

    // Get the perpendicular bisector of (x2, y2) and (x3, y3).
    double x2 = (xe + xcon) / 2;
    double y2 = (ye + ycon) / 2;
    double dy2 = xe - xcon;
    double dx2 = -(ye - ycon);

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
        double dx = x_center - xs;
        double dy = y_center - ys;
        radius = sqrt(dx * dx + dy * dy);

        //formula if arc is cw or ccw.. this is nice..
        //To determine which side of the line from A=(x1,y1) to B=(x2,y2) a point P=(x,y) falls on you need to compute the value:-
        //d=(x−x1)(y2−y1)−(y−y1)(x2−x1) //take the arc controlpoint for x,y
        //If d<0 then the point lies on one side of the line, and if d>0 then it lies on the other side. If d=0 then the point lies exactly line.

        double xp = xe;
        double yp = ye;
        double x1 = xs;
        double y1 = ys;
        double x2 = xcon;
        double y2 = ycon;
        double d = ((xp-x1)*(y2-y1))-((yp-y1)*(x2-x1));

        if(d > 0){
            pi_start_angle = atan2(ye-y_center, xe-x_center);    //start angle G2
            pi_end_angle = atan2(ys-y_center, xs-x_center);    //end angle G2

            if(pi_start_angle>pi_end_angle){
                pi_end_angle+=2*M_PI;
            }
            p.x = x_center + (cos((pi_start_angle+pi_end_angle)/2)*radius);
            p.y = y_center + (sin((pi_start_angle+pi_end_angle)/2)*radius);
        }
        if(d < 0){
            pi_start_angle = atan2(ys-y_center, xs-x_center);    //start angle G3
            pi_end_angle  = atan2(ye-y_center, xe-x_center);    //end angle G3
            if(pi_start_angle>pi_end_angle){
                pi_end_angle+=2*M_PI;
            }
            p.x = x_center + (cos((pi_start_angle+pi_end_angle)/2)*radius);
            p.y = y_center + (sin((pi_start_angle+pi_end_angle)/2)*radius);
        }
        if(d == 0){
            //draw straight line
        }
    }
    return p;
}


























