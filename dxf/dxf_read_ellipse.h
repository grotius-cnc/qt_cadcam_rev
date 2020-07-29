#include <variable.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <algorithm>
#include <structs.h>

void controlpoints_ellipse();
void rgba_ellipse(int color);
POINT ellipse_startpoint(double xcen, double ycen, double xend, double yend, double xinput, double yinput, double pi_angle_start, double pi_angle_end, double ratio);
//http://help.autodesk.com/view/ACD/2016/ENU/?guid=GUID-107CB04F-AD4D-4D2F-8EC9-AC90888063AB

void read_ellipse(std::string filename)
{
    bool trigger=0;
    unsigned int layer_line=0;
    unsigned int color_line=0;
    unsigned int x_center_line=0;
    unsigned int y_center_line=0;
    unsigned int z_center_line=0;
    unsigned int x_endpoint_mayor_axis_line=0;
    unsigned int y_endpoint_mayor_axis_line=0;
    unsigned int z_endpoint_mayor_axis_line=0;
    unsigned int ratio_line=0;
    unsigned int start_angle_line=0;
    unsigned int end_angle_line=0;

    std::string layer;
    //    double x_center=0;
    //    double y_center=0;
    //    double z_center=0;
    double x_endpoint_mayor=0;
    double y_endpoint_mayor=0;
    double z_endpoint_mayor=0;
    //    double ratio=0;
    //    double start_angle=0;
    //    double end_angle=0;
    //    double color=0;

    unsigned int linenumber=0;
    char count[100]; //line buffer
    FILE *file=fopen(filename.c_str(), "r"); //convert std::string to std::char

    while(file && fgets(count, sizeof(count), file)){
        linenumber++;

        if(strstr(count,"ELLIPSE")){
            //cout<<"lwline found at:"<<linenumber<< endl;
            trigger=1;
            object.type="ellipse";
        }

        if(trigger && strstr(count,"  8")){
            //cout<<"8 found at:"<<linenumber<< endl;
            layer_line=linenumber+1;
        }
        if(trigger && linenumber==layer_line){
            //cout<<"layer:"<<count<< endl;
            //layer=count;
            object.layer=count;
        }

        if(trigger && strstr(count," 62")){
            //cout<<"62 found at:"<<linenumber<< endl;
            color_line=linenumber+1;
        }
        if(trigger && linenumber==color_line){
            //cout<<"color:"<<count<< endl;
            //color=std::stoi(count);
            rgba_ellipse(std::stoi(count));
        }

        if(trigger && strstr(count," 10")){
            //cout<<"10 found at:"<<linenumber<< endl;
            x_center_line=linenumber+1;
        }
        if(trigger && linenumber==x_center_line){
            //cout<<"x start:"<<count<< endl;
            //x_center=std::stod(count);
            object.center.x=std::stod(count);
        }

        if(trigger && strstr(count," 20")){
            //cout<<"20 found at:"<<linenumber<< endl;
            y_center_line=linenumber+1;
        }
        if(trigger && linenumber==y_center_line){
            //cout<<"y start:"<<count<< endl;
            //y_center=std::stod(count);
            object.center.y=std::stod(count);
        }

        if(trigger && strstr(count," 30")){
            //cout<<"30 found at:"<<linenumber<< endl;
            z_center_line=linenumber+1;
        }
        if(trigger && linenumber==z_center_line){
            //cout<<"z start:"<<count<< endl;
            //z_center=std::stod(count);
            object.center.z=std::stod(count);
        }

        if(trigger && strstr(count," 11")){
            //cout<<"10 found at:"<<linenumber<< endl;
            x_endpoint_mayor_axis_line=linenumber+1;
        }
        if(trigger && linenumber==x_endpoint_mayor_axis_line){
            //cout<<"x start:"<<count<< endl;
            x_endpoint_mayor=std::stod(count);
        }

        if(trigger && strstr(count," 21")){
            //cout<<"20 found at:"<<linenumber<< endl;
            y_endpoint_mayor_axis_line=linenumber+1;
        }
        if(trigger && linenumber==y_endpoint_mayor_axis_line){
            //cout<<"y start:"<<count<< endl;
            y_endpoint_mayor=std::stod(count);
        }

        if(trigger && strstr(count," 31")){
            //cout<<"30 found at:"<<linenumber<< endl;
            z_endpoint_mayor_axis_line=linenumber+1;
        }
        if(trigger && linenumber==z_endpoint_mayor_axis_line){
            //cout<<"z start:"<<count<< endl;
            z_endpoint_mayor=std::stod(count);
        }

        if(trigger && strstr(count," 40")){
            //cout<<"40 found at:"<<linenumber<< endl;
            ratio_line=linenumber+1;
        }
        if(trigger && linenumber==ratio_line){
            //cout<<"Ratio of minor axis to major axis:"<<count<< endl;
            object.ratio=std::stod(count);
        }

        if(trigger && strstr(count," 41")){
            //cout<<"41 found at:"<<linenumber<< endl;
            start_angle_line=linenumber+1;
        }
        if(trigger && linenumber==start_angle_line){
            //cout<<"Start parameter (this value is 0.0 for a full ellipse) :"<<count<< endl;
            //start_angle=std::stod(count);
            object.pi_angle_start=std::stod(count);
        }

        if(trigger && strstr(count," 42")){
            //cout<<"42 found at:"<<linenumber<< endl;
            end_angle_line=linenumber+1;
        }
        if(trigger && linenumber==end_angle_line){
            //cout<<"End parameter (this value is 2pi for a full ellipse) :"<<count<< endl;
            //end_angle=std::stod(count);
            object.pi_angle_end=std::stod(count);
        }

        if(trigger && strstr(count,"  0")){
            trigger=0;

            object.end.x=object.center.x+x_endpoint_mayor;
            object.end.y=object.center.y+y_endpoint_mayor;
            object.end.z=object.center.z+z_endpoint_mayor;

            object.control.push_back({0,0,0}); //empty control point needed by display.h

            //when no colorcode " 62" is found..
            if(int(object.red)==0 && int(object.green)==0 && int(object.blue)==0 && int(object.alpha)==0){
                object.red = 255;
                object.green = 255;
                object.blue = 255;
                object.alpha = 255;
            }

            controlpoints_ellipse();
            object.start=ellipse_startpoint(object.center.x, object.center.y, object.end.x, object.end.y, object.input.x, object.input.y, object.pi_angle_start, object.pi_angle_end, object.ratio);
            cad.push_back(object);

            //reset object value's
            object.type="";
            object.layer="";
            object.red=0;
            object.green=0;
            object.blue=0;
            object.alpha=0;
            object.start.x=0;
            object.start.y=0;
            object.start.z=0;
            object.control.clear();
            object.end.x=0;
            object.end.y=0;
            object.end.z=0;
            object.center.x=0;
            object.center.y=0;
            object.center.z=0;
            object.radius=0;
            object.ratio=0;
            object.deg_angle_start=0;
            object.deg_angle_end=0;

        }
    }
}

void rgba_ellipse(int color)
{
    if(color == 1){ //red
        object.red = 255; //red
        object.green = 0; //green
        object.blue = 0; //blue
        object.alpha = 255; //alpha
    } else if(color == 2){ //yellow
        object.red = 255; //red
        object.green = 255; //green
        object.blue = 0; //blue
        object.alpha = 255; //alpha
    } else if(color == 3){ //green
        object.red = 0; //red
        object.green = 255; //green
        object.blue = 0; //blue
        object.alpha = 255; //alpha
    } else if(color == 4){ //cyan
        object.red = 255; //red
        object.green = 0; //green
        object.blue = 255; //blue
        object.alpha = 255; //alpha
    } else if(color == 5){ //blue
        object.red = 49; //red
        object.green = 112; //green
        object.blue = 206; //blue
        object.alpha = 255; //alpha
    } else if(color == 6){ //magenta
        object.red = 0; //red
        object.green = 255; //green
        object.blue = 255; //blue
        object.alpha = 255; //alpha
    } else if(color == 7){ //white
        object.red = 255; //red
        object.green = 255; //green
        object.blue = 255; //blue
        object.alpha = 255; //alpha
    } else if(color == 8){ //grey
        object.red = 106; //red
        object.green = 106; //green
        object.blue = 106; //blue
        object.alpha = 255; //alpha
    } else if(color == 9){ //orange/brown
        object.red = 255; //red
        object.green = 170; //green
        object.blue = 0; //blue
        object.alpha = 255; //alpha
    } else {
        object.red = 255; //when no standard colorcode is found we use white
        object.green = 255;
        object.blue = 255;
        object.alpha = 255;
    }
}

void controlpoints_ellipse(){
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

POINT ellipse_startpoint(double xcen, double ycen, double xend, double yend, double xinput, double yinput, double pi_angle_start, double pi_angle_end, double ratio){

    POINT p;
    //ellipse center = xcen, ycen
    //ellipse base right = xend, yend
    //ellipse top point = xinput, yinput

    double center_x = xcen;
    double center_y = ycen;

    //center point to endpoint mayor axis..
    double radius_x = sqrt(pow(xend-xcen,2) + pow(yend-ycen,2));
    //ratio minor axis to mayor axis..
    double radius_y=0;

    if(ratio==0){ //takes the controlpoint instead of ratio factor
        radius_y = sqrt(pow(xinput-xcen,2) + pow(yinput-ycen,2));
    }
    if(ratio!=0){ //if ratio from dxf is available, it takes its value
        radius_y = ratio*radius_x;
    }

    //calculate angle of ellipse
    double x1=xend;
    double y1=yend;
    double radian = atan2(y1 - center_y, x1 - center_x);
    double angle = radian * (180 / M_PI);
    if (angle < 0.0)
        angle += 360.0;

    double j = pi_angle_start;

    //non rotated ellipse..
    double non_rotated_x = /*center_x +*/ cos(j)*radius_x;
    double non_rotated_y = /*center_y +*/ sin(j)*radius_y;
    //glVertex2d(center_x + cos(j)*radius_x, center_y + sin(j)*radius_y);

    //rotated ellipse..
    p.x = center_x  + cos(angle * M_PI / 180.0 )* non_rotated_x - sin(angle * M_PI / 180.0 ) * non_rotated_y;
    p.y = center_y  + sin(angle * M_PI / 180.0 )* non_rotated_x + cos(angle * M_PI / 180.0 ) * non_rotated_y;

    return p;


}





