#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <string>
#include <math.h>

struct POINT{
    double x,y,z;
};

struct OBJECT {
    //cad section

    //enum type {line, linestrip, arc, circle, bezier, ellipse, spline, rectangular, polygon};

    std::string type; //line, linestrip, etc
    std::string layer;
    double linewidth;
    unsigned short linetype;
    int linescale;
    double red,green,blue,alpha;
    int dxfcolor;

    POINT start;
    POINT end;
    POINT center;
    POINT midpoint;
    POINT input;
    std::vector<POINT> control;
    std::vector<POINT> intersect;
    int end_intersection=-1; //final end intersection number, preprocess data for lib_pocket.h
    int sector; //final sector number, calculated by lib_pocket.h
    std::vector<double> intersect_deg_angle;
    double lenght; //only used calculate and erase dots at line objects
    double pi_angle_start, pi_angle_end, deg_angle_start, deg_angle_end, radius, ratio, sides;
    bool select=0;
    double d; //orginal arc determinant

    //cam section
    int nr=0;
    unsigned int job;
    bool end_contour;
};
extern OBJECT object;
extern std::vector<OBJECT> cad,cam,restore_cad;

struct CONTOUR {
    std::vector<int> sequence; //object sequence inside contour
    std::vector<int> inside; //containing inside contours
    int contourtype=0; //0=open, 1=closed, 2=single object closed
    int depth=0;
    bool select=0;
    double area=0;
    double poly_cx;
    double poly_cy;
};
extern CONTOUR contour;
extern std::vector<CONTOUR> contours;

extern std::vector<int> sequence;

struct SECTOR{
    std::vector<int> primitive;
    double area;
};
extern std::vector<SECTOR> sectors;

struct JOBS{
    std::string jobtype;
    std::string layer;
    std::string tool;
    std::string head;
    std::string mcode_start;
    std::string mcode_stop;
    double offset_x;
    double offset_y;
};
extern JOBS job;
extern std::vector<JOBS> jobs;

struct HEAD{
    std::string axis;
    double offset_x;
    double offset_y;
    std::string m_code_start;
    std::string m_code_stop;
};
extern HEAD head;
extern std::vector<HEAD> heads;

struct TOOL{
    std::string toolname;
    std::string tooltype;
    std::string head;
    double travelheight;
    double workheight;
    double feedrate;
    double lead_in;
    double lead_out;
    double drill_depth;
    double retract_distance;
    int retract_times;
    bool use_lead_in;
    bool use_lead_out;
    bool use_retract;
    double diameter;
    double rpm;
    double plungeheight;
    double plungespeed;
    double delay;
};
extern TOOL tool;
extern std::vector<TOOL> tools;

struct GCODE{
    std::string intro;
    std::string outtro;
    std::string delay_m_code;
    std::string filepath;
    std::string file_extension;
    unsigned int decimal_precision;
    bool use_linenumbers;
    std::string linenumber_character;
    std::string filename;
};
extern GCODE gcode_settings;

#endif // OBJECT_H































