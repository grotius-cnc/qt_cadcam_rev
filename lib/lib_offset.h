#ifndef FILE_LIB_OFFSET
#define FILE_LIB_OFFSET
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
    DATE      : 10.6.2020
    REVISION  : 13.06.2020  line offset bug solved
                            added pi start angle, pi and angle for arc offset bug
                            added arc g2/g3 direction
    TODO      : --
    USAGE     : --
    PROTOCOL  : --
    NOTES     : --
*/
#include <iostream>
#include <algorithm>
#include <math.h>
#include <structs.h>
#include <lib/lib_matrix.h>
#include <lib/lib_intersect.h>

std::vector<POINT> spline_vector_points(int objectnr);
OBJECT line_offset_coordinate_input(double xstart, double ystart, double xend, double yend, double offset);
std::vector<double> arc_deg_start_deg_end(double xs, double ys, double xcon, double ycon, double xe, double ye);
OBJECT linestrip_save_self_intersect(OBJECT obj);
double area_from_points(std::vector<POINT> p);
std::vector<POINT> construct_spline(std::vector<std::vector<double>> C, double resolution);

POINT offset_point_on_line(double xs, double ys, double xe, double ye, double offset_from_xs_ys){
    //  cross calculation
    //    A-----------B------------C
    // (Xa,Ya)     (Xb,Yb)      (Xc,Yc)
    //    AB = sqrt( (Xb - Xa)² + (Yb - Ya)² )
    //    AC = 1000
    //    Xc = Xa + (AC * (Xb - Xa) / AB)
    //    Yc = Ya + (AC * (Yb - Ya) / AB)
    POINT p;
    double AB=sqrt(pow(xe-xs,2)+pow(ye-ys,2));
    double AC=offset_from_xs_ys;
    p.x=xs+(AC*(xe-xs)/AB);
    p.y=ys+(AC*(ye-ys)/AB);
    return p;
}

OBJECT line_offset(std::vector<OBJECT>::iterator it, double offset){
    OBJECT data;
    POINT p_rot;

    //endpoint offsets relative to startpoint zero
    double dx=it->end.x-it->start.x;
    double dy=it->end.y-it->start.y;
    if(offset>0){
        p_rot=rotate_3d(dx,dy,0,0,0,90);
    }
    if(offset<0){
        p_rot=rotate_3d(dx,dy,0,0,0,-90);
    }

    //bring back to orginal coordinates
    p_rot.x+=it->start.x;
    p_rot.y+=it->start.y;

    POINT ps;
    if(offset>0){
        ps=offset_point_on_line(it->start.x,it->start.y,p_rot.x,p_rot.y,offset*-1);
    }
    if(offset<0){
        ps=offset_point_on_line(it->start.x,it->start.y,p_rot.x,p_rot.y,offset);
    }

    data.type="line";
    data.start.x=ps.x;
    data.start.y=ps.y;
    data.start.z=it->start.z;
    data.end.x=ps.x+dx;
    data.end.y=ps.y+dy;
    data.end.z=it->end.z;
    data.red=255;
    data.green=255;
    data.blue=0;
    data.alpha=255;
    data.control.clear();
    //std::cout<<"start x: "<<data.start.x<<" start y: "<<data.start.y<< " end x: "<<data.end.x<<" end y: "<<data.end.y<<std::endl;
    return data;
}

OBJECT line_offset_coordinate_input(double xstart, double ystart, double xend, double yend, double offset){
    OBJECT data;
    POINT p_rot;

    //endpoint offsets relative to startpoint zero
    double dx=xend-xstart;
    double dy=yend-ystart;
    if(offset>0){
        p_rot=rotate_3d(dx,dy,0,0,0,90);
    }
    if(offset<0){
        p_rot=rotate_3d(dx,dy,0,0,0,-90);
    }

    //bring back to orginal coordinates
    p_rot.x+=xstart;
    p_rot.y+=ystart;

    POINT ps;
    if(offset>0){
        ps=offset_point_on_line(xstart,ystart,p_rot.x,p_rot.y,offset*-1);
    }
    if(offset<0){
        ps=offset_point_on_line(xstart,ystart,p_rot.x,p_rot.y,offset);
    }

    data.type="line";
    data.start.x=ps.x;
    data.start.y=ps.y;
    data.end.x=ps.x+dx;
    data.end.y=ps.y+dy;
    data.red=255;
    data.green=255;
    data.blue=0;
    data.alpha=255;
    data.control.clear();
    return data;
}

OBJECT linestrip_offset(std::vector<OBJECT>::iterator it, double offset){ //transforms linestrip into offsetted lines to be intersected later on.
    OBJECT linestrip;
    linestrip.type="linestrip";
    linestrip.red=255;
    linestrip.green=255;
    linestrip.blue=0;
    linestrip.alpha=255;
    POINT p_rot;

    //startpoint
    double dx=it->control.front().x-it->start.x;
    double dy=it->control.front().y-it->start.y;
    //rotate vector
    if(offset>0){
        p_rot=rotate_3d(dx,dy,0,0,0,90);
    }
    if(offset<0){
        p_rot=rotate_3d(dx,dy,0,0,0,-90);
    }

    //move vector
    p_rot.x+=it->start.x;
    p_rot.y+=it->start.y;

    //calculate offset point
    POINT ps;
    if(offset>0){
        ps=offset_point_on_line(it->start.x,it->start.y,p_rot.x,p_rot.y,offset*-1);
    }
    if(offset<0){
        ps=offset_point_on_line(it->start.x,it->start.y,p_rot.x,p_rot.y,offset);
    }

    //write data
    linestrip.start.x=ps.x;
    linestrip.start.y=ps.y;

    //controlpoints
    std::vector<POINT>::iterator it1, it2;
    for(it1=it->control.begin(); it1<it->control.end()-1; it1++){
        //vector
        it2=it1+1;
        double dx=it2->x-it1->x;
        double dy=it2->y-it1->y;
        //rotate vector
        if(offset>0){
            p_rot=rotate_3d(dx,dy,0,0,0,90);
        }
        if(offset<0){
            p_rot=rotate_3d(dx,dy,0,0,0,-90);
        }

        //move vector
        p_rot.x+=it1->x;
        p_rot.y+=it1->y;

        //calculate offset point
        POINT ps;
        if(offset>0){
            ps=offset_point_on_line(it1->x,it1->y,p_rot.x,p_rot.y,offset*-1);
        }
        if(offset<0){
            ps=offset_point_on_line(it1->x,it1->y,p_rot.x,p_rot.y,offset);
        }

        //write data
        linestrip.control.push_back({ps.x+dx,ps.y+dy,ps.z});
    }

    //endpoint
    dx=it->end.x-it->control.back().x;
    dy=it->end.y-it->control.back().y;
    //rotate vector
    if(offset>0){
        p_rot=rotate_3d(dx,dy,0,0,0,90);
    }
    if(offset<0){
        p_rot=rotate_3d(dx,dy,0,0,0,-90);
    }

    //move vector
    p_rot.x+=it->control.back().x;
    p_rot.y+=it->control.back().y;

    //calculate offset point
    if(offset>0){
        ps=offset_point_on_line(it->control.back().x,it->control.back().y,p_rot.x,p_rot.y,offset*-1);
    }
    if(offset<0){
        ps=offset_point_on_line(it->control.back().x,it->control.back().y,p_rot.x,p_rot.y,offset);
    }

    //write data
    linestrip.end.x=ps.x+dx;
    linestrip.end.y=ps.y+dy;

    OBJECT obj= linestrip_save_self_intersect(linestrip);
    return obj;
}

OBJECT circle_offset(std::vector<OBJECT>::iterator it, double offset){

    OBJECT data;
    data.type="circle";
    data.radius=sqrt(pow(it->end.x-it->center.x,2)+pow(it->end.y-it->center.y,2))+offset;
    data.center.x=it->center.x;
    data.center.y=it->center.y;
    data.center.z=it->center.z;
    data.end.x=data.center.x+data.radius;
    data.end.y=data.center.y;
    data.end.z=data.center.z;
    data.start.x=data.end.x;
    data.start.y=data.end.y;
    data.start.z=data.end.z;
    data.deg_angle_start=0;
    data.deg_angle_end=0;
    data.red=255;
    data.green=255;
    data.blue=0;
    data.alpha=255;
    return data;
}

OBJECT ellipse_offset(std::vector<OBJECT>::iterator it, double offset){

    OBJECT data;
    data.type="ellipse";
    data.red=255;
    data.green=255;
    data.blue=0;
    data.alpha=255;
    data.ratio=0; //if ratio is 0, it takes the input.x and input.y point

    data.center.x=it->center.x;
    data.center.y=it->center.y;
    data.center.z=it->center.z;
    data.pi_angle_start=it->pi_angle_start;
    data.pi_angle_end=it->pi_angle_end;

    double lmayor=sqrt(pow(it->end.x-it->center.x,2)+pow(it->end.y-it->center.y,2));
    POINT mayor=offset_point_on_line(it->center.x,it->center.y,it->end.x,it->end.y,lmayor+offset);
    data.end.x=mayor.x;
    data.end.y=mayor.y;
    data.end.z=it->end.z;

    double lminor=sqrt(pow(it->input.x-it->center.x,2)+pow(it->input.y-it->center.y,2));
    POINT minor=offset_point_on_line(it->center.x,it->center.y,it->input.x,it->input.y,lminor+offset);
    data.input.x=minor.x;
    data.input.y=minor.y;
    data.input.z=it->input.z;

    data.start.x=data.end.x;
    data.start.y=data.end.y;
    data.start.z=data.end.z;

    if(lmayor<0){
        //data.error=1;
    }
    return data;
}

OBJECT ellipse_offset_as_linestrip(std::vector<OBJECT>::iterator it, double resolution, double offset){

    //ellipse center = xcen, ycen
    //ellipse base right = xend, yend
    //ellipse top point = xinput, yinput

    //calculate mayor and minor axis offset points
    double lmayor=sqrt(pow(it->end.x-it->center.x,2)+pow(it->end.y-it->center.y,2));
    POINT mayor=offset_point_on_line(it->center.x,it->center.y,it->end.x,it->end.y,lmayor+offset);
    double lminor=sqrt(pow(it->input.x-it->center.x,2)+pow(it->input.y-it->center.y,2));
    POINT minor=offset_point_on_line(it->center.x,it->center.y,it->input.x,it->input.y,lminor+offset);


    double center_x = it->center.x;
    double center_y = it->center.y;

    //center point to endpoint mayor axis..
    double radius_x = sqrt(pow(mayor.x-it->center.x,2) + pow(mayor.y-it->center.y,2));
    //ratio minor axis to mayor axis..
    double radius_y=0;

    if(it->ratio==0){ //takes the controlpoint instead of ratio factor
        radius_y = sqrt(pow(minor.x-it->center.x,2) + pow(minor.y-it->center.y,2));
    }
    if(it->ratio!=0){ //if ratio from dxf is available, it takes its value
        radius_y = it->ratio*radius_x;
    }

    //calculate angle of ellipse
    double x1=it->end.x;
    double y1=it->end.y;
    double radian = atan2(y1 - center_y, x1 - center_x);
    double angle = radian * (180 / M_PI);
    if (angle < 0.0)
        angle += 360.0;

    OBJECT obj;
    obj.type="linestrip";
    obj.red=255;
    obj.green=255;
    obj.blue=0;
    obj.alpha=255;
    for(double t = it->pi_angle_start; t<it->pi_angle_end; t+=resolution) //std resolution 0.01
    {
        //non rotated ellipse..
        double non_rotated_x = /*center_x +*/ cos(t)*radius_x;
        double non_rotated_y = /*center_y +*/ sin(t)*radius_y;

        //rotated ellipse..
        if(t==it->pi_angle_start){
            obj.start.x=center_x+cos(angle * M_PI / 180.0 )* non_rotated_x - sin(angle * M_PI / 180.0 ) * non_rotated_y;
            obj.start.y=center_y+sin(angle * M_PI / 180.0 )* non_rotated_x + cos(angle * M_PI / 180.0 ) * non_rotated_y;
        }
        if(t>it->pi_angle_start && t<it->pi_angle_end){
            double x=center_x+cos(angle * M_PI / 180.0 )* non_rotated_x - sin(angle * M_PI / 180.0 ) * non_rotated_y;
            double y=center_y+sin(angle * M_PI / 180.0 )* non_rotated_x + cos(angle * M_PI / 180.0 ) * non_rotated_y;
            obj.control.push_back({x,y,0});
        }
    }
    obj.end=obj.control.back();
    obj.control.pop_back();

    obj= linestrip_save_self_intersect(obj);
    return obj;
}

//Todo change into linestrip...
OBJECT bezier_offset_as_linestrip(std::vector<OBJECT>::iterator it, double resolution, double offset){

    // void draw_4p_bezier(double xs, double ys, double zs, double xcon0, double ycon0, double zcon0, double xcon1, double ycon1, double zcon1, double xend, double yend, double zend, double red, double green, double blue, double alpha){
    OBJECT linestrip;
    double xstart=0;
    double ystart=0;
    double zstart=0;
    double xend=0;
    double yend=0;
    double zend=0;

    for(double t=0; t<1; t+=resolution){ //std resolution 0.001

        if(t==0){
            //Calculate Point E, on the line AB
            double Ex = ((1-t) *  it->start.x) + (t * it->control.at(0).x);
            double Ey = ((1-t) *  it->start.y) + (t * it->control.at(0).y);
            double Ez = ((1-t) *  it->start.z) + (t * it->control.at(0).z);
            //Calculate Point F, on the line BC
            double Fx = ((1-t) * it->control.at(0).x) + (t * it->control.at(1).x);
            double Fy = ((1-t) * it->control.at(0).y) + (t * it->control.at(1).y);
            double Fz = ((1-t) * it->control.at(0).z) + (t * it->control.at(1).z);
            //Calculate Point G, on the line CD
            double Gx = ((1-t) * it->control.at(1).x) + (t * it->end.x);
            double Gy = ((1-t) * it->control.at(1).y) + (t * it->end.y);
            double Gz = ((1-t) * it->control.at(1).z) + (t * it->end.z);
            //Calculate Point Q, on the line EF
            double Qx = ((1-t) * Ex) + (t * Fx);
            double Qy = ((1-t) * Ey) + (t * Fy);
            double Qz = ((1-t) * Ez) + (t * Fz);
            //Calculate Point R, on the line FG
            double Rx = ((1-t) * Fx) + (t * Gx);
            double Ry = ((1-t) * Fy) + (t * Gy);
            double Rz = ((1-t) * Fz) + (t * Gz);
            //Calculate Point P, on the line QR
            xstart = ((1-t) * Qx) + (t * Rx);
            ystart = ((1-t) * Qy) + (t * Ry);
            zstart = ((1-t) * Qz) + (t * Rz);
            linestrip.start.x=xstart;
            linestrip.start.y=ystart;
            linestrip.start.z=zstart;
        }

        if(t>0 && t<1){
            //Calculate Point E, on the line AB
            double Ex = ((1-t) *  it->start.x) + (t * it->control.at(0).x);
            double Ey = ((1-t) *  it->start.y) + (t * it->control.at(0).y);
            double Ez = ((1-t) *  it->start.z) + (t * it->control.at(0).z);
            //Calculate Point F, on the line BC
            double Fx = ((1-t) * it->control.at(0).x) + (t * it->control.at(1).x);
            double Fy = ((1-t) * it->control.at(0).y) + (t * it->control.at(1).y);
            double Fz = ((1-t) * it->control.at(0).z) + (t * it->control.at(1).z);
            //Calculate Point G, on the line CD
            double Gx = ((1-t) * it->control.at(1).x) + (t * it->end.x);
            double Gy = ((1-t) * it->control.at(1).y) + (t * it->end.y);
            double Gz = ((1-t) * it->control.at(1).z) + (t * it->end.z);
            //Calculate Point Q, on the line EF
            double Qx = ((1-t) * Ex) + (t * Fx);
            double Qy = ((1-t) * Ey) + (t * Fy);
            double Qz = ((1-t) * Ez) + (t * Fz);
            //Calculate Point R, on the line FG
            double Rx = ((1-t) * Fx) + (t * Gx);
            double Ry = ((1-t) * Fy) + (t * Gy);
            double Rz = ((1-t) * Fz) + (t * Gz);
            //Calculate Point P, on the line QR
            xend = ((1-t) * Qx) + (t * Rx);
            yend = ((1-t) * Qy) + (t * Ry);
            zend = ((1-t) * Qz) + (t * Rz);
            OBJECT line=line_offset_coordinate_input(xstart,ystart,xend,yend,offset);
            linestrip.control.push_back({line.end.x,line.end.y,line.end.z});
            xstart = ((1-t) * Qx) + (t * Rx);
            ystart = ((1-t) * Qy) + (t * Ry);
            zstart = ((1-t) * Qz) + (t * Rz);
        }

        if(t==1){
            //Calculate Point E, on the line AB
            double Ex = ((1-t) *  it->start.x) + (t * it->control.at(0).x);
            double Ey = ((1-t) *  it->start.y) + (t * it->control.at(0).y);
            double Ez = ((1-t) *  it->start.z) + (t * it->control.at(0).z);
            //Calculate Point F, on the line BC
            double Fx = ((1-t) * it->control.at(0).x) + (t * it->control.at(1).x);
            double Fy = ((1-t) * it->control.at(0).y) + (t * it->control.at(1).y);
            double Fz = ((1-t) * it->control.at(0).z) + (t * it->control.at(1).z);
            //Calculate Point G, on the line CD
            double Gx = ((1-t) * it->control.at(1).x) + (t * it->end.x);
            double Gy = ((1-t) * it->control.at(1).y) + (t * it->end.y);
            double Gz = ((1-t) * it->control.at(1).z) + (t * it->end.z);
            //Calculate Point Q, on the line EF
            double Qx = ((1-t) * Ex) + (t * Fx);
            double Qy = ((1-t) * Ey) + (t * Fy);
            double Qz = ((1-t) * Ez) + (t * Fz);
            //Calculate Point R, on the line FG
            double Rx = ((1-t) * Fx) + (t * Gx);
            double Ry = ((1-t) * Fy) + (t * Gy);
            double Rz = ((1-t) * Fz) + (t * Gz);
            //Calculate Point P, on the line QR
            xend = ((1-t) * Qx) + (t * Rx);
            yend = ((1-t) * Qy) + (t * Ry);
            zend = ((1-t) * Qz) + (t * Rz);
            OBJECT line=line_offset_coordinate_input(xstart,ystart,xend,yend,offset);
        }
    }
    OBJECT line=line_offset_coordinate_input(xend,yend,it->end.x,it->end.y,offset); //depending on resolution, now the curve is always fitted to the endpoint.
    linestrip.end.x=line.end.x;
    linestrip.end.y=line.end.y;
    linestrip.end.z=line.end.z;

    OBJECT obj= linestrip_save_self_intersect(linestrip);
    return obj;
}

std::vector<POINT> construct_spline(std::vector<std::vector<double>> C, double resolution){
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


    for(int p=0; p<C.size()-1; p++)  //spline points
    {
        for(double t=0; t<1; t+=resolution)  //time 0-1 for each spline
        {
            POINT point;
            point.x = ((S[p][0][3]*t+S[p][0][2])*t+S[p][0][1])*t+S[p][0][0];
            point.y = ((S[p][1][3]*t+S[p][1][2])*t+S[p][1][1])*t+S[p][1][0];
            //std::cout<< "x:" << px << " y:" << py << std::endl;
            points.push_back(point);
        }
    }
    return points;
}

OBJECT spline_offset_as_linestrip(std::vector<OBJECT>::iterator it, double resolution, double offset){

    //spline startpoint,endpoint,controlpoints into container
    std::vector<std::vector<double>> C; //2d array
    C.push_back({it->start.x, it->start.y}); //spline startpoint
    for(unsigned int j=0; j<it->control.size(); j++){
        C.push_back({it->control.at(j).x, it->control.at(j).y}); //spline controlpoints
    }
    C.push_back({it->end.x, it->end.y}); //spline endpoint

    //calculate the "grotius invented true spline"
    std::vector<POINT>::iterator it1,it2;
    //the forward and backward function has only to be recalculated when spline controlpoints are edited by user.
    std::vector<POINT> forward=construct_spline(C,resolution);
    std::reverse(C.begin(),C.end());
    std::vector<POINT> backward=construct_spline(C,resolution);
    std::reverse(backward.begin(),backward.end());
    std::vector<POINT> difference;
    POINT p;

    //get the true spline coordinates
    for(int i=0; i<forward.size(); i++){
        p.x=(forward.at(i).x+backward.at(i).x)/2;
        p.y=(forward.at(i).y+backward.at(i).y)/2;
        difference.push_back(p);
    }

    //offset the true spline coordinates as a linestrip
    OBJECT obj;
    obj.type="linestrip";
    obj.red=255; obj.green=0; obj.blue=255; obj.alpha=255;

    //non offsetted "true spline" linestrip
    for(it1=difference.begin(); it1<difference.end()-1; it1++){
        it2=it1+1;

        if(it1==difference.begin()){
            OBJECT line=line_offset_coordinate_input(it1->x,it1->y,it2->x,it2->y,offset);
            obj.start=line.start;
        }
        if(it1>difference.begin() && it1<difference.end()-2){
            OBJECT line=line_offset_coordinate_input(it1->x,it1->y,it2->x,it2->y,offset);
            obj.control.push_back(line.start);
        }
        if(it1==difference.end()-2){
            OBJECT line=line_offset_coordinate_input(it1->x,it1->y,it2->x,it2->y,offset);
            obj.end=line.end;
        }
    }
    return obj;
}

OBJECT linestrip_save_self_intersect(OBJECT obj)
{
    std::vector<POINT>::iterator it,it1,it2,it3,it4;
    POINT p;
    for(it=obj.control.begin(); it<obj.control.end()-1; it++){
        it1=it+1;
        for(it3=obj.control.begin(); it3<obj.control.end()-1; it3++){
            it4=it3+1;

            if(it!=it3 && it!=it4 && it1!=it3){ //avoid comparing the same line
                //line1
                double x1=it->x;
                double y1=it->y;
                double x2=it1->x;
                double y2=it1->y;
                //line2
                double x3=it3->x;
                double y3=it3->y;
                double x4=it4->x;
                double y4=it4->y;

                p.x=((((x1*y2)-(y1*x2))*(x3-x4))-((x1-x2)*(x3*y4-y3*x4))) / (((x1-x2)*(y3-y4))-((y1-y2)*(x3-x4))); //wikipedia line-line intersection
                p.y=((((x1*y2)-(y1*x2))*(y3-y4))-((y1-y2)*(x3*y4-y3*x4))) / (((x1-x2)*(y3-y4))-((y1-y2)*(x3-x4)));

                //check if intersection px,py is in the line1 and line2 segment.
                double l1_0=sqrt(pow(x1-p.x,2)+pow(y1-p.y,2));
                double l1_1=sqrt(pow(x2-p.x,2)+pow(y2-p.y,2));
                double l1_tot=sqrt(pow(x2-x1,2)+pow(y2-y1,2));

                double l2_0=sqrt(pow(x3-p.x,2)+pow(y3-p.y,2));
                double l2_1=sqrt(pow(x4-p.x,2)+pow(y4-p.y,2));
                double l2_tot=sqrt(pow(x4-x3,2)+pow(y4-y3,2));

                if((l1_tot>l1_0+l1_1-0.01 && l1_tot<l1_0+l1_1+0.01) && (l2_tot>l2_0+l2_1-0.01 && l2_tot<l2_0+l2_1+0.01)){ //linestrip self intersection spotted
                    //std::cout<<"linestrip self intersecton found at x: "<<p.x<<" y: "<<p.y<<std::endl;
                    //obj.intersect.push_back(p);
                }
            }
        }
    }
    return obj;
}


double area_from_points(std::vector<POINT> p){
    double area=0;
    //if object has controlpoints
    std::vector<POINT>::iterator it,it1;

    if(p.size()>2){
        for(it=p.begin(); it<p.end()-1; it++){
            it1=it+1;
            area+=(it1->x-it->x)*(it1->y+it->y);
        }
    }
    std::cout<<"area: "<<area<<std::endl;
    return area/2;
}

OBJECT arc_offset(OBJECT obj, double offset){ //problem was that there where unwanted calculations after this wich result in rare output.

    double xp = obj.control.front().x;
    double yp = obj.control.front().y;
    double x1 = obj.start.x;
    double y1 = obj.start.y;
    double x2 = obj.end.x;
    double y2 = obj.end.y;
    double d = ((xp-x1)*(y2-y1))-((yp-y1)*(x2-x1));

    if(d > 0){ //g2
        obj.start=offset_point_on_line(obj.start.x,obj.start.y,obj.center.x,obj.center.y,offset*-1);
        obj.control.front()=offset_point_on_line(obj.control.front().x,obj.control.front().y,obj.center.x,obj.center.y,offset*-1);
        obj.end=offset_point_on_line(obj.end.x,obj.end.y,obj.center.x,obj.center.y,offset*-1);
        obj.radius=sqrt(pow(obj.end.x-obj.center.x,2)+pow(obj.end.y-obj.center.y,2));
    }
    if(d < 0){ //g3
        obj.start=offset_point_on_line(obj.start.x,obj.start.y,obj.center.x,obj.center.y,offset);
        obj.control.front()=offset_point_on_line(obj.control.front().x,obj.control.front().y,obj.center.x,obj.center.y,offset);
        obj.end=offset_point_on_line(obj.end.x,obj.end.y,obj.center.x,obj.center.y,offset);
        obj.radius=sqrt(pow(obj.end.x-obj.center.x,2)+pow(obj.end.y-obj.center.y,2));
    }
    if(d == 0){
        //do nothing
    }
    return obj;

}
#endif

















































