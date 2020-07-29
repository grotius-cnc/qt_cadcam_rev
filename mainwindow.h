#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

protected:

    void timerEvent(QTimerEvent *);

private slots:

    void on_toolButton_line_pressed();

    void on_toolButton_open_pressed();

    void on_toolButton_spline_pressed();

    void on_toolButton_arc_pressed();

    void on_toolButton_circle_pressed();

    void on_toolButton_rectangular_pressed();

    void on_toolButton_linestrip_pressed();

    void on_toolButton_ellipse_pressed();

    void on_toolButton_bezier_pressed();

    void on_toolButton_polygon_pressed();

    void on_toolButton_new_pressed();

    void on_pushButton_offset_pressed();

    void on_pushButton_nest_pressed();

    void on_pushButton_restore_pressed();

    void load_tools();

    void load_nest();

    void load_heads();

    void load_gcode();

    void on_comboBox_toolname_currentIndexChanged(int index);

    void on_pushButton_add_tool_pressed();

    void on_pushButton_delete_tool_pressed();

    void on_pushButton_save_tools_pressed();

    void on_pushButton_save_nest_settings_pressed();

    void on_pushButton_add_head_pressed();

    void on_pushButton_save_head_pressed();

    void on_pushButton_delete_head_pressed();

    void on_comboBox_heads_currentIndexChanged(int index);

    void on_comboBox_tooltype_currentIndexChanged(int index);

    void update_layers();

    void on_listWidget_jobs_currentTextChanged(const QString &currentText);

    void on_pushButton_clear_cam_pressed();

    void on_comboBox_layer_currentIndexChanged(const QString &arg1);

    void on_pushButton_generate_gcode_pressed();

    void on_pushButton_save_gcode_settings_pressed();

private:
    Ui::MainWindow *ui;
    int timerId;
};

#endif // MAINWINDOW_H
