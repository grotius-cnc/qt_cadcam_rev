#ifndef LOAD_GCODE_SETTINGS_H
#define LOAD_GCODE_SETTINGS_H
#include <structs.h>
#include <variable.h>
#include <fstream>
#include <iostream>
#include <string.h>

void load_gcode_settings(){

    unsigned int intro_line=0;
    unsigned int outtro_line=0;
    unsigned int delay_line=0;
    unsigned int filepath_line=0;
    unsigned int file_extension_line=0;
    unsigned int decimal_precision_line=0;
    unsigned int use_linenumbers_line=0;
    unsigned int linenumber_character_line=0;
    unsigned int filename_line=0;
    bool intro_trigger=0;
    bool outtro_trigger=0;

unsigned int linenumber=0;
char count[200];
FILE *file=fopen("gcode_settings.txt", "r"); //convert std::string to std::char

while(file && fgets(count, sizeof(count), file)){
    linenumber++;

    if(strstr(count,"-- intro")){
        intro_line=linenumber+1;
         intro_trigger=1;
    }
    if(linenumber==intro_line && intro_trigger){
        gcode_settings.intro+=count;
        intro_line++;
    }

    if(strstr(count,"-- outtro")){
        outtro_line=linenumber+1;
        outtro_trigger=1;
    }
    if(linenumber==outtro_line && outtro_trigger){
        gcode_settings.outtro+=count;
        outtro_line++;
    }

    if(strstr(count,"-- delay_m_code")){
        delay_line=linenumber+1;
    }
    if(linenumber==delay_line){
        gcode_settings.delay_m_code=count;
    }

    if(strstr(count,"-- filepath")){
        filepath_line=linenumber+1;
    }
    if(linenumber== filepath_line){
        gcode_settings.filepath=count;
    }


    if(strstr(count,"-- file_extension")){
        file_extension_line=linenumber+1;
    }
    if(linenumber== file_extension_line){
        gcode_settings.file_extension=count;
    }

    if(strstr(count,"-- decimal_precision")){
        decimal_precision_line=linenumber+1;
    }
    if(linenumber== decimal_precision_line){
        gcode_settings.decimal_precision=std::stoi(count);
    }

    if(strstr(count,"-- use_linenumbers")){
        use_linenumbers_line=linenumber+1;
    }
    if(linenumber== use_linenumbers_line){
        gcode_settings.use_linenumbers=std::stoi(count);
    }

    if(strstr(count,"-- linenumber_character")){
        linenumber_character_line=linenumber+1;
    }
    if(linenumber== linenumber_character_line){
        gcode_settings.linenumber_character=count;
    }

    if(strstr(count,"-- filename")){
        filename_line=linenumber+1;
    }
    if(linenumber== filename_line){
        gcode_settings.filename=count;
    }

    if(strstr(count,"-- intro end")){
        intro_trigger=0;
    }

    if(strstr(count,"-- outtro end")){
        outtro_trigger=0;
    }
}
fcloseall();
}

#endif // LOAD_GCODE_SETTINGS_H
