//#include <string>
//#include <extern_var.h>
//#include <iostream>
//#include <fstream>
//#include <string.h>
//#include <structs.h>

////http://help.autodesk.com/view/ACD/2016/ENU/?guid=GUID-EDD54EAC-A339-4EBA-AEA6-EC8066505E2B

//void read_dimension(std::string filename){

//    bool trigger=0;
//    unsigned int x_defenition_line=0;
//    unsigned int y_defenition_line=0;
//    unsigned int z_defenition_line=0;
//    unsigned int x_dim_text_mid_line=0;
//    unsigned int y_dim_text_mid_line=0;
//    unsigned int z_dim_text_mid_line=0;
//    unsigned int x_dim_A_line=0;
//    unsigned int y_dim_A_line=0;
//    unsigned int z_dim_A_line=0;
//    unsigned int x_dim_B_line=0;
//    unsigned int y_dim_B_line=0;
//    unsigned int z_dim_B_line=0;
//    unsigned int type_line=0;
//    unsigned int attachment_point_line=0;
//    unsigned int text_spacing_line=0;
//    unsigned int measurement_line=0;
//    unsigned int dimension_text_overide_line=0;

//    unsigned int linenumber=0;
//    char count[100]; //line buffer
//    FILE *file=fopen(filename.c_str(), "r"); //convert std::string to std::char

//    while(file && fgets(count, sizeof(count), file)){
//        linenumber++;

//        if(strstr(count,"AcDbDimension")){
//            //std::cout<<"dimension found at:"<<linenumber<<std::endl;
//            trigger=1;
//            object.type="dimension";
//        }

//        //todo

//        //10,20,30 = point located at the dimension arrow
//        if(trigger && strstr(count," 10")){
//            //cout<<"10 found at:"<<linenumber<< endl;
//            x_defenition_line=linenumber+1;
//        }
//        if(trigger && linenumber==x_defenition_line){
//            //std::cout<<"xdefenition:"<<count<< std::endl;
//            dimension.xdefenition=std::stod(count);
//        }

//        if(trigger && strstr(count," 20")){
//            //cout<<"20 found at:"<<linenumber<< endl;
//            y_defenition_line=linenumber+1;
//        }
//        if(trigger && linenumber==y_defenition_line){
//            //std::cout<<"ydefenition:"<<count<< std::endl;
//            dimension.ydefenition=std::stod(count);
//        }

//        if(trigger && strstr(count," 30")){
//            //cout<<"30 found at:"<<linenumber<< endl;
//            z_defenition_line=linenumber+1;
//        }
//        if(trigger && linenumber==z_defenition_line){
//            //std::cout<<"zdefenition:"<<count<< std::endl;
//            dimension.zdefenition=std::stod(count);
//        }

//        //11,21,31 = point located at the dimension text midpoint
//        if(trigger && strstr(count," 11")){
//            //cout<<"11 found at:"<<linenumber<< endl;
//            x_dim_text_mid_line=linenumber+1;
//        }
//        if(trigger && linenumber==x_dim_text_mid_line){
//            //std::cout<<"x_mid_dim_text_line;:"<<count<< std::endl;
//            dimension.xdimtextmid=std::stod(count);
//        }

//        if(trigger && strstr(count," 21")){
//            //cout<<"21 found at:"<<linenumber<< endl;
//            y_dim_text_mid_line=linenumber+1;
//        }
//        if(trigger && linenumber==y_dim_text_mid_line){
//            //std::cout<<"y_mid_dim_text_line:"<<count<< std::endl;
//            dimension.ydimtextmid=std::stod(count);
//        }

//        if(trigger && strstr(count," 31")){
//            //cout<<"31 found at:"<<linenumber<< endl;
//            z_dim_text_mid_line=linenumber+1;
//        }
//        if(trigger && linenumber==z_dim_text_mid_line){
//            //std::cout<<"z_mid_dim_text_line:"<<count<< std::endl;
//            dimension.zdimtextmid=std::stod(count);
//        }

//        if(trigger && strstr(count," 70")){
//            //cout<<"70 found at:"<<linenumber<< endl;
//            type_line=linenumber+1;
//        }
//        if(trigger && linenumber==type_line){
//            //std::cout<<"type_line:"<<count<< std::endl;
//            dimension.type=std::stoi(count);
//        }

//        if(trigger && strstr(count," 71")){
//            //cout<<"71 found at:"<<linenumber<< endl;
//            attachment_point_line=linenumber+1;
//        }
//        if(trigger && linenumber==attachment_point_line){
//            //std::cout<<"attachment_point_line:"<<count<< std::endl;
//            dimension.type=std::stoi(count);
//        }

//        if(trigger && strstr(count," 41")){
//            //cout<<"41 found at:"<<linenumber<< endl;
//            text_spacing_line=linenumber+1;
//        }
//        if(trigger && linenumber==text_spacing_line){
//            //std::cout<<"text_spacing_line:"<<count<< std::endl;
//            dimension.text_line_spacing=std::stod(count);
//        }

//        if(trigger && strstr(count," 42")){
//            //cout<<"42 found at:"<<linenumber<< endl;
//            measurement_line=linenumber+1;
//        }
//        if(trigger && linenumber==measurement_line){
//            //std::cout<<"measurement_line:"<<count<< std::endl;
//            dimension.measurement=std::stod(count);
//        }

//        //13,23,33 = dimension measure point A
//        if(trigger && strstr(count," 13")){
//            //cout<<"13 found at:"<<linenumber<< endl;
//            x_dim_A_line=linenumber+1;
//        }
//        if(trigger && linenumber==x_dim_A_line){
//            //std::cout<<"x_dim_A_line:"<<count<< std::endl;
//            dimension.xmeasureA=std::stod(count);
//        }

//        if(trigger && strstr(count," 23")){
//            //cout<<"23 found at:"<<linenumber<< endl;
//            y_dim_A_line=linenumber+1;
//        }
//        if(trigger && linenumber==y_dim_A_line){
//            //std::cout<<"y_dim_A_line:"<<count<< std::endl;
//            dimension.ymeasureA=std::stod(count);
//        }

//        if(trigger && strstr(count," 33")){
//            //cout<<"33 found at:"<<linenumber<< endl;
//            z_dim_A_line=linenumber+1;
//        }
//        if(trigger && linenumber==z_dim_A_line){
//            //std::cout<<"z_dim_A_line:"<<count<< std::endl;
//            dimension.zmeasureA=std::stod(count);
//        }

//        //14,24,34 = dimension measure point B
//        if(trigger && strstr(count," 14")){
//            //cout<<"14 found at:"<<linenumber<< endl;
//            x_dim_B_line=linenumber+1;
//        }
//        if(trigger && linenumber==x_dim_B_line){
//            //std::cout<<"x_dim_B_line:"<<count<< std::endl;
//            dimension.xmeasureB=std::stod(count);
//        }

//        if(trigger && strstr(count," 24")){
//            //cout<<"24 found at:"<<linenumber<< endl;
//            y_dim_B_line=linenumber+1;
//        }
//        if(trigger && linenumber==y_dim_B_line){
//            //std::cout<<"y_dim_B_line:"<<count<< std::endl;
//            dimension.ymeasureB=std::stod(count);
//        }

//        if(trigger && strstr(count," 34")){
//            //cout<<"34 found at:"<<linenumber<< endl;
//            z_dim_B_line=linenumber+1;
//        }
//        if(trigger && linenumber==z_dim_B_line){
//            //std::cout<<"z_dim_B_line:"<<count<< std::endl;
//            dimension.zmeasureB=std::stod(count);
//        }

//        if(trigger && strstr(count," 1")){
//            //cout<<"1 found at:"<<linenumber<< endl;
//            dimension_text_overide_line=linenumber+1;
//        }
//        if(trigger && linenumber==dimension_text_overide_line){
//            //std::cout<<"dimension_text_overide_line:"<<count<< std::endl;
//            dimension.dimension_text_override=count;
//        }

//        if(trigger && strstr(count,"102")){ //End of group
//            dimensions.push_back(dimension);
//            trigger=0;
//        }
//    }
//}

