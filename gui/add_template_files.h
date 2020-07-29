#ifndef ADD_TEMPLATE_FILES_H
#define ADD_TEMPLATE_FILES_H
#include <structs.h>
#include <variable.h>
#include <fstream>
#include <iostream>
#include <string.h>

void generate_template_files(){

    //write template file heads.txt
    std::ofstream myfile;
    myfile.open("heads.txt"); //ios app = append true

    myfile<<"-- head"<<std::endl;
    myfile<<"Z"<<std::endl;
    myfile<<"-- offset_x"<<std::endl;
    myfile<<0<<std::endl;
    myfile<<"-- offset_y"<<std::endl;
    myfile<<0<<std::endl;
    myfile<<"-- m_code_start"<<std::endl;
    myfile<<"M3"<<std::endl;
    myfile<<"-- m_code_stop"<<std::endl;
    myfile<<"M5"<<std::endl;
    myfile<<""<<std::endl;
    myfile<<"//expand heads"<<std::endl;
    myfile<<"-- end"<<std::endl;
    myfile<<""<<std::endl;
    myfile.close();

    //write template file tools.txt
    myfile.open("tools.txt"); //ios app = append true
    myfile.clear();
    myfile <<"tooltypes : Mill, Drill, Plasma-laser,Oxyfuel"<<std::endl;
    myfile<<"-- toolname"<<std::endl;
    myfile<<"Mill 6x50"<<std::endl;
    myfile<<"-- tooltype"<<std::endl;
    myfile<<"Mill"<<std::endl;
    myfile<<"-- head"<<std::endl;
    myfile<<"Z"<<std::endl;
    myfile<<"-- travelheight"<<std::endl;
    myfile<<10<<std::endl;
    myfile<<"-- feedrate/cutspeed"<<std::endl;
    myfile<<2500<<std::endl;
    myfile<<"-- workheight"<<std::endl;
    myfile<<-10<<std::endl;
    myfile<<"-- lead_in"<<std::endl;
    myfile<<0<<std::endl;
    myfile<<" -- lead_out"<<std::endl;
    myfile<<0<<std::endl;
    myfile<<"-- use_lead_in"<<std::endl;
    myfile<<0<<std::endl;
    myfile<<" -- use_lead_out"<<std::endl;
    myfile<<0<<std::endl;
    myfile<<"-- diameter"<<std::endl;
    myfile<<6<<std::endl;
    myfile<<"-- rpm/power"<<std::endl;
    myfile<<15000<<std::endl;
    myfile<<"-- drill_depth"<<std::endl;
    myfile<<0<<std::endl;
    myfile<<"-- use_retract"<<std::endl;
    myfile<<0<<std::endl;
    myfile<<"-- retract_distance"<<std::endl;
    myfile<<0<<std::endl;
    myfile<<"-- retract_times"<<std::endl;
    myfile<<0<<std::endl;
    myfile<<"-- plungeheight/pierceheight"<<std::endl;
    myfile<<2<<std::endl;
    myfile<<"-- plungespeed/piercespeed"<<std::endl;
    myfile<<500<<std::endl;
    myfile<<"-- spindledelay/piercedelay/preheat"<<std::endl;
    myfile<<0.5<<std::endl;
    myfile<<""<<std::endl;
    myfile<<"//expand tools"<<std::endl;
    myfile<<"-- end"<<std::endl;
    myfile<<""<<std::endl;
    myfile.close();

    //write template file gcode_settings.txt
    myfile.open("gcode_settings.txt"); //ios app = append true
    myfile<<"-- intro"<<std::endl;
    myfile<<"( --intro --)"<<std::endl;
    myfile<<"G21 (unit:mm)"<<std::endl;
    myfile<<"G40 (cutter compensation off)"<<std::endl;
    myfile<<"G80 (cancel canned cycle modal motion)"<<std::endl;
    myfile<<"G90  (absolute distance, no offsets)"<<std::endl;
    myfile<<"G64P0.01 (path following accuracy)"<<std::endl;
    myfile<<"F1 (initialize feedrate)"<<std::endl;
    myfile<<"S1 (initialize spindle speed/power)"<<std::endl;
    myfile<<" ( --end intro --)"<<std::endl;
    myfile<<"-- intro end"<<std::endl; //done to trigger end of multiline text input
    myfile<<"-- outtro"<<std::endl;
    myfile<<"(-- outtro --)"<<std::endl;
    myfile<<"M30"<<std::endl;
    myfile<<"(--end outtro--)"<<std::endl;
    myfile<<"-- outtro end"<<std::endl; //done to trigger end of multiline text input
    myfile<<"-- delay_m_code"<<std::endl;
    myfile<<"G4 P"<<std::endl;
    myfile<<"-- filepath"<<std::endl;
    myfile<<"currentpath"<<std::endl;
    myfile<<"-- file_extension"<<std::endl;
    myfile<<".ngc"<<std::endl;
    myfile<<"-- decimal_precision"<<std::endl;
    myfile<<"4"<<std::endl;
    myfile<<"-- use_linenumbers"<<std::endl;
    myfile<<"1"<<std::endl;
    myfile<<"-- linenumber_character"<<std::endl;
    myfile<<"N"<<std::endl;
    myfile<<"-- filename"<<std::endl;
    myfile<<"gcode_output"<<std::endl;
    myfile<<""<<std::endl;
    myfile.close();
}

#endif // ADD_TEMPLATE_FILES_H




































