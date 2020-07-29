#ifndef VARIABLE_H
#define VARIABLE_H
#include <vector>
#include <string>

extern int click,int_key;
extern double zl,zr,zt,zb,zf,xr,yr,pxw;
extern double mx,my,mz; //mouse cad
extern double mx_snap,my_snap,mz_snap; //mouse cad snap
extern double aspect;
extern double segments;

extern std::string message;
extern std::string mode;
extern int progressbar_value;

//extern double offset;
extern double resolution;
extern bool inside_offset;
extern bool outside_offset;

//nesting vars:
extern double bin_start_x;
extern double bin_start_y;
extern double bin_widht;
extern double bin_height;
extern unsigned int bins;
extern double bin_space_x;
extern double bin_space_y;
extern int rotation_mode;
extern std::string use_holes;
extern unsigned int population_size;
extern unsigned int mutation_rate;
extern double part_spacing;

//cad
extern std::string current_layer;

//update
extern bool update_flag;

class variable
{
public:
    variable();
};

#endif // VARIABLE_H
