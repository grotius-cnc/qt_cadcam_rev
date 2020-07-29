#ifndef LOAD_HEADS_H
#define LOAD_HEADS_H
#include <structs.h>
#include <variable.h>
#include <fstream>
#include <iostream>
#include <string.h>

void load_heads_from_file(){

    int axis_line=0;
    int offset_x_line=0;
    int offset_y_line=0;
    int m_code_start_line=0;
    int m_code_stop_line=0;

    heads.clear();

    unsigned int linenumber=0;
    char count[200];
    FILE *file=fopen("heads.txt", "r"); //convert std::string to std::char

    while(file && fgets(count, sizeof(count), file)){
        linenumber++;

        if(strstr(count,"-- head")){
            axis_line=linenumber+1;
        }
        if(linenumber==axis_line){
            head.axis=count;
            //std::cout<<"head: "<<head.axis<<std::endl;
        }

        if(strstr(count,"-- offset_x")){
            offset_x_line=linenumber+1;
        }
        if(linenumber==offset_x_line){
            head.offset_x=std::stod(count);
        }

        if(strstr(count,"-- offset_y")){
            offset_y_line=linenumber+1;
        }
        if(linenumber==offset_y_line){
            head.offset_y=std::stod(count);
        }

        if(strstr(count,"-- m_code_start")){
            m_code_start_line=linenumber+1;
        }
        if(linenumber== m_code_start_line){
            head.m_code_start=count;
        }

        if(strstr(count,"-- m_code_stop")){
            m_code_stop_line=linenumber+1;
        }
        if(linenumber== m_code_stop_line){
            head.m_code_stop=count;
        }

        if(strstr(count,"-- end")){
            heads.push_back(head);
        }
    }
    fcloseall();
}

#endif // LOAD_HEADS_H
