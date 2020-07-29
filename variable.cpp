#include "variable.h"
#include <string>
#include <vector>
#include <iostream>
#include <opengl.h>

int click,int_key;
double zl,zr,zt,zb,zf,xr,yr,pxw;
double mx,my,mz; //mouse cad
double mx_snap,my_snap,mz_snap; //mouse cad snap
double aspect;
double segments = 25;

std::string message;
std::string mode;
int progressbar_value;

//double offset;
double resolution=0.05; //spline to linestrip conversion resolution, same idea is used for ellipse, bezier etc.
bool inside_offset;
bool outside_offset;

//nesting vars:
double bin_start_x;
double bin_start_y;
double bin_widht;
double bin_height;
unsigned int bins;
double bin_space_x;
double bin_space_y;
int rotation_mode;
std::string use_holes;
unsigned int population_size;
unsigned int mutation_rate;
double part_spacing;

//cad
std::string current_layer="cad layer 0\n";

//update
bool update_flag=0;

variable::variable()
{

}
























