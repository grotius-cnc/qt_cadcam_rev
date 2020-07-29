#include <string>
#include <variable.h>
//#include <iostream>
//#include <fstream>
//#include <string.h>
//#include <structs.h>

//http://help.autodesk.com/view/ACD/2016/ENU/?guid=GUID-62E5383D-8A14-47B4-BFC4-35824CAE8363

//void read_text(std::string filename){

//    bool trigger=0;
//    unsigned int x_start_line=0;        //code 10
//    unsigned int y_start_line=0;        //code 20
//    unsigned int z_start_line=0;        //code 30
//    unsigned int text_height_line=0;    //code 40
//    unsigned int text_rotation_line=0;  //code 50
//    unsigned int text_string_line=0;    //code 1

//    unsigned int linenumber=0;
//    char count[100]; //line buffer
//    FILE *file=fopen(filename.c_str(), "r"); //convert std::string to std::char

//    while(file && fgets(count, sizeof(count), file)){
//        linenumber++;

//        if(strstr(count,"AcDbMText")){
//            //std::cout<<"text found at:"<<linenumber<<std::endl;
//            trigger=1;
//            object.type="text";
//        }

//        //10,20,30 = first aligment point
//        if(trigger && strstr(count," 10")){
//            //cout<<"10 found at:"<<linenumber<< endl;
//            x_start_line=linenumber+1;
//        }
//        if(trigger && linenumber==x_start_line){
//            std::cout<<"x_start_line:"<<count<< std::endl;
//            text.xstart=std::stod(count);
//        }

//        if(trigger && strstr(count," 20")){
//            //cout<<"20 found at:"<<linenumber<< endl;
//            y_start_line=linenumber+1;
//        }
//        if(trigger && linenumber==y_start_line){
//            std::cout<<"y_start_line:"<<count<< std::endl;
//            text.ystart=std::stod(count);
//        }

//        if(trigger && strstr(count," 30")){
//            //cout<<"30 found at:"<<linenumber<< endl;
//            z_start_line=linenumber+1;
//        }
//        if(trigger && linenumber==z_start_line){
//            std::cout<<"z_start_line:"<<count<< std::endl;
//            text.zstart=std::stod(count);
//        }

//        if(trigger && strstr(count," 40")){
//            //cout<<"40 found at:"<<linenumber<< endl;
//            text_height_line=linenumber+1;
//        }
//        if(trigger && linenumber==text_height_line){
//            std::cout<<"text_height_line:"<<count<< std::endl;
//            text.height=std::stod(count);
//        }

//        if(trigger && strstr(count," 50")){
//            //cout<<"50 found at:"<<linenumber<< endl;
//            text_rotation_line=linenumber+1;
//        }
//        if(trigger && linenumber==text_rotation_line){
//            std::cout<<"text_rotation_line:"<<count<< std::endl;
//            text.rotation=std::stod(count);
//        }

//        if(trigger && strstr(count,"  1")){
//            //cout<<"1 found at:"<<linenumber<< endl;
//            text_string_line=linenumber+1;
//        }
//        if(trigger && linenumber==text_string_line){
//            std::cout<<"text_string_line:"<<count<< std::endl;
//            text.string=count;
//        }

//        if(trigger && strstr(count," 73")){//Vertical text justification type used for end
//            texts.push_back(text);
//            trigger=0;
//        }

//    }
//}


