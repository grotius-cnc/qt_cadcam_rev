#ifndef FILE_MATRIX
#define FILE_MATRIX
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
    DATE      : 14.06.2020
    REVISION  : --
    TODO      : --
    USAGE     : --
    PROTOCOL  : --
    NOTES     :

    7-4-2020, made by grotius, no licence, free to use.
    header file only library
    function: 3d matrix calculation :
     1. 3d translate (move)
     2. 3d scale
     3. 3d rotate
     4. 3d mirror on xy, yz, xz plane
     5. 3d shear

    reference:
    https://www.youtube.com/watch?v=RqZH-7hlI48
    http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
    http://www.c-jump.com/bcc/common/Talk3/Math/Matrices/W01_0100_3d_transformations.htm
    https://www.gatevidyalay.com/3d-shearing-in-computer-graphics-definition-examples/

    usage:
    #include <matrix_3d.h>
    vec3d trans0=translate_3d(50,5,0,10,0,0);
    vec3d scale0=scale_3d(50,10,30,2);
    vec3d rot0=rotate_3d(1,0,0,0,30,53.1);
    vec3d mir0 = mirror_3d(10,0,0,0,1,0);
    vec3d shear0 = shearing_3d(1,1,2,2,2,3,1,0,0); //will shear in x axis ==> output : after shearing = (1, 3, 5).
    vec3d shear0 = shearing_3d(1,1,2,2,2,3,0,1,0); //will shear in y axis
    vec3d shear0 = shearing_3d(1,1,2,2,2,3,0,0,1); //will shear in z axis
*/
#include <iostream>
#include <math.h>
#include <structs.h>

POINT translate_3d(double x_base,double y_base, double z_base, double x_translate, double y_translate, double z_translate){

    double matrix[4][4] = {
        {1, 0, 0, 0}, //[1 0 0 x]
        {0, 1, 0, 0}, //[0 1 0 y]
        {0, 0, 1, 0}, //[0 0 1 z]
        {0, 0, 0, 1}  //[0 0 0 1]
    };
    matrix[0][3]=x_base;
    matrix[1][3]=y_base;
    matrix[2][3]=z_base;

    double vector[4] = {0, 0, 0, 1}; //x,y,z,w //w=1 for translate, w=0 for direction
    vector[0]=x_translate;
    vector[1]=y_translate;
    vector[2]=z_translate;

    double x=(matrix[0][0]*vector[0])+
            (matrix[0][1]*vector[1])+
            (matrix[0][2]*vector[2])+
            (matrix[0][3]*vector[3]);

    double y=(matrix[1][0]*vector[0])+
            (matrix[1][1]*vector[1])+
            (matrix[1][2]*vector[2])+
            (matrix[1][3]*vector[3]);

    double z=(matrix[2][0]*vector[0])+
            (matrix[2][1]*vector[1])+
            (matrix[2][2]*vector[2])+
            (matrix[2][3]*vector[3]);

    double w=(matrix[3][0]*vector[0])+
            (matrix[3][1]*vector[1])+
            (matrix[3][2]*vector[2])+
            (matrix[3][3]*vector[3]);

    //    cout<<" x:"<<x<<endl;
    //    cout<<" y:"<<y<<endl;
    //    cout<<" z:"<<z<<endl;
    //    cout<<" w:"<<w<<endl;

    POINT result;
    result.x=x;
    result.y=y;
    result.z=z;
    return result;
}

POINT scale_3d(double x_base,double y_base, double z_base, double scale_x, double scale_y, double scale_z){

    double sx=scale_x;
    double sy=scale_y;
    double sz=scale_z;
    double matrix[4][4] = {
        {sx, 0,  0,  0}, //[x 0 0 0]
        {0,  sy, 0,  0}, //[0 y 0 0]
        {0,  0,  sz, 0}, //[0 0 z 0]
        {0,  0,  0,  1}  //[0 0 0 1]
    };

    double vector[4] = {0, 0, 0, 1}; //[x,y,z,w] //w=1 for translate, w=0 for direction
    vector[0]=x_base;
    vector[1]=y_base;
    vector[2]=z_base;

    double x=(matrix[0][0]*vector[0])+
            (matrix[0][1]*vector[1])+
            (matrix[0][2]*vector[2])+
            (matrix[0][3]*vector[3]);

    double y=(matrix[1][0]*vector[0])+
            (matrix[1][1]*vector[1])+
            (matrix[1][2]*vector[2])+
            (matrix[1][3]*vector[3]);

    double z=(matrix[2][0]*vector[0])+
            (matrix[2][1]*vector[1])+
            (matrix[2][2]*vector[2])+
            (matrix[2][3]*vector[3]);

    double w=(matrix[3][0]*vector[0])+
            (matrix[3][1]*vector[1])+
            (matrix[3][2]*vector[2])+
            (matrix[3][3]*vector[3]);

    //    cout<<" x:"<<x<<endl;
    //    cout<<" y:"<<y<<endl;
    //    cout<<" z:"<<z<<endl;
    //    cout<<" w:"<<w<<endl;

    POINT result;
    result.x=x;
    result.y=y;
    result.z=z;
    return result;
}

POINT rotate_3d(double x_to_rotate,double y_to_rotate, double z_to_rotate, double rotate_degrees_x, double rotate_degrees_y, double rotate_degrees_z){

    double cx=cos(rotate_degrees_x*M_PI/180.0); //cos of rotate_degrees
    double sx=sin(rotate_degrees_x*M_PI/180.0);
    double matrix_x[4][4] = { //this matrix only calculates rotate_degrees into cos and sin value's
                              { 1, 0,  0,  0 },
                              { 0, cx,-sx, 0 },
                              { 0, sx, cx, 0 },
                              { 0, 0,  0,  1 }
                            };

    double cy=cos(rotate_degrees_y*M_PI/180.0); //cos of rotate_degrees
    double sy=sin(rotate_degrees_y*M_PI/180.0);
    double matrix_y[4][4] = {
        { cy, 0, sy, 0 },
        { 0,  1, 0,  0 },
        {-sy, 0, cy, 0 },
        { 0,  0, 0,  1 },
    };

    double cz=cos(rotate_degrees_z*M_PI/180.0); //cos of rotate_degrees
    double sz=sin(rotate_degrees_z*M_PI/180.0);
    double matrix_z[4][4] = {
        { cz,-sz, 0, 0 },
        { sz, cz, 0, 0 },
        { 0,   0, 1, 0 },
        { 0,   0, 0, 1 }
    };

    double vector[4] = {0, 0, 0, 1}; //[x,y,z,w] //w=1 for translate, w=0 for direction
    vector[0]=x_to_rotate;
    vector[1]=y_to_rotate;
    vector[2]=z_to_rotate;

    double x=(matrix_x[0][0]*vector[0])+(matrix_x[0][1]*vector[1])+(matrix_x[0][2]*vector[2])+(matrix_x[0][3]*vector[3]);
    double y=(matrix_x[1][0]*vector[0])+(matrix_x[1][1]*vector[1])+(matrix_x[1][2]*vector[2])+(matrix_x[1][3]*vector[3]);
    double z=(matrix_x[2][0]*vector[0])+(matrix_x[2][1]*vector[1])+(matrix_x[2][2]*vector[2])+(matrix_x[2][3]*vector[3]);
    double w=(matrix_x[3][0]*vector[0])+(matrix_x[3][1]*vector[1])+(matrix_x[3][2]*vector[2])+(matrix_x[3][3]*vector[3]);

    //rotate around y axis, take the result of the x axis rotation above :
    vector[0]=x;
    vector[1]=y;
    vector[2]=z;
    x=(matrix_y[0][0]*vector[0])+(matrix_y[0][1]*vector[1])+(matrix_y[0][2]*vector[2])+(matrix_y[0][3]*vector[3]);
    y=(matrix_y[1][0]*vector[0])+(matrix_y[1][1]*vector[1])+(matrix_y[1][2]*vector[2])+(matrix_y[1][3]*vector[3]);
    z=(matrix_y[2][0]*vector[0])+(matrix_y[2][1]*vector[1])+(matrix_y[2][2]*vector[2])+(matrix_y[2][3]*vector[3]);
    w=(matrix_y[3][0]*vector[0])+(matrix_y[3][1]*vector[1])+(matrix_y[3][2]*vector[2])+(matrix_y[3][3]*vector[3]);

    //rotate around z axis, take the result of the y axis rotation above :
    vector[0]=x;
    vector[1]=y;
    vector[2]=z;
    x=(matrix_z[0][0]*vector[0])+(matrix_z[0][1]*vector[1])+(matrix_z[0][2]*vector[2])+(matrix_z[0][3]*vector[3]);
    y=(matrix_z[1][0]*vector[0])+(matrix_z[1][1]*vector[1])+(matrix_z[1][2]*vector[2])+(matrix_z[1][3]*vector[3]);
    z=(matrix_z[2][0]*vector[0])+(matrix_z[2][1]*vector[1])+(matrix_z[2][2]*vector[2])+(matrix_z[2][3]*vector[3]);
    w=(matrix_z[3][0]*vector[0])+(matrix_z[3][1]*vector[1])+(matrix_z[3][2]*vector[2])+(matrix_z[3][3]*vector[3]);

    //    cout<<" x:"<<x<<endl;
    //    cout<<" y:"<<y<<endl;
    //    cout<<" z:"<<z<<endl;
    //    cout<<" w:"<<w<<endl;

    POINT result;
    result.x=x;
    result.y=y;
    result.z=z;
    return result;
}

POINT mirror_3d(double x_to_mirror,double y_to_mirror, double z_to_mirror, bool xy_plane, bool yz_plane, bool xz_plane){

    double matrix_xy_plane[4][4] = { //this matrix only calculates rotate_degrees into cos and sin value's
                                     { 1, 0, 0, 0 },
                                     { 0, 1, 0, 0 },
                                     { 0, 0,-1, 0 },
                                     { 0, 0, 0, 1 }
                                   };

    double matrix_yz_plane[4][4] = { //this matrix only calculates rotate_degrees into cos and sin value's
                                     {-1, 0, 0, 0 },
                                     { 0, 1, 0, 0 },
                                     { 0, 0, 1, 0 },
                                     { 0, 0, 0, 1 }
                                   };

    double matrix_xz_plane[4][4] = { //this matrix only calculates rotate_degrees into cos and sin value's
                                     { 1, 0, 0, 0 },
                                     { 0,-1, 0, 0 },
                                     { 0, 0, 1, 0 },
                                     { 0, 0, 0, 1 }
                                   };

    double vector[4] = {0, 0, 0, 1}; //[x,y,z,w] //w=1 for translate, w=0 for direction
    //mirror around xy plane:
    vector[0]=x_to_mirror;
    vector[1]=y_to_mirror;
    vector[2]=z_to_mirror;
    double x=(matrix_xy_plane[0][0]*vector[0])+(matrix_xy_plane[0][1]*vector[1])+(matrix_xy_plane[0][2]*vector[2])+(matrix_xy_plane[0][3]*vector[3]);
    double y=(matrix_xy_plane[1][0]*vector[0])+(matrix_xy_plane[1][1]*vector[1])+(matrix_xy_plane[1][2]*vector[2])+(matrix_xy_plane[1][3]*vector[3]);
    double z=(matrix_xy_plane[2][0]*vector[0])+(matrix_xy_plane[2][1]*vector[1])+(matrix_xy_plane[2][2]*vector[2])+(matrix_xy_plane[2][3]*vector[3]);
    double w=(matrix_xy_plane[3][0]*vector[0])+(matrix_xy_plane[3][1]*vector[1])+(matrix_xy_plane[3][2]*vector[2])+(matrix_xy_plane[3][3]*vector[3]);

    //mirror around yz plane:
    vector[0]=x;
    vector[1]=y;
    vector[2]=z;
    x=(matrix_yz_plane[0][0]*vector[0])+(matrix_yz_plane[0][1]*vector[1])+(matrix_yz_plane[0][2]*vector[2])+(matrix_yz_plane[0][3]*vector[3]);
    y=(matrix_yz_plane[1][0]*vector[0])+(matrix_yz_plane[1][1]*vector[1])+(matrix_yz_plane[1][2]*vector[2])+(matrix_yz_plane[1][3]*vector[3]);
    z=(matrix_yz_plane[2][0]*vector[0])+(matrix_yz_plane[2][1]*vector[1])+(matrix_yz_plane[2][2]*vector[2])+(matrix_yz_plane[2][3]*vector[3]);
    w=(matrix_yz_plane[3][0]*vector[0])+(matrix_yz_plane[3][1]*vector[1])+(matrix_yz_plane[3][2]*vector[2])+(matrix_yz_plane[3][3]*vector[3]);

    //mirror around xz plane:
    vector[0]=x;
    vector[1]=y;
    vector[2]=z;
    x=(matrix_xz_plane[0][0]*vector[0])+(matrix_xz_plane[0][1]*vector[1])+(matrix_xz_plane[0][2]*vector[2])+(matrix_xz_plane[0][3]*vector[3]);
    y=(matrix_xz_plane[1][0]*vector[0])+(matrix_xz_plane[1][1]*vector[1])+(matrix_xz_plane[1][2]*vector[2])+(matrix_xz_plane[1][3]*vector[3]);
    z=(matrix_xz_plane[2][0]*vector[0])+(matrix_xz_plane[2][1]*vector[1])+(matrix_xz_plane[2][2]*vector[2])+(matrix_xz_plane[2][3]*vector[3]);
    w=(matrix_xz_plane[3][0]*vector[0])+(matrix_xz_plane[3][1]*vector[1])+(matrix_xz_plane[3][2]*vector[2])+(matrix_xz_plane[3][3]*vector[3]);

    //    cout<<" x:"<<x<<endl;
    //    cout<<" y:"<<y<<endl;
    //    cout<<" z:"<<z<<endl;
    //    cout<<" w:"<<w<<endl;

    POINT result;
    result.x=x;
    result.y=y;
    result.z=z;
    return result;
}

POINT shearing_3d(double x_base,double y_base, double z_base, double shearing_x, double shearing_y, double shearing_z, bool shearing_x_axis, bool shearing_y_axis, bool shearing_z_axis){

    double x=0; double y=0; double z=0; double w=0;
    double sx=shearing_x;
    double sy=shearing_y;
    double sz=shearing_z;

    if(shearing_x_axis){
        double matrix_x[4][4] = { //this matrix only calculates rotate_degrees into cos and sin value's
                                  { 1, 0, 0, 0 },
                                  { sy,1, 0, 0 },
                                  { sz,0, 1, 0 },
                                  { 0, 0, 0, 1 }
                                };

        double vector[4] = {0, 0, 0, 1}; //[x,y,z,w] //w=1 for translate, w=0 for direction
        //mirror around xy plane:
        vector[0]=x_base;
        vector[1]=y_base;
        vector[2]=z_base;
        x=(matrix_x[0][0]*vector[0])+(matrix_x[0][1]*vector[1])+(matrix_x[0][2]*vector[2])+(matrix_x[0][3]*vector[3]);
        y=(matrix_x[1][0]*vector[0])+(matrix_x[1][1]*vector[1])+(matrix_x[1][2]*vector[2])+(matrix_x[1][3]*vector[3]);
        z=(matrix_x[2][0]*vector[0])+(matrix_x[2][1]*vector[1])+(matrix_x[2][2]*vector[2])+(matrix_x[2][3]*vector[3]);
        w=(matrix_x[3][0]*vector[0])+(matrix_x[3][1]*vector[1])+(matrix_x[3][2]*vector[2])+(matrix_x[3][3]*vector[3]);
    }

    if(shearing_y_axis){
        double matrix_y[4][4] = { //this matrix only calculates rotate_degrees into cos and sin value's
                                  { 1, sx,0, 0 },
                                  { 0, 1, 0, 0 },
                                  { 0, sz,1, 0 },
                                  { 0, 0, 0, 1 }
                                };

        double vector[4] = {0, 0, 0, 1}; //[x,y,z,w] //w=1 for translate, w=0 for direction
        //mirror around xy plane:
        vector[0]=x_base;
        vector[1]=y_base;
        vector[2]=z_base;
        x=(matrix_y[0][0]*vector[0])+(matrix_y[0][1]*vector[1])+(matrix_y[0][2]*vector[2])+(matrix_y[0][3]*vector[3]);
        y=(matrix_y[1][0]*vector[0])+(matrix_y[1][1]*vector[1])+(matrix_y[1][2]*vector[2])+(matrix_y[1][3]*vector[3]);
        z=(matrix_y[2][0]*vector[0])+(matrix_y[2][1]*vector[1])+(matrix_y[2][2]*vector[2])+(matrix_y[2][3]*vector[3]);
        w=(matrix_y[3][0]*vector[0])+(matrix_y[3][1]*vector[1])+(matrix_y[3][2]*vector[2])+(matrix_y[3][3]*vector[3]);
    }

    if(shearing_z_axis){
        double matrix_z[4][4] = { //this matrix only calculates rotate_degrees into cos and sin value's
                                  { 1, 0 ,sx,0 },
                                  { 0, 1, sy,0 },
                                  { 0, 0 ,1, 0 },
                                  { 0, 0, 0, 1 }
                                };

        double vector[4] = {0, 0, 0, 1}; //[x,y,z,w] //w=1 for translate, w=0 for direction
        //mirror around xy plane:
        vector[0]=x_base;
        vector[1]=y_base;
        vector[2]=z_base;
        x=(matrix_z[0][0]*vector[0])+(matrix_z[0][1]*vector[1])+(matrix_z[0][2]*vector[2])+(matrix_z[0][3]*vector[3]);
        y=(matrix_z[1][0]*vector[0])+(matrix_z[1][1]*vector[1])+(matrix_z[1][2]*vector[2])+(matrix_z[1][3]*vector[3]);
        z=(matrix_z[2][0]*vector[0])+(matrix_z[2][1]*vector[1])+(matrix_z[2][2]*vector[2])+(matrix_z[2][3]*vector[3]);
        w=(matrix_z[3][0]*vector[0])+(matrix_z[3][1]*vector[1])+(matrix_z[3][2]*vector[2])+(matrix_z[3][3]*vector[3]);
    }

//    std::cout<<" x:"<<x<<std::endl;
//    std::cout<<" y:"<<y<<std::endl;
//    std::cout<<" z:"<<z<<std::endl;
//    std::cout<<" w:"<<w<<std::endl;

    POINT result;
    result.x=x;
    result.y=y;
    result.z=z;
    return result;
}

#endif





















