#include <mainwindow.h>
#include "ui_mainwindow.h"
#include <opengl.h>
#include <variable.h>
#include <structs.h>
#include <iostream>
#include <dxf/dxf_read_line.h>
#include <dxf/dxf_read_spline.h>
#include <dxf/dxf_read_circle.h>
#include <dxf/dxf_read_arc.h>
#include <dxf/dxf_read_ellipse.h>
#include <dxf/dxf_read_dimension.h>
#include <dxf/dxf_read_text.h>
#include <cam/cam_contours.h>
#include <cam/cam_dept_sequence.h>
#include <cam/cam_gcode.h>
#include <cam/cam_offset.h>
#include <lib/lib_offset.h>
#include <lib/lib_pocket.h>
#include <display/zoom_extend.h>
#include <cam/gravity_nester.h>

#include <gui/load_gcode_settings.h>
#include <gui/load_tools.h>
#include <gui/load_heads.h>
#include <gui/add_template_files.h>

GCODE gcode_settings;
TOOL tool;
std::vector<TOOL> tools;
HEAD head;
std::vector<HEAD> heads;
JOBS job;
std::vector<JOBS> jobs;

#define MyQLineEdit() << QLineEdit() << fixed << qSetRealNumberPrecision(3)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timerId = startTimer(200);

    //example how to connect to buttons:
    //connect(ui->toolButton_new, SIGNAL(clicked()),this, SLOT(on_toolButton_arc_pressed()));

    std::ifstream infile("tools.txt");
    if(infile.good()){
        //std::cout<<"tools.txt excists"<<std::endl;
        //keep the current settings file's as they are.
    } else {
         generate_template_files();
    }

    load_heads();
    load_tools();
    load_nest();
    load_gcode();
}

MainWindow::~MainWindow()
{
    killTimer(timerId);
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *)
{
    ui->lineEdit_mx->setText(QString::number(mx,'f',6));
    ui->lineEdit_my->setText(QString::number(my,'f',6));
    ui->lineEdit_message->setText(QString::fromStdString(message));

    if(update_flag){

        update_layers();
        update_flag=0;

    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int_key=event->key();
    //std::cout<<"key:"<<int_key<<std::endl;
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    int_key=-1;
}

void MainWindow::on_toolButton_open_pressed()
{
#define DXF_FILES "AC1027 Inkscape dxf R14 files, AC1021 Librecad dxf 2007 files(*.dxf)"
#define GRO_FILES "gro files (*.gro)"

    QFileDialog *dialog = new QFileDialog;
    dialog->resize(500,250);
    QString selectedFilter;

    QString filename_open = QFileDialog::getOpenFileName(
                dialog,
                "Open",
                QDir::currentPath(),
                DXF_FILES ";;" GRO_FILES,
                &selectedFilter,
                QFileDialog::DontUseNativeDialog); //this solves a dxf multiload problem

    if(selectedFilter == GRO_FILES){
        //(read_gro(filename_open));
    }
    if(selectedFilter == DXF_FILES){
        read_line(std::string(filename_open.toStdString()));
        read_spline(std::string(filename_open.toStdString()));
        read_circle(std::string(filename_open.toStdString()));
        read_arc(std::string(filename_open.toStdString()));
        read_ellipse(std::string(filename_open.toStdString()));
        //read_dimension(std::string(filename_open.toStdString()));
        //read_text(std::string(filename_open.toStdString()));
    }

    zoom_extend();
    update_layers();
}

void MainWindow::on_toolButton_line_pressed()
{
    mode="line";
}

void MainWindow::on_toolButton_linestrip_pressed()
{
    mode="linestrip";
}

void MainWindow::on_toolButton_rectangular_pressed()
{
    mode="rectangular";
}

void MainWindow::on_toolButton_circle_pressed()
{
    mode="circle";
}

void MainWindow::on_toolButton_ellipse_pressed()
{
    mode="ellipse";
}

void MainWindow::on_toolButton_arc_pressed()
{
    mode="arc";
}

void MainWindow::on_toolButton_spline_pressed()
{
    mode="spline";
}

void MainWindow::on_toolButton_bezier_pressed()
{
    mode="bezier";
}

void MainWindow::on_toolButton_polygon_pressed()
{
    mode="polygon";
}

void MainWindow::on_toolButton_new_pressed()
{
    cad.clear();
}

void MainWindow::on_pushButton_offset_pressed()
{
    find_contours();
    keep_parts_together();
    area();
    map_cw_ccw();

    //still need this for good cam arc output. must be done somewhere else in future.
    //add g2 or g3 to arcs
    for(int i=0; i<cad.size(); i++){
        if(cad.at(i).type=="arc"){
            double xp = cad.at(i).end.x;
            double yp = cad.at(i).end.y;
            double x1 = cad.at(i).start.x;
            double y1 = cad.at(i).start.y;
            double x2 = cad.at(i).control.at(0).x;
            double y2 = cad.at(i).control.at(0).y;
            double d = ((xp-x1)*(y2-y1))-((yp-y1)*(x2-x1));
            if(int(cad.at(i).red)==255 && int(cad.at(i).green)==255){
                if(d>0){//G2
                    cad.at(i).d=d;
                } else
                    if(d<0){//G3
                        cad.at(i).d=d;
                    }
            } else
                if(int(cad.at(i).red)==255 && int(cad.at(i).green)==0){
                    if(d>0){//G2
                        cad.at(i).d=d;
                    } else
                        if(d<0){//G3
                            cad.at(i).d=d;
                        }
                } else { cad.at(i).d=0;}
        }
    }

    std::string layer;
    layer=ui->comboBox_layer->currentText().toStdString();
    layer+='\n';
    double overlap=ui->doubleSpinBox_overlap->text().toDouble();

    //std::cout<<"combobox current layer: "<<layer<<std::endl;

    job.jobtype=ui->comboBox_offset_type->currentText().toStdString();
    job.tool=ui->comboBox_tool->currentText().toStdString();
    job.layer=ui->comboBox_layer->currentText().toStdString()+'\n';
    job.head=ui->comboBox_head_tools->currentText().toStdString();
    jobs.push_back(job);

    for(unsigned int i=0; i<tools.size(); i++){
        if(ui->comboBox_tool->currentText().toStdString()==tools.at(i).toolname){

            double offset=tools.at(i).diameter/2;

            if(ui->comboBox_offset_type->currentText()=="outside offset"){
                cam_normal_offset(abs(offset),layer,jobs.size()-1);
            }

            if(ui->comboBox_offset_type->currentText()=="inside offset"){
                cam_normal_offset(-abs(offset),layer,jobs.size()-1);
            }

            if(ui->comboBox_offset_type->currentText()=="no offset"){
                //pass objecs as they are
                no_offset(layer,jobs.size()-1);
            }

            if(ui->comboBox_offset_type->currentText()=="pocket"){
                cam_pocket_offset_with_islands(-abs(offset),layer,overlap,jobs.size()-1); //pocket overlap from 0 to 1
            }

            if(ui->comboBox_offset_type->currentText()=="drill hole"){
                cam_drill_offset(abs(offset),layer,jobs.size()-1);
            }
        }
    }

    ui->listWidget_jobs->addItem("Job: " + QString::number(jobs.size()-1) + ", " + ui->comboBox_offset_type->currentText() + ", " + ui->comboBox_tool->currentText() + ", " + ui->comboBox_layer->currentText());
}

void MainWindow::on_listWidget_jobs_currentTextChanged(const QString &currentText)
{
    std::cout<<"string:"<<currentText.toStdString()<<std::endl;
}

void MainWindow::update_layers()
{
    //populate combobox layer
    std::vector<std::string> layers;
    for(unsigned int i=0; i<cad.size(); i++){

        layers.push_back(cad.at(i).layer);
        //std::cout<<"layers: "<<layers.at(i)<<std::endl;
    }

    auto last=std::unique(layers.begin(),layers.end());
    layers.erase(last, layers.end());

    std::sort(layers.begin(),layers.end());
    last=std::unique(layers.begin(),layers.end());
    layers.erase(last, layers.end());

    ui->comboBox_layer->clear();
    for(std::string i : layers){

        QString str= QString::fromStdString(i);
        str.remove('\n');
        ui->comboBox_layer->addItem(str);
    }
    ui->comboBox_layer->addItem("blanco");
}

void MainWindow::on_pushButton_nest_pressed()
{
    //save a copy of current cad items
    restore_cad=cad;

    find_contours();
    keep_parts_together();
    area();
    map_cw_ccw();

    bin_start_x=ui->lineEdit_bin_start_x->text().toDouble();
    bin_start_y=ui->lineEdit_bin_start_y->text().toDouble();
    bin_widht=ui->lineEdit_bin_width->text().toDouble();
    bin_height=ui->lineEdit_bin_height->text().toDouble();
    bins=ui->lineEdit_bins->text().toInt();
    bin_space_x=ui->lineEdit_bin_spacing_x->text().toDouble();
    rotation_mode=ui->spinBox_rotation_mode->text().toUInt();
    use_holes=ui->comboBox_use_holes->currentText().toStdString();
    population_size=ui->lineEdit_population_size->text().toInt();
    mutation_rate=ui->lineEdit_mutation_rate->text().toInt();
    part_spacing=ui->lineEdit_part_spacing->text().toDouble();

    nest();
    java_nest();
    //set the ammount of bins
    std::string b = std::to_string(bins);
    ui->lineEdit_bins->setText(QString::fromStdString(b));
}

void MainWindow::on_pushButton_restore_pressed()
{
    cad=restore_cad;
}

void MainWindow::load_tools(){
    load_tools_from_file();

    //udate mainwindow's combobox_toolname.
    for(unsigned int i=0; i<tools.size(); i++){
        tools.at(i).toolname.erase(std::remove(tools.at(i).toolname.begin(), tools.at(i).toolname.end(), '\n'));
        tools.at(i).tooltype.erase(std::remove(tools.at(i).tooltype.begin(), tools.at(i).tooltype.end(), '\n'));
        tools.at(i).head.erase(std::remove(tools.at(i).head.begin(), tools.at(i).head.end(), '\n'));

        ui->comboBox_toolname->addItem(QString::fromStdString(tools.at(i).toolname));
    }

    ui->comboBox_tool->clear();
    for(unsigned int i=0; i<tools.size(); i++){
        ui->comboBox_tool->addItem(QString::fromStdString(tools.at(i).toolname));
    }

}

void MainWindow::load_nest(){

    int bin_start_x_line=0;
    int bin_start_y_line=0;
    int bin_width_line=0;
    int bin_height_line=0;
    int bins_line=0;
    int bin_space_x_line=0;
    int rotation_mode_line=0;
    int use_holes_line=0;
    int population_size_line=0;
    int mutation_rate_line=0;
    int part_spacing_line=0;

    unsigned int linenumber=0;
    char count[200];
    FILE *file=fopen("nest.txt", "r");

    while(file && fgets(count, sizeof(count), file)){
        linenumber++;

        if(strstr(count,"-- bin_start_x")){
            bin_start_x_line=linenumber+1;
        }
        if(linenumber==bin_start_x_line){
            ui->lineEdit_bin_start_x->setText(count);
        }

        if(strstr(count,"-- bin_start_y")){
            bin_start_y_line=linenumber+1;
        }
        if(linenumber==bin_start_y_line){
            ui->lineEdit_bin_start_y->setText(count);
        }

        if(strstr(count,"-- bin_width")){
            bin_width_line=linenumber+1;
        }
        if(linenumber==bin_width_line){
            ui->lineEdit_bin_width->setText(count);
        }

        if(strstr(count,"-- bin_height")){
            bin_height_line=linenumber+1;
        }
        if(linenumber==bin_height_line){
            ui->lineEdit_bin_height->setText(count);
        }

        if(strstr(count,"-- bins")){
            bins_line=linenumber+1;
        }
        if(linenumber==bins_line){
            ui->lineEdit_bins->setText(count);
        }

        if(strstr(count,"-- bin_space_x")){
            bin_space_x_line=linenumber+1;
        }
        if(linenumber==bin_space_x_line){
            ui->lineEdit_bin_spacing_x->setText(count);
        }

        if(strstr(count,"-- rotation_mode")){
            rotation_mode_line=linenumber+1;
        }
        if(linenumber==rotation_mode_line){
            ui->spinBox_rotation_mode->setValue(std::stoi(count));
        }

        if(strstr(count,"-- use_holes")){
            use_holes_line=linenumber+1;
        }
        if(linenumber==use_holes_line){
            ui->comboBox_use_holes->setCurrentIndex(std::stoi(count));
        }

        if(strstr(count,"-- population_size")){
            population_size_line=linenumber+1;
        }
        if(linenumber==population_size_line){
            ui->lineEdit_population_size->setText(count);
        }

        if(strstr(count,"-- mutation_rate")){
            mutation_rate_line=linenumber+1;
        }
        if(linenumber==mutation_rate_line){
            ui->lineEdit_mutation_rate->setText(count);
        }

        if(strstr(count,"-- part_spacing")){
            part_spacing_line=linenumber+1;
        }
        if(linenumber==part_spacing_line){
            ui->lineEdit_part_spacing->setText(count);
        }
    }
    fcloseall();
}

void MainWindow::on_comboBox_toolname_currentIndexChanged(int index)
{
    ui->lineEdit_toolname_edit->setText(QString::fromStdString(tools.at(index).toolname));
    ui->comboBox_tooltype->setCurrentText(QString::fromStdString(tools.at(index).tooltype));
    ui->comboBox_head_tools->setCurrentText(QString::fromStdString(tools.at(index).head));

    //travelheight
    ui->lineEdit_travelheight_mill->setText(QString::number(tools.at(index).travelheight));
    ui->lineEdit_travelheight_drill->setText(QString::number(tools.at(index).travelheight));
    ui->lineEdit_travelheight_plasma->setText(QString::number(tools.at(index).travelheight));
    ui->lineEdit_travelheight_oxyfuel->setText(QString::number(tools.at(index).travelheight));

    //workheight, cutheight
    ui->lineEdit_workheight_mill->setText(QString::number(tools.at(index).workheight));
    ui->lineEdit_workheight_drill->setText(QString::number(tools.at(index).workheight));
    ui->lineEdit_cutheight_plasma->setText(QString::number(tools.at(index).workheight));
    ui->lineEdit_cutheight_oxyfuel->setText(QString::number(tools.at(index).workheight));

    //feedrate, cutspeed
    ui->lineEdit_feedrate_mill->setText(QString::number(tools.at(index).feedrate));
    ui->lineEdit_cutspeed_plasma->setText(QString::number(tools.at(index).feedrate));
    ui->lineEdit_cutspeed_oxyfuel->setText(QString::number(tools.at(index).feedrate));

    //lead in, lead out
    ui->checkBox_use_lead_in_mill->setChecked(tools.at(index).use_lead_in);
    ui->checkBox_use_lead_out_mill->setChecked(tools.at(index).use_lead_out);
    ui->checkBox_use_lead_in_plasma->setChecked(tools.at(index).use_lead_in);
    ui->checkBox_use_lead_out_plasma->setChecked(tools.at(index).use_lead_out);
    ui->checkBox_use_lead_in_oxyfuel->setChecked(tools.at(index).use_lead_in);
    ui->checkBox_use_lead_out_oxyfuel->setChecked(tools.at(index).use_lead_out);

    //kerf, tooldiameter
    ui->lineEdit_tooldiameter_mill->setText(QString::number((tools.at(index).diameter)));
    ui->lineEdit_tooldiameter_drill->setText(QString::number((tools.at(index).diameter)));
    ui->lineEdit_kerf_plasma->setText(QString::number((tools.at(index).diameter)));
    ui->lineEdit_kerf_oxyfuel->setText(QString::number((tools.at(index).diameter)));

    //rpm, power
    ui->lineEdit_power_plasma->setText(QString::number((tools.at(index).rpm)));
    ui->lineEdit_rpm_mill->setText(QString::number((tools.at(index).rpm)));
    ui->lineEdit_rpm_drill->setText(QString::number((tools.at(index).rpm)));

    //plungeheight, pierceheight
    ui->lineEdit_plungeheight_mill->setText(QString::number((tools.at(index).plungeheight)));
    ui->lineEdit_plungeheight_drill->setText(QString::number((tools.at(index).plungeheight)));
    ui->lineEdit_pierceheight_plasma->setText(QString::number((tools.at(index).plungeheight)));
    ui->lineEdit_pierceheight_oxyfuel->setText(QString::number((tools.at(index).plungeheight)));

    //plungespeed, piercespeed
    ui->lineEdit_plungespeed_mill->setText(QString::number((tools.at(index).plungespeed)));
    ui->lineEdit_plungespeed_drill->setText(QString::number((tools.at(index).plungespeed)));
    ui->lineEdit_piercespeed_plasma->setText(QString::number((tools.at(index).plungespeed)));
    ui->lineEdit_piercespeed_oxyfuel->setText(QString::number((tools.at(index).plungespeed)));

    //spindledelay, piercedelay
    ui->lineEdit_spindledelay_mill->setText(QString::number((tools.at(index).delay)));
    ui->lineEdit_spindledelay_drill->setText(QString::number((tools.at(index).delay)));
    ui->lineEdit_piercedelay_plasma->setText(QString::number((tools.at(index).delay)));
    ui->lineEdit_piercedelay_oxyfuel->setText(QString::number((tools.at(index).delay)));

    //drill retract
    ui->checkBox_use_retract_drill->setChecked(tools.at(index).use_retract);
    ui->lineEdit_retract_distance_drill->setText(QString::number((tools.at(index).retract_distance)));
    ui->lineEdit_retract_times_drill->setText(QString::number((tools.at(index).retract_times)));
}

void MainWindow::on_pushButton_add_tool_pressed()
{
    ui->comboBox_toolname->addItem(ui->lineEdit_toolname_edit->text());

    TOOL t;
    t.toolname=ui->lineEdit_toolname_edit->text().toStdString();
    t.tooltype=ui->comboBox_tooltype->currentText().toStdString();
    t.head=ui->comboBox_head_tools->currentText().toStdString();

    if(ui->comboBox_tooltype->currentText()=="Mill"){
        t.tooltype="Mill";
        t.travelheight=ui->lineEdit_travelheight_mill->text().toDouble();
        t.workheight=ui->lineEdit_workheight_mill->text().toDouble();
        t.rpm=ui->lineEdit_rpm_mill->text().toDouble();
        t.delay=ui->lineEdit_spindledelay_mill->text().toDouble();
        t.lead_in=ui->lineEdit_lead_in_mill->text().toDouble();
        t.lead_out=ui->lineEdit_lead_out_mill->text().toDouble();
        t.diameter=ui->lineEdit_tooldiameter_mill->text().toDouble();
        t.feedrate=ui->lineEdit_feedrate_mill->text().toDouble();
        t.plungespeed=ui->lineEdit_plungespeed_mill->text().toDouble();
        t.plungeheight=ui->lineEdit_plungeheight_mill->text().toDouble();
        t.use_lead_in=ui->checkBox_use_lead_in_mill->checkState();
        t.use_lead_out=ui->checkBox_use_lead_out_mill->checkState();
        t.use_retract=0;
        t.retract_times=0;
        t.retract_distance=0;
    }

    if(ui->comboBox_tooltype->currentText()=="Drill"){
        t.tooltype="Drill";
        t.travelheight=ui->lineEdit_travelheight_drill->text().toDouble();
        t.workheight=ui->lineEdit_workheight_drill->text().toDouble();
        t.rpm=ui->lineEdit_rpm_drill->text().toDouble();
        t.delay=ui->lineEdit_spindledelay_drill->text().toDouble();
        t.lead_in=0;
        t.lead_out=0;
        t.diameter=ui->lineEdit_tooldiameter_drill->text().toDouble();
        t.feedrate=0;
        t.plungespeed=ui->lineEdit_plungespeed_drill->text().toDouble();
        t.plungeheight=ui->lineEdit_plungeheight_drill->text().toDouble();
        t.use_lead_in=0;
        t.use_lead_out=0;
        t.use_retract=ui->checkBox_use_retract_drill->checkState();
        t.retract_times=ui->lineEdit_retract_times_drill->text().toInt();
        t.retract_distance=ui->lineEdit_retract_distance_drill->text().toDouble();
    }

    if(ui->comboBox_tooltype->currentText()=="Plasma-laser"){
        t.tooltype="Plasma-laser";
        t.travelheight=ui->lineEdit_travelheight_plasma->text().toDouble();
        t.workheight=ui->lineEdit_cutheight_plasma->text().toDouble();
        t.rpm=ui->lineEdit_power_plasma->text().toDouble();
        t.delay=ui->lineEdit_piercedelay_plasma->text().toDouble();
        t.lead_in=ui->lineEdit_lead_in_plasma->text().toDouble();
        t.lead_out=ui->lineEdit_lead_out_plasma->text().toDouble();
        t.diameter=ui->lineEdit_kerf_plasma->text().toDouble();
        t.feedrate=ui->lineEdit_cutspeed_plasma->text().toDouble();
        t.plungespeed=ui->lineEdit_piercespeed_plasma->text().toDouble();
        t.plungeheight=ui->lineEdit_pierceheight_plasma->text().toDouble();
        t.use_lead_in=ui->checkBox_use_lead_in_plasma->checkState();
        t.use_lead_out=ui->checkBox_use_lead_out_plasma->checkState();
        t.use_retract=0;
        t.retract_times=0;
        t.retract_distance=0;
    }

    if(ui->comboBox_tooltype->currentText()=="Oxyfuel"){
        t.tooltype="Oxyfuel";
        t.travelheight=ui->lineEdit_travelheight_oxyfuel->text().toDouble();
        t.workheight=ui->lineEdit_cutheight_oxyfuel->text().toDouble();
        t.rpm=0;
        t.delay=ui->lineEdit_piercedelay_oxyfuel->text().toDouble();
        t.lead_in=ui->lineEdit_lead_in_oxyfuel->text().toDouble();
        t.lead_out=ui->lineEdit_lead_out_oxyfuel->text().toDouble();
        t.diameter=ui->lineEdit_kerf_oxyfuel->text().toDouble();
        t.feedrate=ui->lineEdit_cutspeed_oxyfuel->text().toDouble();
        t.plungespeed=ui->lineEdit_piercespeed_oxyfuel->text().toDouble();
        t.plungeheight=ui->lineEdit_pierceheight_oxyfuel->text().toDouble();
        t.use_lead_in=ui->checkBox_use_lead_in_oxyfuel->checkState();
        t.use_lead_out=ui->checkBox_use_lead_out_oxyfuel->checkState();
        t.use_retract=0;
        t.retract_times=0;
        t.retract_distance=0;
    }
    tools.push_back(t);

    ui->comboBox_tool->clear();
    for(unsigned int i=0; i<tools.size(); i++){
        ui->comboBox_tool->addItem(QString::fromStdString(tools.at(i).toolname));
    }
}

void MainWindow::on_pushButton_delete_tool_pressed()
{
    int index=ui->comboBox_toolname->currentIndex();
    if(ui->comboBox_toolname->count()>1){
        ui->comboBox_toolname->removeItem(index);
        tools.erase(tools.begin()+index);
    }

    ui->comboBox_tool->clear();
    for(unsigned int i=0; i<tools.size(); i++){
        ui->comboBox_tool->addItem(QString::fromStdString(tools.at(i).toolname));
    }
}

void MainWindow::on_pushButton_save_tools_pressed()
{
    int i = ui->comboBox_toolname->currentIndex();
    tools.at(i).toolname=ui->lineEdit_toolname_edit->text().toStdString();
    ui->comboBox_toolname->setItemText(i,ui->lineEdit_toolname_edit->text());
    tools.at(i).head=ui->comboBox_head_tools->currentText().toStdString();

    if(ui->comboBox_tooltype->currentText()=="Mill"){
        tools.at(i).tooltype="Mill";
        tools.at(i).travelheight=ui->lineEdit_travelheight_mill->text().toDouble();
        tools.at(i).workheight=ui->lineEdit_workheight_mill->text().toDouble();
        tools.at(i).rpm=ui->lineEdit_rpm_mill->text().toDouble();
        tools.at(i).delay=ui->lineEdit_spindledelay_mill->text().toDouble();
        tools.at(i).lead_in=ui->lineEdit_lead_in_mill->text().toDouble();
        tools.at(i).lead_out=ui->lineEdit_lead_out_mill->text().toDouble();
        tools.at(i).diameter=ui->lineEdit_tooldiameter_mill->text().toDouble();
        tools.at(i).feedrate=ui->lineEdit_feedrate_mill->text().toDouble();
        tools.at(i).plungespeed=ui->lineEdit_plungespeed_mill->text().toDouble();
        tools.at(i).plungeheight=ui->lineEdit_plungeheight_mill->text().toDouble();
        tools.at(i).use_lead_in=ui->checkBox_use_lead_in_mill->checkState();
        tools.at(i).use_lead_out=ui->checkBox_use_lead_out_mill->checkState();
        tools.at(i).use_retract=0;
        tools.at(i).retract_times=0;
        tools.at(i).retract_distance=0;
    }

    if(ui->comboBox_tooltype->currentText()=="Drill"){
        tools.at(i).tooltype="Drill";
        tools.at(i).travelheight=ui->lineEdit_travelheight_drill->text().toDouble();
        tools.at(i).workheight=ui->lineEdit_workheight_drill->text().toDouble();
        tools.at(i).rpm=ui->lineEdit_rpm_drill->text().toDouble();
        tools.at(i).delay=ui->lineEdit_spindledelay_drill->text().toDouble();
        tools.at(i).lead_in=0;
        tools.at(i).lead_out=0;
        tools.at(i).diameter=ui->lineEdit_tooldiameter_drill->text().toDouble();
        tools.at(i).feedrate=0;
        tools.at(i).plungespeed=ui->lineEdit_plungespeed_drill->text().toDouble();
        tools.at(i).plungeheight=ui->lineEdit_plungeheight_drill->text().toDouble();
        tools.at(i).use_lead_in=0;
        tools.at(i).use_lead_out=0;
        tools.at(i).use_retract=ui->checkBox_use_retract_drill->checkState();
        tools.at(i).retract_times=ui->lineEdit_retract_times_drill->text().toInt();
        tools.at(i).retract_distance=ui->lineEdit_retract_distance_drill->text().toDouble();
    }

    if(ui->comboBox_tooltype->currentText()=="Plasma-laser"){
        tools.at(i).tooltype="Plasma-laser";
        tools.at(i).travelheight=ui->lineEdit_travelheight_plasma->text().toDouble();
        tools.at(i).workheight=ui->lineEdit_cutheight_plasma->text().toDouble();
        tools.at(i).rpm=ui->lineEdit_power_plasma->text().toDouble();
        tools.at(i).delay=ui->lineEdit_piercedelay_plasma->text().toDouble();
        tools.at(i).lead_in=ui->lineEdit_lead_in_plasma->text().toDouble();
        tools.at(i).lead_out=ui->lineEdit_lead_out_plasma->text().toDouble();
        tools.at(i).diameter=ui->lineEdit_kerf_plasma->text().toDouble();
        tools.at(i).feedrate=ui->lineEdit_cutspeed_plasma->text().toDouble();
        tools.at(i).plungespeed=ui->lineEdit_piercespeed_plasma->text().toDouble();
        tools.at(i).plungeheight=ui->lineEdit_pierceheight_plasma->text().toDouble();
        tools.at(i).use_lead_in=ui->checkBox_use_lead_in_plasma->checkState();
        tools.at(i).use_lead_out=ui->checkBox_use_lead_out_plasma->checkState();
        tools.at(i).use_retract=0;
        tools.at(i).retract_times=0;
        tools.at(i).retract_distance=0;
    }

    if(ui->comboBox_tooltype->currentText()=="Oxyfuel"){
        tools.at(i).tooltype="Oxyfuel";
        tools.at(i).travelheight=ui->lineEdit_travelheight_oxyfuel->text().toDouble();
        tools.at(i).workheight=ui->lineEdit_cutheight_oxyfuel->text().toDouble();
        tools.at(i).rpm=0;
        tools.at(i).delay=ui->lineEdit_piercedelay_oxyfuel->text().toDouble();
        tools.at(i).lead_in=ui->lineEdit_lead_in_oxyfuel->text().toDouble();
        tools.at(i).lead_out=ui->lineEdit_lead_out_oxyfuel->text().toDouble();
        tools.at(i).diameter=ui->lineEdit_kerf_oxyfuel->text().toDouble();
        tools.at(i).feedrate=ui->lineEdit_cutspeed_oxyfuel->text().toDouble();
        tools.at(i).plungespeed=ui->lineEdit_piercespeed_oxyfuel->text().toDouble();
        tools.at(i).plungeheight=ui->lineEdit_pierceheight_oxyfuel->text().toDouble();
        tools.at(i).use_lead_in=ui->checkBox_use_lead_in_oxyfuel->checkState();
        tools.at(i).use_lead_out=ui->checkBox_use_lead_out_oxyfuel->checkState();
        tools.at(i).use_retract=0;
        tools.at(i).retract_times=0;
        tools.at(i).retract_distance=0;
    }

    //write to file tools.txt
    std::ofstream myfile;
    myfile.open("tools.txt"); //ios app = append true
    myfile.clear();


    myfile <<"tooltypes : Mill, Drill, Plasma-laser,Oxyfuel"<<std::endl;
    for(unsigned int i=0; i<tools.size(); i++){

        myfile<<"-- toolname"<<std::endl;
        myfile<<tools.at(i).toolname<<std::endl;
        myfile<<"-- tooltype"<<std::endl;
        myfile<<tools.at(i).tooltype<<std::endl;
        myfile<<"-- head"<<std::endl;
        myfile<<tools.at(i).head<<std::endl;
        myfile<<"-- travelheight"<<std::endl;
        myfile<<tools.at(i).travelheight<<std::endl;
        myfile<<"-- feedrate/cutspeed"<<std::endl;
        myfile<<tools.at(i).feedrate<<std::endl;
        myfile<<"-- workheight"<<std::endl;
        myfile<<tools.at(i).workheight<<std::endl;
        myfile<<"-- lead_in"<<std::endl;
        myfile<<tools.at(i).lead_in<<std::endl;
        myfile<<" -- lead_out"<<std::endl;
        myfile<<tools.at(i).lead_out<<std::endl;
        myfile<<"-- use_lead_in"<<std::endl;
        myfile<<tools.at(i).use_lead_in<<std::endl;
        myfile<<" -- use_lead_out"<<std::endl;
        myfile<<tools.at(i).use_lead_out<<std::endl;
        myfile<<"-- diameter"<<std::endl;
        myfile<<tools.at(i).diameter<<std::endl;
        myfile<<"-- rpm/power"<<std::endl;
        myfile<<tools.at(i).rpm<<std::endl;
        myfile<<"-- drill_depth"<<std::endl;
        myfile<<tools.at(i).drill_depth<<std::endl;
        myfile<<"-- use_retract"<<std::endl;
        myfile<<tools.at(i).use_retract<<std::endl;
        myfile<<"-- retract_distance"<<std::endl;
        myfile<<tools.at(i).retract_distance<<std::endl;
        myfile<<"-- retract_times"<<std::endl;
        myfile<<tools.at(i).retract_times<<std::endl;
        myfile<<"-- plungeheight/pierceheight"<<std::endl;
        myfile<<tools.at(i).plungeheight<<std::endl;
        myfile<<"-- plungespeed/piercespeed"<<std::endl;
        myfile<<tools.at(i).plungespeed<<std::endl;
        myfile<<"-- spindledelay/piercedelay/preheat"<<std::endl;
        myfile<<tools.at(i).delay<<std::endl;
        myfile<<""<<std::endl;
        myfile<<"//expand tools"<<std::endl;
        myfile<<"-- end"<<std::endl;
        myfile<<""<<std::endl;
    }
    myfile.close();

    ui->comboBox_tool->clear();
    for(unsigned int i=0; i<tools.size(); i++){
        ui->comboBox_tool->addItem(QString::fromStdString(tools.at(i).toolname));
    }
}

void MainWindow::on_pushButton_save_nest_settings_pressed()
{
    std::ofstream myfile;
    myfile.open("nest.txt");
    myfile.clear();
    myfile<<"-- bin_start_x"<<std::endl;
    myfile<<ui->lineEdit_bin_start_x->text().toDouble()<<std::endl;
    myfile<<"-- bin_start_y"<<std::endl;
    myfile<<ui->lineEdit_bin_start_y->text().toDouble()<<std::endl;
    myfile<<"-- bin_width"<<std::endl;
    myfile<<ui->lineEdit_bin_width->text().toDouble()<<std::endl;
    myfile<<"-- bin_height"<<std::endl;
    myfile<<ui->lineEdit_bin_height->text().toDouble()<<std::endl;
    myfile<<"-- bins"<<std::endl;
    myfile<<ui->lineEdit_bins->text().toDouble()<<std::endl;
    myfile<<"-- bin_space_x"<<std::endl;
    myfile<<ui->lineEdit_bin_spacing_x->text().toDouble()<<std::endl;
    myfile<<"-- rotation_mode"<<std::endl;
    myfile<<ui->spinBox_rotation_mode->text().toDouble()<<std::endl;
    myfile<<"-- use_holes"<<std::endl;
    myfile<<ui->comboBox_use_holes->currentIndex()<<std::endl;
    myfile<<"-- population_size"<<std::endl;
    myfile<<ui->lineEdit_population_size->text().toDouble()<<std::endl;
    myfile<<"-- mutation_rate"<<std::endl;
    myfile<<ui->lineEdit_mutation_rate->text().toDouble()<<std::endl;
    myfile<<"-- part_spacing"<<std::endl;
    myfile<<ui->lineEdit_part_spacing->text().toDouble()<<std::endl;
    myfile.close();
}

void MainWindow::load_heads(){
    load_heads_from_file();

    for(unsigned int i=0; i<heads.size(); i++){
        heads.at(i).axis.erase(std::remove(heads.at(i).axis.begin(), heads.at(i).axis.end(), '\n'));
        ui->comboBox_heads->addItem(QString::fromStdString(heads.at(i).axis));
        //std::cout<<"load heads: "<<heads.at(i).axis<<std::endl;
    }

    //repopulate heads.tools
    ui->comboBox_head_tools->clear();
    for(int i=0; i<ui->comboBox_heads->count(); i++){
        ui->comboBox_head_tools->addItem(ui->comboBox_heads->itemText(i));
    }
}

void MainWindow::on_pushButton_add_head_pressed()
{
    HEAD t;
    t.axis=ui->lineEdit_head_edit->text().toStdString();
    t.offset_x=ui->lineEdit_head_offset_x->text().toDouble();
    t.offset_y=ui->lineEdit_head_offset_y->text().toDouble();
    t.m_code_start=ui->lineEdit_m_code_start->text().toStdString();
    t.m_code_stop=ui->lineEdit_m_code_stop->text().toStdString();

    heads.push_back(t);

    ui->comboBox_heads->addItem(ui->lineEdit_head_edit->text());

    //repopulate heads.tools
    ui->comboBox_head_tools->clear();
    for(int i=0; i<ui->comboBox_heads->count(); i++){
        ui->comboBox_head_tools->addItem(ui->comboBox_heads->itemText(i));
    }
}

void MainWindow::on_pushButton_save_head_pressed()
{
    int i=ui->comboBox_heads->currentIndex();
    ui->comboBox_heads->setItemText(i,ui->lineEdit_head_edit->text());

    heads.at(i).axis=ui->lineEdit_head_edit->text().toStdString();
    heads.at(i).offset_x=ui->lineEdit_head_offset_x->text().toDouble();
    heads.at(i).offset_y=ui->lineEdit_head_offset_y->text().toDouble();
    heads.at(i).m_code_start=ui->lineEdit_m_code_start->text().toStdString();
    heads.at(i).m_code_stop=ui->lineEdit_m_code_stop->text().toStdString();

    std::ofstream myfile;
    myfile.open("heads.txt");
    myfile.clear();

    for(unsigned int i=0; i<heads.size(); i++){
        myfile<<"-- head"<<std::endl;
        myfile<<heads.at(i).axis<<std::endl;
        myfile<<"-- offset_x"<<std::endl;
        myfile<<heads.at(i).offset_x<<std::endl;
        myfile<<"-- offset_y"<<std::endl;
        myfile<<heads.at(i).offset_y<<std::endl;
        myfile<<"-- m_code_start"<<std::endl;
        myfile<<heads.at(i).m_code_start<<std::endl;
        myfile<<"-- m_code_stop"<<std::endl;
        myfile<<heads.at(i).m_code_stop<<std::endl;
        myfile<<""<<std::endl;
        myfile<<"//expand heads"<<std::endl;
        myfile<<"-- end"<<std::endl;
        myfile<<""<<std::endl;
    }
    myfile.close();

    //repopulate heads.tools
    ui->comboBox_head_tools->clear();
    for(int i=0; i<ui->comboBox_heads->count(); i++){
        ui->comboBox_head_tools->addItem(ui->comboBox_heads->itemText(i));
    }
}

void MainWindow::load_gcode(){

    load_gcode_settings();
    //set gui values:

    std::string str;
    str=gcode_settings.intro;
    str.erase(std::remove(str.end()-2, str.end(), '\n'),str.end());
    ui->plainTextEdit_gcode_intro->setPlainText(QString::fromStdString(str));

    str.clear();
    str=gcode_settings.outtro;
    str.erase(std::remove(str.end()-2, str.end(), '\n'),str.end());
    ui->plainTextEdit_gcode_outtro->setPlainText(QString::fromStdString(str));

    str.clear();
    str=gcode_settings.delay_m_code;
    str.erase(std::remove(str.begin(), str.end(), '\n'),str.end());
    ui->lineEdit_delay->setText(QString::fromStdString(str));

    str.clear();
    str=gcode_settings.filepath;
    str.erase(std::remove(str.begin(), str.end(), '\n'),str.end());
    ui->lineEdit_gcode_filepath->setText(QString::fromStdString(str));

    str.clear();
    str=gcode_settings.file_extension;
    str.erase(std::remove(str.begin(), str.end(), '\n'),str.end());

    ui->comboBox_gcode_file_extension->setCurrentText(QString::fromStdString(str));
    ui->spinBox_gcode_decimal_precision->setValue(gcode_settings.decimal_precision);
    ui->checkBox_use_line_numbers->setChecked(gcode_settings.use_linenumbers);

    str.clear();
    str=gcode_settings.linenumber_character;
    str.erase(std::remove(str.begin(), str.end(), '\n'),str.end());
    ui->lineEdit_linenumber_first_character->setText(QString::fromStdString(str));

    ui->lineEdit_gcode_filename->setText(QString::fromStdString(gcode_settings.filename));
}

void MainWindow::on_pushButton_save_gcode_settings_pressed()
{
    std::ofstream myfile;
    myfile.open("gcode_settings.txt");
    myfile.clear();

    QString a = ui->plainTextEdit_gcode_intro->toPlainText();
    std::string aa = a.toStdString();
    QString b = ui->plainTextEdit_gcode_outtro->toPlainText();
    std::string bb = b.toStdString();

        myfile<<"-- intro"<<std::endl;
        myfile<<aa<<std::endl;
        myfile<<"-- intro end"<<std::endl; //done to trigger end of multiline text input
        myfile<<"-- outtro"<<std::endl;
        myfile<<bb<<std::endl;
        myfile<<"-- outtro end"<<std::endl; //done to trigger end of multiline text input
        myfile<<"-- delay_m_code"<<std::endl;
        myfile<<ui->lineEdit_delay->text().toStdString()<<std::endl;
        myfile<<"-- filepath"<<std::endl;
        myfile<<ui->lineEdit_gcode_filepath->text().toStdString()<<std::endl;
        myfile<<"-- file_extension"<<std::endl;
        myfile<<ui->comboBox_gcode_file_extension->currentText().toStdString()<<std::endl;
        myfile<<"-- decimal_precision"<<std::endl;
        myfile<<ui->spinBox_gcode_decimal_precision->value()<<std::endl;
        myfile<<"-- use_linenumbers"<<std::endl;
        myfile<<ui->checkBox_use_line_numbers->checkState()<<std::endl;
        myfile<<"-- linenumber_character"<<std::endl;
        myfile<<ui->lineEdit_linenumber_first_character->text().toStdString()<<std::endl;
        myfile<<"-- filename"<<std::endl;
        myfile<<ui->lineEdit_gcode_filename->text().toStdString()<<std::endl;
        myfile<<""<<std::endl;

    myfile.close();
}

void MainWindow::on_pushButton_delete_head_pressed()
{
    int index=ui->comboBox_heads->currentIndex();
    if(ui->comboBox_heads->count()>1){
        ui->comboBox_heads->removeItem(index);
        heads.erase(heads.begin()+index);
    }

    //repopulate heads.tools
    ui->comboBox_head_tools->clear();
    for(int i=0; i<ui->comboBox_heads->count(); i++){
        ui->comboBox_head_tools->addItem(ui->comboBox_heads->itemText(i));
    }
}

void MainWindow::on_comboBox_heads_currentIndexChanged(int index)
{
    ui->lineEdit_head_edit->setText(QString::fromStdString(heads.at(index).axis));
    ui->lineEdit_head_offset_x->setText(QString::number(heads.at(index).offset_x));
    ui->lineEdit_head_offset_y->setText(QString::number(heads.at(index).offset_y));
    ui->lineEdit_m_code_start->setText(QString::fromStdString(heads.at(index).m_code_start));
    ui->lineEdit_m_code_stop->setText(QString::fromStdString(heads.at(index).m_code_stop));
}

void MainWindow::on_comboBox_tooltype_currentIndexChanged(int index)
{
    ui->stackedWidget_tools->setCurrentIndex(index);
}

void MainWindow::on_pushButton_clear_cam_pressed()
{
    cam.clear();
    jobs.clear();
    ui->listWidget_jobs->clear();
    ui->textEdit_gcode->clear();
}

void MainWindow::on_comboBox_layer_currentIndexChanged(const QString &arg1)
{

    std::string text=arg1.toStdString();
    text+='\n';
    //std::cout<<"layer: "<<text<<std::endl;

    for(unsigned int i=0; i<cad.size(); i++){
        if(cad.at(i).layer==text){
            cad.at(i).linewidth=2;
        } else {
            cad.at(i).linewidth=1;
        }
    }
}

void MainWindow::on_pushButton_generate_gcode_pressed()
{
    mode="";

    std::string intro = ui->plainTextEdit_gcode_intro->toPlainText().toStdString();
    std::string outtro = ui->plainTextEdit_gcode_outtro->toPlainText().toStdString();
    std::string filename = ui->lineEdit_gcode_filename->text().toStdString();
    std::string filepath = ui->lineEdit_gcode_filepath->text().toStdString();
    std::string file_extension= ui->comboBox_gcode_file_extension->currentText().toStdString();
    std::string full_filename;



    std::ofstream myfile;

    if(filepath=="currentpath"){
        full_filename=get_current_dir()+"/"+filename+file_extension;

    } else {
        full_filename=filepath+"/"+filename+file_extension /*, std::ios::app*/; //ios app = append true
    }

    full_filename.erase(std::remove(full_filename.begin(), full_filename.end(), '\n'));

    myfile.open(full_filename/*, std::ios::app*/); //ios app = append true
    myfile.clear(); //start with empty file
    generate_gcode(full_filename, intro, outtro, ui->spinBox_gcode_decimal_precision->value(), ui->lineEdit_delay->text().toStdString(), ui->lineEdit_linenumber_first_character->text().toStdString(),ui->checkBox_use_line_numbers->checkState());
    myfile.close();

    //update mainwindow gcode view
    ui->textEdit_gcode->clear();
    QString string = QString::fromStdString(full_filename);
    QFile file(string);
    QString line;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){

            line.append(stream.readLine()+"\n");
        }
        //testLabel->setText(line);
        ui->textEdit_gcode->setText(line);
    }
    file.close();
}



