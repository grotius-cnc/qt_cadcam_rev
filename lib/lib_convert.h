#ifndef LIB_CONVERT_H
#define LIB_CONVERT_H
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
std::vector<POINT> construct_spline_1(std::vector<std::vector<double>> C, double resolution);

std::vector<POINT> bezier_to_points(std::vector<OBJECT>::iterator it, double resolution){
    //void draw_4p_bezier(double xs, double ys, double zs, double xcon0, double ycon0, double zcon0, double xcon1, double ycon1, double zcon1, double xend, double yend, double zend, double red, double green, double blue, double alpha){
    POINT p;
    std::vector<POINT> points;
    std::vector<POINT> excl_tiny;

    for(double t=0; t<1; t+=resolution){

        //Calculate Point E, on the line AB
        double Ex = ((1-t) * it->start.x) + (t * it->control.front().x);
        double Ey = ((1-t) * it->start.y) + (t * it->control.front().y);
        double Ez = ((1-t) * it->start.z) + (t * it->control.front().z);

        //Calculate Point F, on the line BC
        double Fx = ((1-t) * it->control.front().x) + (t * it->control.at(1).x);
        double Fy = ((1-t) * it->control.front().y) + (t * it->control.at(1).y);
        double Fz = ((1-t) * it->control.front().z) + (t * it->control.at(1).z);

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
        p.x = ((1-t) * Qx) + (t * Rx);
        p.y = ((1-t) * Qy) + (t * Ry);
        p.z = ((1-t) * Qz) + (t * Rz);

        points.push_back(p);
    }

    //remove small segments
    std::vector<POINT>::iterator it1,it2;
    it1=points.begin();
    excl_tiny.push_back(*it1); //startpoint ellipse
    for(it2=points.begin(); it2<points.end()-1; it2++){ //excluding last point

        double l=sqrt(pow(it1->x-it2->x,2)+pow(it1->y-it2->y,2));

        if(l>1){ //when lenght reaches the value, write points
            excl_tiny.push_back(*it2);
            it1=it2; //set new startpoint
        }
    }
    return excl_tiny;
}


std::vector<POINT> ellipse_to_points(std::vector<OBJECT>::iterator it, double resolution){

    //ellipse center = xcen, ycen
    //ellipse base right = xend, yend
    //ellipse top point = xinput, yinput

    POINT p;
    std::vector<POINT> points;
    std::vector<POINT> excl_tiny;

    double center_x = it->center.x;
    double center_y = it->center.y;

    //center point to endpoint mayor axis..
    double radius_x = sqrt(pow(it->end.x-it->center.x,2) + pow(it->end.y-it->center.y,2));
    //ratio minor axis to mayor axis..
    double radius_y=0;

    if(it->ratio==0){ //takes the controlpoint instead of ratio factor
        radius_y = sqrt(pow(it->input.x-it->center.x,2) + pow(it->input.y-it->center.y,2));
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

    for(double j = it->pi_angle_start; j<it->pi_angle_end; j+=resolution)
    {
        //non rotated ellipse..
        double non_rotated_x = /*center_x +*/ cos(j)*radius_x;
        double non_rotated_y = /*center_y +*/ sin(j)*radius_y;

        //rotated ellipse..
        p.x = center_x  + cos(angle * M_PI / 180.0 )* non_rotated_x - sin(angle * M_PI / 180.0 ) * non_rotated_y;
        p.y = center_y  + sin(angle * M_PI / 180.0 )* non_rotated_x + cos(angle * M_PI / 180.0 ) * non_rotated_y;
        points.push_back(p);
    }


    //remove small segments
    std::vector<POINT>::iterator it1,it2;
    it1=points.begin();
    excl_tiny.push_back(*it1); //startpoint ellipse
    for(it2=points.begin(); it2<points.end()-1; it2++){ //excluding last point

        double l=sqrt(pow(it1->x-it2->x,2)+pow(it1->y-it2->y,2));

        if(l>1){ //when lenght reaches the value, write points
            excl_tiny.push_back(*it2);
            it1=it2; //set new startpoint
        }
    }
    return excl_tiny;
}

std::vector<POINT> spline_to_points(std::vector<OBJECT>::iterator it, double resolution){

    //spline startpoint,endpoint,controlpoints into container
    std::vector<std::vector<double>> C; //2d array
    C.push_back({it->start.x, it->start.y}); //spline startpoint
    for(unsigned int j=0; j<it->control.size(); j++){
        C.push_back({it->control.at(j).x, it->control.at(j).y}); //spline controlpoints
    }
    C.push_back({it->end.x, it->end.y}); //spline endpoint

    //calculate the "grotius invented true spline"
    //the forward and backward function has only to be recalculated when spline controlpoints are edited by user.
    std::vector<POINT> forward=construct_spline_1(C,resolution);
    std::reverse(C.begin(),C.end());
    std::vector<POINT> backward=construct_spline_1(C,resolution);
    std::reverse(backward.begin(),backward.end());
    std::vector<POINT> difference, excl_tiny;
    POINT p;

    //get the true spline coordinates
    for(unsigned int i=0; i<forward.size(); i++){
        p.x=(forward.at(i).x+backward.at(i).x)/2;
        p.y=(forward.at(i).y+backward.at(i).y)/2;
        difference.push_back(p);
    }

    //remove small segments
    std::vector<POINT>::iterator it1,it2;
    it1=difference.begin();
    excl_tiny.push_back(*it1); //startpoint spline
    for(it2=difference.begin(); it2<difference.end()-1; it2++){ //excluding last point

        double l=sqrt(pow(it1->x-it2->x,2)+pow(it1->y-it2->y,2));

        if(l>1){ //when lenght reaches the value, write points
            excl_tiny.push_back(*it2);
            it1=it2; //set new startpoint
        }
    }
    return excl_tiny;
}

std::vector<POINT> construct_spline_1(std::vector<std::vector<double>> C, double resolution){
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





















#endif // LIB_CONVERT_H
