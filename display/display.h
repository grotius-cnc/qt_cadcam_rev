#include <variable.h>
#include <opengl.h>
#include <vector>
#include <math.h>
#include <iostream>
#include <structs.h>

void draw_2p_circle(double xc, double yc, double xe, double ye, double red, double green, double blue, double alpha);
void draw_3p_arc_by_point(double xs, double ys, double xc, double yc, double xe, double ye, double red, double green, double blue, double alpha);
void draw_3p_arc_by_angle(double xc, double yc, double deg_angle_start, double deg_angle_end, double radius, double red, double green, double blue, double alpha);
void draw_3p_ellipse(double xcen, double ycen, double xend, double yend, double xinput, double yinput, double pi_angle_start, double pi_angle_end, double ratio, double red, double green, double blue, double alpha);
void draw_spline(std::vector<std::vector<double>> C, double red, double green, double blue, double alpha);
std::vector<POINT> construct_spline(std::vector<std::vector<double>> C);
void draw_4p_bezier(double xs, double ys, double zs, double xcon0, double ycon0, double zcon0, double xcon1, double ycon1, double zcon1, double xend, double yend, double zend, double red, double green, double blue, double alpha);
void preview_polygon(double xcen, double ycen, double zcen, double xend, double yend, double zend, int sides);

void display(){

    //temp controlpoints preview
    //    glLineWidth(1);
    //    glPointSize(5);
    //    glColor4d(255,0,0,255);
    //    glBegin(GL_POINTS);
    //    for(int i=0; i<cad.size(); i++)  //spline points
    //    {
    //        if(cad.at(i).control.size()>0){
    //            glVertex2d(cad.at(i).control.at(0).x,cad.at(i).control.at(0).y);
    //        }
    //    }
    //    glEnd();

    //    //cam arc controlpoints
    //    glLineWidth(1);
    //    glPointSize(5);
    //    glColor4d(0,255,0,255);
    //    glBegin(GL_POINTS);
    //    for(int i=0; i<cam.size(); i++)  //spline points
    //    {
    //        if(cam.at(i).type=="arc"){
    //            glVertex2d(cam.at(i).control.front().x,cam.at(i).control.front().y);
    //        }
    //    }
    //    glEnd();

    //    //temp arc cam points preview
    //    glLineWidth(1);
    //    glPointSize(5);

    //    glBegin(GL_POINTS);
    //    for(int i=0; i<cam.size(); i++)  //spline points
    //        for(int j=0; j<cam.at(i).size(); j++)  //spline points
    //    {
    //        if(cam[i][j].type=="arc"){
    //            glColor4d(0,255,0,255);
    //            glVertex2d(cam[i][j].start.x,cam[i][j].start.y);
    //            glColor4d(255,0,0,255);
    //            glVertex2d(cam[i][j].end.x,cam[i][j].end.y);
    //        }
    //    }
    //    glEnd();

    //    //arc cad controlpoints
    //    glLineWidth(1);
    //    glPointSize(5);
    //    glColor4d(255,0,255,255);
    //    glBegin(GL_POINTS);
    //    for(int i=0; i<cad.size(); i++)  //spline points
    //            if(cad[i].type=="arc"){
    //                glVertex2d(cad[i].control.front().x,cad[i].control.front().y);
    //        }
    //    glEnd();

    //    //arc cam controlpoints
    //    glLineWidth(1);
    //    glPointSize(5);
    //    glColor4d(255,255,0,255);
    //    glBegin(GL_POINTS);
    //    for(int i=0; i<cam.size(); i++)  //spline points
    //            if(cam[i].type=="arc"){
    //                glVertex2d(cam[i].control.at(0).x,cam[i].control.at(0).y);
    //        }
    //    glEnd();

    //show bins
    //glLineWidth(0.5);
    glColor4d(255,0,255,255);

    for(unsigned int i=0; i<bins; i++){
        double bsx = bin_start_x+((bin_widht+bin_space_x)*i);
        double bsy = bin_start_y;
        double bw =  bsx+bin_widht;
        double bh =  bsy+bin_height;

        glBegin(GL_LINE_LOOP);
        glVertex2d(bsx,bsy);
        glVertex2d(bsx,bh);
        glVertex2d(bw,bh);
        glVertex2d(bw,bsy);
        glEnd();
    }

    //line preview function
    if(object.type=="line"){
        //glLineWidth(1);
        glColor4d(object.red,object.green,object.blue,object.alpha);
        glBegin(GL_LINES);
        glVertex3d(object.start.x,object.start.y,object.start.z);
        glVertex3d(object.end.x,object.end.y,object.end.z);
        glEnd();
    }

    //linestrip preview function
    if(object.type=="linestrip" && object.control.size()>0){
        //glLineWidth(1);
        glColor4d(object.red,object.green,object.blue,object.alpha);
        glBegin(GL_LINE_STRIP);
        glVertex3d(object.start.x,object.start.y,object.start.z);
        for(unsigned int i=0; i<object.control.size(); i++){
            glVertex3d(object.control.at(i).x,object.control.at(i).y,object.control.at(i).z);
        }
        glVertex3d(object.end.x,object.end.y,object.end.z);
        glEnd();
    }

    //rectangular preview function
    if(object.type=="rectangular"){
        //glLineWidth(1);
        glColor4d(object.red,object.green,object.blue,object.alpha);
        glBegin(GL_LINE_STRIP);
        glVertex3d(object.start.x,object.start.y,object.start.z); //lower left
        glVertex3d(object.control.at(0).x,object.control.at(0).y,object.control.at(0).z); //upper left
        glVertex3d(object.control.at(1).x,object.control.at(1).y,object.control.at(1).z); //upper right
        glVertex3d(object.control.at(2).x,object.control.at(2).y,object.control.at(2).z); //lower right
        glVertex3d(object.end.x,object.end.y,object.end.z); //back to startpoint
        glEnd();
    }

    //circle preview function
    if(object.type=="circle"){
        draw_2p_circle(object.center.x,object.center.y,object.end.x,object.end.y,object.red,object.green,object.blue,object.alpha);
    }

    //ellipse preview function
    if(object.type=="ellipse"){
        draw_3p_ellipse(object.center.x,object.center.y,object.end.x,object.end.y,object.input.x,object.input.y,object.pi_angle_start,object.pi_angle_end, object.ratio,object.red,object.green,object.blue,object.alpha);
    }

    //arc preview function
    if(object.type=="arc" && object.control.size()>0){
        draw_3p_arc_by_point(object.start.x, object.start.y, object.control.back().x, object.control.back().y, object.end.x, object.end.y, object.red,object.green,object.blue,object.alpha);
    }

    //spline preview function
    if(object.type=="spline" && object.control.size()>0){
        std::vector<std::vector<double>> C; //2d array
        C.push_back({object.start.x, object.start.y}); //spline startpoint
        for(unsigned int i=0; i<object.control.size(); i++){
            C.push_back({object.control.at(i).x, object.control.at(i).y}); //spline controlpoints
        }
        C.push_back({object.end.x, object.end.y}); //spline endpoint
        draw_spline(C, object.red,object.green,object.blue,object.alpha);
        C.clear();
    }

    //bezier preview function
    if(object.type=="bezier"){
        draw_4p_bezier(object.start.x,object.start.y,object.start.z,object.control.front().x,object.control.front().y,object.control.front().z,object.control.back().x,object.control.back().y,object.control.back().z,
                       object.end.x,object.end.y,object.end.z, object.red,object.green,object.blue,object.alpha);
    }

    //polygon preview function
    if(object.type=="polygon"){
        preview_polygon(object.center.x,object.center.y,object.center.z,object.end.x,object.end.y,object.end.z,int(object.sides));
        //glLineWidth(1);
        glColor4d(1,1,1,1);
        glBegin(GL_LINE_STRIP);
        glVertex3d(object.start.x,object.start.y,object.start.z);
        for(unsigned int i=0; i<object.control.size(); i++){
            glVertex3d(object.control.at(i).x,object.control.at(i).y,object.control.at(i).z);
        }
        glVertex3d(object.end.x,object.end.y,object.end.z);
        glEnd();
    }

    //cad display stored data
    for(unsigned int i=0; i<cad.size(); i++){
        glLineWidth(cad.at(i).linewidth);
        if(cad.at(i).type=="line"){
            //glLineWidth(1);
            glColor4d(cad.at(i).red,cad.at(i).green,cad.at(i).blue,cad.at(i).alpha);
            glBegin(GL_LINES);
            glVertex3d(cad.at(i).start.x,cad.at(i).start.y,cad.at(i).start.z);
            glVertex3d(cad.at(i).end.x,cad.at(i).end.y,cad.at(i).end.z);
            glEnd();
        } else if(cad.at(i).type=="linestrip"){
            //glLineWidth(1);
            glColor4d(cad.at(i).red,cad.at(i).green,cad.at(i).blue,cad.at(i).alpha);
            glBegin(GL_LINE_STRIP);
            glVertex3d(cad.at(i).start.x,cad.at(i).start.y,cad.at(i).start.z);
            for(unsigned int j=0; j<cad.at(i).control.size(); j++){
                glVertex3d(cad.at(i).control.at(j).x,cad.at(i).control.at(j).y,cad.at(i).control.at(j).z);
            }
            glVertex3d(cad.at(i).end.x,cad.at(i).end.y,cad.at(i).end.z);
            glEnd();
        } else if(cad.at(i).type=="spline"){
            std::vector<std::vector<double>> C; //2d array
            C.push_back({cad.at(i).start.x, cad.at(i).start.y}); //spline startpoint
            for(unsigned int j=0; j<cad.at(i).control.size(); j++){
                C.push_back({cad.at(i).control.at(j).x, cad.at(i).control.at(j).y}); //spline controlpoints
            }
            C.push_back({cad.at(i).end.x, cad.at(i).end.y}); //spline endpoint
            draw_spline(C,cad.at(i).red,cad.at(i).green,cad.at(i).blue,cad.at(i).alpha);
            C.clear();
        } else if(cad.at(i).type=="arc"){
            //draw_3p_arc_by_angle(cad.at(i).center.x,cad.at(i).center.y,  cad.at(i).deg_angle_start,cad.at(i).deg_angle_end,cad.at(i).radius,cad.at(i).red,cad.at(i).green,cad.at(i).blue,cad.at(i).alpha);
            draw_3p_arc_by_point(cad.at(i).start.x, cad.at(i).start.y, cad.at(i).control.front().x, cad.at(i).control.front().y, cad.at(i).end.x, cad.at(i).end.y, cad.at(i).red,cad.at(i).green,cad.at(i).blue,cad.at(i).alpha);
        } else if(cad.at(i).type=="circle"){
            draw_2p_circle(cad.at(i).center.x,cad.at(i).center.y,cad.at(i).end.x,cad.at(i).end.y,cad.at(i).red,cad.at(i).green,cad.at(i).blue,cad.at(i).alpha);
        } else if(cad.at(i).type=="ellipse"){
            draw_3p_ellipse(cad.at(i).center.x,cad.at(i).center.y,cad.at(i).end.x,cad.at(i).end.y,cad.at(i).input.x,cad.at(i).input.y,cad.at(i).pi_angle_start,cad.at(i).pi_angle_end, cad.at(i).ratio,
                            cad.at(i).red,cad.at(i).green,cad.at(i).blue,cad.at(i).alpha);
        } else if(cad.at(i).type=="bezier"){
            draw_4p_bezier(cad.at(i).start.x,cad.at(i).start.y,cad.at(i).start.z,cad.at(i).control.front().x,cad.at(i).control.front().y,cad.at(i).control.front().z,cad.at(i).control.back().x,cad.at(i).control.back().y,
                           cad.at(i).control.back().z,cad.at(i).end.x,cad.at(i).end.y,cad.at(i).end.z,cad.at(i).red,cad.at(i).green,cad.at(i).blue,cad.at(i).alpha);
        } else if(cad.at(i).type=="rectangular"){
            //glLineWidth(1);
            glColor4d(cad.at(i).red,cad.at(i).green,cad.at(i).blue,cad.at(i).alpha);
            glBegin(GL_LINE_STRIP);
            glVertex3d(cad.at(i).start.x,cad.at(i).start.y,cad.at(i).start.z); //lower left
            glVertex3d(cad.at(i).control.at(0).x,cad.at(i).control.at(0).y,cad.at(i).control.at(0).z); //upper left
            glVertex3d(cad.at(i).control.at(1).x,cad.at(i).control.at(1).y,cad.at(i).control.at(1).z); //upper right
            glVertex3d(cad.at(i).control.at(2).x,cad.at(i).control.at(2).y,cad.at(i).control.at(2).z); //lower right
            glVertex3d(cad.at(i).end.x,cad.at(i).end.y,cad.at(i).end.z); //back to startpoint
            glEnd();
        }
    }


    std::vector<OBJECT>::iterator it1;
    //cam display stored data
    for(it1=cam.begin(); it1<cam.end(); it1++){
        glLineWidth(it1->linewidth);
        if(it1->type=="line"){
            //glLineWidth(it1->linewidth);
            glColor4d(it1->red,it1->green,it1->blue,it1->alpha);
            glBegin(GL_LINES);
            glVertex3d(it1->start.x,it1->start.y,it1->start.z);
            glVertex3d(it1->end.x,it1->end.y,it1->end.z);
            glEnd();
        } else if(it1->type=="arc"){
            //draw_3p_arc_by_angle(it1->center.x,it1->center.y, it1->deg_angle_start,it1->deg_angle_end,it1->radius,it1->red,it1->green,it1->blue,it1->alpha);
            draw_3p_arc_by_point(it1->start.x, it1->start.y, it1->control.front().x, it1->control.front().y, it1->end.x, it1->end.y, it1->red,it1->green,it1->blue,it1->alpha);
        } else if(it1->type=="circle"){
            draw_2p_circle(it1->center.x,it1->center.y,it1->end.x,it1->end.y,it1->red,it1->green,it1->blue,it1->alpha);
        } else if(it1->type=="linestrip"){
            //glLineWidth(1);
            glColor4d(it1->red,it1->green,it1->blue,it1->alpha);
            glBegin(GL_LINE_STRIP);
            glVertex3d(it1->start.x,it1->start.y,it1->start.z);
            for(unsigned int j=0; j<it1->control.size(); j++){
                glVertex3d(it1->control.at(j).x,it1->control.at(j).y,it1->control.at(j).z);
            }
            glVertex3d(it1->end.x,it1->end.y,it1->end.z);
            glEnd();

        } else if(it1->type=="spline"){
            std::vector<std::vector<double>> C; //2d array
            C.push_back({it1->start.x, it1->start.y}); //spline startpoint
            for(unsigned int j=0; j<it1->control.size(); j++){
                C.push_back({it1->control.at(j).x, it1->control.at(j).y}); //spline controlpoints
            }
            C.push_back({it1->end.x, it1->end.y}); //spline endpoint
            draw_spline(C,it1->red,it1->green,it1->blue,it1->alpha);
            C.clear();

        } else if(it1->type=="ellipse"){
            draw_3p_ellipse(it1->center.x,it1->center.y,it1->end.x,it1->end.y,it1->input.x,it1->input.y,it1->pi_angle_start,it1->pi_angle_end, it1->ratio,
                            it1->red,it1->green,it1->blue,it1->alpha);
        } else if(it1->type=="bezier"){
            draw_4p_bezier(it1->start.x,it1->start.y,it1->start.z,it1->control.front().x,it1->control.front().y,it1->control.front().z,it1->control.back().x,it1->control.back().y,
                           it1->control.back().z,it1->end.x,it1->end.y,it1->end.z,it1->red,it1->green,it1->blue,it1->alpha);
        } else if(it1->type=="rectangular"){
            //glLineWidth(1);
            glColor4d(it1->red,it1->green,it1->blue,it1->alpha);
            glBegin(GL_LINE_STRIP);
            glVertex3d(it1->start.x,it1->start.y,it1->start.z); //lower left
            glVertex3d(it1->control.at(0).x,it1->control.at(0).y,it1->control.at(0).z); //upper left
            glVertex3d(it1->control.at(1).x,it1->control.at(1).y,it1->control.at(1).z); //upper right
            glVertex3d(it1->control.at(2).x,it1->control.at(2).y,it1->control.at(2).z); //lower right
            glVertex3d(it1->end.x,it1->end.y,it1->end.z); //back to startpoint
            glEnd();
        }
    }
}

void draw_2p_circle(double xcen, double ycen, double xend, double yend, double red, double green, double blue, double alpha){

    int segments = 25;
    double radius = sqrt(pow(xend-xcen,2)+pow(yend-ycen,2));
    glColor4d(red,green,blue,alpha);
    glBegin(GL_LINE_LOOP);
    for (int j = 0; j<segments; j++)
    {
        double theta = 2.0 * M_PI * int(j) / int(segments);
        double x = radius * cos(theta);
        double y = radius * sin(theta);
        glVertex2d(x + xcen, y + ycen);
    }
    glEnd();
}

void draw_3p_ellipse(double xcen, double ycen, double xend, double yend, double xinput, double yinput, double pi_angle_start, double pi_angle_end, double ratio, double red, double green, double blue, double alpha){

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

    glColor4d(red,green,blue,alpha);
    glBegin(GL_LINE_STRIP);
    for(double j = pi_angle_start; j<pi_angle_end; j+=0.01)
    {
        //non rotated ellipse..
        double non_rotated_x = /*center_x +*/ cos(j)*radius_x;
        double non_rotated_y = /*center_y +*/ sin(j)*radius_y;
        //glVertex2d(center_x + cos(j)*radius_x, center_y + sin(j)*radius_y);

        //rotated ellipse..
        double rotate_x = center_x  + cos(angle * M_PI / 180.0 )* non_rotated_x - sin(angle * M_PI / 180.0 ) * non_rotated_y;
        double rotate_y = center_y  + sin(angle * M_PI / 180.0 )* non_rotated_x + cos(angle * M_PI / 180.0 ) * non_rotated_y;
        glVertex2d(rotate_x, rotate_y);
    }
    glEnd();
}

void draw_3p_arc_by_angle(double xc, double yc, double deg_angle_start, double deg_angle_end, double radius, double red, double green, double blue, double alpha){

    //draw arc..
    double pi_angle_start = (deg_angle_start/360)*(2*M_PI); //start angle
    //if(end_angle==360)
    //{
    //    end_angle=0;   //autocad degree problem..
    //}
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

    glColor4d(red,green,blue,alpha);
    glBegin(GL_LINE_STRIP);//since the arc is not a closed curve, this is a strip now
    for(int ii = 0; ii < segments; ii++)
    {
        glVertex2d(x + xc, y + yc); //x center, y center..

        double tx = -y;
        double ty = x;

        x += tx * tangetial_factor;
        y += ty * tangetial_factor;

        x *= radial_factor;
        y *= radial_factor;
    } glEnd();
    glEnd();
}

void draw_3p_arc_by_point(double xs, double ys, double xcon, double ycon, double xe, double ye, double red, double green, double blue, double alpha){

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
    double deg_angle_start=0;
    double deg_angle_end=0;
    double radius=0;
    double x_center=0;
    double y_center=0;

    if (determinant==0) // The lines are parallel.
    {
        // lines are parallel
    }
    else  {
        x_center = (delta_x1*c0 - delta_x0*c1)/determinant;
        y_center = (delta_y0*c1 - delta_y1*c0)/determinant;
        double dx = x_center - xs;
        double dy = y_center - ys;
        radius = sqrt(dx * dx + dy * dy);

        //formula if arc is cw or ccw..
        double x1 = xs;
        double y1 = ys;
        double x2 = xe;
        double y2 = ye;
        double xp = xcon;
        double yp = ycon;
        double determinant = ((x2-x1)*(yp-y1))-((y2-y1)*(xp-x1));

        if(determinant > 0){
            deg_angle_start = 180/M_PI*atan2(ye-y_center, xe-x_center);    //start angle G2
            deg_angle_end = 180/M_PI*atan2(ys-y_center, xs-x_center);    //end angle G2
        }
        if(determinant < 0){
            deg_angle_start = 180/M_PI*atan2(ys-y_center, xs-x_center);    //start angle G3
            deg_angle_end  = 180/M_PI*atan2(ye-y_center, xe-x_center);    //end angle G3
        }
        if(determinant == 0){
            //draw straight line
        }
    }

    //draw arc..
    double pi_angle_start = (deg_angle_start/360)*(2*M_PI); //start angle
    //if(end_angle==360)
    //{
    //    end_angle=0;   //autocad degree problem..
    //}
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

    glColor4d(red,green,blue,alpha);
    glBegin(GL_LINE_STRIP);//since the arc is not a closed curve, this is a strip now
    for(int ii = 0; ii < segments; ii++)
    {
        glVertex2d(x + x_center, y + y_center); //x center, y center..

        double tx = -y;
        double ty = x;

        x += tx * tangetial_factor;
        y += ty * tangetial_factor;

        x *= radial_factor;
        y *= radial_factor;
    }
    glEnd();
}

void draw_spline(std::vector<std::vector<double>> C, double red, double green, double blue, double alpha){ //this is a "true" spline
    std::vector<POINT>::iterator it;
    //the forward and backward function has only to be recalculated when spline controlpoints are edited by user.
    std::vector<POINT> forward=construct_spline(C);
    std::reverse(C.begin(),C.end());
    std::vector<POINT> backward=construct_spline(C);
    std::reverse(backward.begin(),backward.end());
    std::vector<POINT> differce;
    POINT p;

    for(int i=0; i<forward.size(); i++){
        p.x=(forward.at(i).x+backward.at(i).x)/2;
        p.y=(forward.at(i).y+backward.at(i).y)/2;
        differce.push_back(p);
    }

    //glLineWidth(1);
    glColor4d(red,green,blue,alpha);
    glBegin(GL_LINE_STRIP);
    for(it=differce.begin(); it<differce.end(); it++){
        glVertex2d(it->x,it->y);
    }
    glEnd();
}

std::vector<POINT> construct_spline(std::vector<std::vector<double>> C){
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
        for(double t=0; t<1; t+=0.1)  //time 0-1 for each spline
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

void draw_4p_bezier(double xs, double ys, double zs, double xcon0, double ycon0, double zcon0, double xcon1, double ycon1, double zcon1, double xend, double yend, double zend, double red, double green, double blue, double alpha){
    //glLineWidth(1);
    glColor4d(red,green,blue,alpha);
    glBegin(GL_LINE_STRIP);
    for(double t=0; t<1; t+=0.01){

        //Calculate Point E, on the line AB
        double Ex = ((1-t) * xs) + (t * xcon0);
        double Ey = ((1-t) * ys) + (t * ycon0);
        double Ez = ((1-t) * zs) + (t * zcon0);

        //Calculate Point F, on the line BC
        double Fx = ((1-t) * xcon0) + (t * xcon1);
        double Fy = ((1-t) * ycon0) + (t * ycon1);
        double Fz = ((1-t) * zcon0) + (t * zcon1);

        //Calculate Point G, on the line CD
        double Gx = ((1-t) * xcon1) + (t * xend);
        double Gy = ((1-t) * ycon1) + (t * yend);
        double Gz = ((1-t) * zcon1) + (t * zend);

        //Calculate Point Q, on the line EF
        double Qx = ((1-t) * Ex) + (t * Fx);
        double Qy = ((1-t) * Ey) + (t * Fy);
        double Qz = ((1-t) * Ez) + (t * Fz);

        //Calculate Point R, on the line FG
        double Rx = ((1-t) * Fx) + (t * Gx);
        double Ry = ((1-t) * Fy) + (t * Gy);
        double Rz = ((1-t) * Fz) + (t * Gz);

        //Calculate Point P, on the line QR
        double Px = ((1-t) * Qx) + (t * Rx);
        double Py = ((1-t) * Qy) + (t * Ry);
        double Pz = ((1-t) * Qz) + (t * Rz);

        glVertex3d(Px, Py, Pz);  //draw linestrip..
    }
    glVertex3d(xend, yend, zend); //depending on resolution, now the curve is always fitted to the endpoint.
    glEnd();

    //preview points
//    glLineWidth(1);
//    glPointSize(5);
//    glColor4d(255,0,0,255);
//    glBegin(GL_POINTS);
//    glVertex2d(xs,ys);
//    glVertex2d(xcon0,ycon0);
//    glVertex2d(xcon1,ycon1);
//    glVertex2d(xend,yend);

    glEnd();
}

void preview_polygon(double xcen, double ycen, double zcen, double xend, double yend, double zend, int sides){

    if(sides<3){sides = 3;}
    bool even=0;
    double dist_x = xend - xcen;
    double dist_y = yend - ycen;
    double radius = sqrt(pow(dist_x,2) + pow(dist_y,2));
    double cake_angle = 360 / sides;
    double start_angle = atan2(ycen - yend, xcen - xend)* 180.0 / M_PI;
    double x0,y0,x1,y1;

    if (sides % 2 == 0){
        even = 1;
    } else {
        even = 0;
    }

    object.control.clear();

    if(even == 1){
        for(int i=0; i<sides; i++){
            if(i==0){
                x0 = xcen + (radius * cos ( start_angle * M_PI / 180.0 ));
                y0 = ycen + (radius * sin ( start_angle * M_PI / 180.0 ));
                x1 = xcen + (radius * cos ( (start_angle + cake_angle) * M_PI / 180.0 ));
                y1 = ycen + (radius * sin ( (start_angle + cake_angle) * M_PI / 180.0 ));
                start_angle += cake_angle;
                object.start.x=x0;
                object.start.y=y0;
                object.start.z=zend;
                object.control.push_back({x1,y1,zend});
            }
            if(i>0 && i<sides-1){
                x1 = xcen + (radius * cos ( (start_angle + cake_angle) * M_PI / 180.0 ));
                y1 = ycen + (radius * sin ( (start_angle + cake_angle) * M_PI / 180.0 ));
                start_angle += cake_angle;
                object.control.push_back({x1,y1,zend});
            }
            if(i==sides-1){
                x1 = xcen + (radius * cos ( (start_angle + cake_angle) * M_PI / 180.0 ));
                y1 = ycen + (radius * sin ( (start_angle + cake_angle) * M_PI / 180.0 ));
                object.end.x=x1;
                object.end.y=y1;
                object.end.z=zend;
            }
        }
    }

    if(even == 0){
        for(int i=0; i<sides; i++){
            if(i==0){
                x0 = xcen + (radius * cos ( (start_angle + (0.5*cake_angle)) * M_PI / 180.0 ));
                y0 = ycen + (radius * sin ( (start_angle + (0.5*cake_angle)) * M_PI / 180.0 ));
                x1 = xcen + (radius * cos ( ((start_angle + cake_angle) + (0.5*cake_angle)) * M_PI / 180.0 ));
                y1 = ycen + (radius * sin ( ((start_angle + cake_angle) + (0.5*cake_angle)) * M_PI / 180.0 ));
                start_angle += cake_angle;
                object.start.x=x0;
                object.start.y=y0;
                object.start.z=zend;
                object.control.push_back({x1,y1,zend});
            }
            if(i>0 && i<sides-1){
                x1 = xcen + (radius * cos ( ((start_angle + cake_angle) + (0.5*cake_angle)) * M_PI / 180.0 ));
                y1 = ycen + (radius * sin ( ((start_angle + cake_angle) + (0.5*cake_angle)) * M_PI / 180.0 ));
                start_angle += cake_angle;
                object.control.push_back({x1,y1,zend});
            }
            if(i==sides-1){
                x1 = xcen + (radius * cos ( ((start_angle + cake_angle) + (0.5*cake_angle)) * M_PI / 180.0 ));
                y1 = ycen + (radius * sin ( ((start_angle + cake_angle) + (0.5*cake_angle)) * M_PI / 180.0 ));
                object.end.x=x1;
                object.end.y=y1;
                object.end.z=zend;
            }
        }
    }
}




