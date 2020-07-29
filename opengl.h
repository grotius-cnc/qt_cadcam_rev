#ifndef OPENGL_H
#define OPENGL_H

#include <QMainWindow>
#include <mainwindow.h>
#include <QWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QToolTip>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <QtWidgets>

class opengl : public QOpenGLWidget
{

public:
    opengl(QWidget *parent = nullptr);
    ~opengl();

public:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
};

#endif // OPENGL_H
