#ifndef LOAD_TOOLS_H
#define LOAD_TOOLS_H
#include <structs.h>
#include <variable.h>
#include <fstream>
#include <iostream>
#include <string.h>

void load_tools_from_file(){

    int toolname_line=0;
    int tooltype_line=0;
    int head_line=0;
    int travelheight_line=0;
    int workheight_line=0;
    int feedrate_line=0;
    int lead_in_line=0;
    int lead_out_line=0;
    int retract_distance_line=0;
    int retract_times_line=0;
    int use_lead_in_line=0;
    int use_lead_out_line=0;
    int use_retract_line=0;
    int diameter_line=0;
    int rpm_line=0;
    int plungeheight_line=0;
    int plungespeed_line=0;
    int spindledelay_line=0;

    int end_line=0;

    int platenumber=0,bid=0;
    double translation_x=0, translation_y=0, rotation=0;

    tools.clear();

    unsigned int linenumber=0;
    char count[200];
    FILE *file=fopen("tools.txt", "r"); //convert std::string to std::char

    while(file && fgets(count, sizeof(count), file)){
        linenumber++;

        if(strstr(count,"-- toolname")){
            toolname_line=linenumber+1;
        }
        if(linenumber==toolname_line){
            tool.toolname=count;
           //std::cout<<"toolname: "<<count<<std::endl;
        }

        if(strstr(count,"-- tooltype")){
            tooltype_line=linenumber+1;
        }
        if(linenumber==tooltype_line){
            tool.tooltype=count;
            //std::cout<<"tooltype: "<<count<<std::endl;
        }

        if(strstr(count,"-- head")){
            head_line=linenumber+1;
        }
        if(linenumber==head_line){
            tool.head=count;
            //std::cout<<"head: "<<count<<std::endl;
        }

        if(strstr(count,"-- travelheight")){
            travelheight_line=linenumber+1;
        }
        if(linenumber==travelheight_line){
            tool.travelheight=std::stod(count);
            //std::cout<<"travelheight: "<<std::stod(count)<<std::endl;
        }

        if(strstr(count,"-- workheight")){
            workheight_line=linenumber+1;
        }
        if(linenumber==workheight_line){
            tool.workheight=std::stod(count);
            //std::cout<<"workheight: "<<std::stod(count)<<std::endl;
        }

        if(strstr(count,"-- feedrate/cutspeed")){
            feedrate_line=linenumber+1;
        }
        if(linenumber==feedrate_line){
            tool.feedrate=std::stod(count);
            //std::cout<<"feedrate: "<<std::stod(count)<<std::endl;
        }

        if(strstr(count,"-- lead_in")){
            lead_in_line=linenumber+1;
        }
        if(linenumber==lead_in_line){
            tool.lead_in=std::stod(count);
            //std::cout<<"lead_in: "<<std::stod(count)<<std::endl;
        }

        if(strstr(count,"-- lead_out")){
            lead_in_line=linenumber+1;
        }
        if(linenumber==lead_out_line){
            tool.lead_out=std::stod(count);
            //std::cout<<"lead_in: "<<std::stod(count)<<std::endl;
        }

        if(strstr(count,"-- retract_distance")){
            retract_distance_line=linenumber+1;
        }
        if(linenumber==retract_distance_line){
            tool.retract_distance=std::stod(count);
            //std::cout<<"retract_distance: "<<std::stod(count)<<std::endl;
        }

        if(strstr(count,"-- retract_times")){
            retract_times_line=linenumber+1;
        }
        if(linenumber==retract_times_line){
            tool.retract_times=std::stoi(count);
            //std::cout<<"retract_times: "<<std::stoi(count)<<std::endl;
        }

        if(strstr(count,"-- use_retract")){
            use_retract_line=linenumber+1;
        }
        if(linenumber==use_retract_line){
            tool.use_retract=std::stoi(count);
            //std::cout<<"use_retract: "<<std::stoi(count)<<std::endl;
        }

        if(strstr(count,"-- use_lead_in")){
            use_lead_in_line=linenumber+1;
        }
        if(linenumber==use_lead_in_line){
            tool.use_lead_in=std::stoi(count);
            //std::cout<<"use_lead_in: "<<std::stoi(count)<<std::endl;
        }

        if(strstr(count,"-- use_lead_out")){
            use_lead_out_line=linenumber+1;
        }
        if(linenumber==use_lead_out_line){
            tool.use_lead_out=std::stoi(count);
            //std::cout<<"use_lead_out: "<<std::stoi(count)<<std::endl;
        }

        if(strstr(count,"-- diameter")){
            diameter_line=linenumber+1;
        }
        if(linenumber==diameter_line){
            tool.diameter=std::stod(count);
            //std::cout<<"diameter: "<<std::stod(count)<<std::endl;
        }

        if(strstr(count,"-- rpm/power")){
            rpm_line=linenumber+1;
        }
        if(linenumber==rpm_line){
            tool.rpm=std::stod(count);
            //std::cout<<"rpm: "<<std::stod(count)<<std::endl;
        }

        if(strstr(count,"-- plungeheight/pierceheight")){
            plungeheight_line=linenumber+1;
        }
        if(linenumber==plungeheight_line){
            tool.plungeheight=std::stod(count);
            //std::cout<<"plungeheight: "<<std::stod(count)<<std::endl;
        }

        if(strstr(count,"-- plungespeed/piercespeed")){
            plungespeed_line=linenumber+1;
        }
        if(linenumber==plungespeed_line){
            tool.plungespeed=std::stod(count);
            //std::cout<<"plungespeed: "<<std::stod(count)<<std::endl;
        }

        if(strstr(count,"-- spindledelay/piercedelay/preheat")){
            spindledelay_line=linenumber+1;
        }
        if(linenumber==spindledelay_line){
            tool.delay=std::stod(count);
            //std::cout<<"spindledelay: "<<std::stod(count)<<std::endl;
        }

        if(strstr(count,"-- end")){
            end_line=linenumber+1;
        }
        if(linenumber==end_line){
            tools.push_back(tool);
        }
    }
    fcloseall();
}

#endif // LOAD_TOOLS_H
