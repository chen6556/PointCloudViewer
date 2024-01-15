#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QPaintEvent>
#include "base/PointCloud.hpp"


class Viewer : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

private:
    size_t _data_count = 0;
    int _uniforms[6];
    unsigned int _VAO, _VBO;
    
    float _ctm0[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f};
    float _ctm1[16] = {1.0f, 0.0f, 0.0f, 0.0f,
                      0.0f, 1.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 1.0f, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f};

    float _ratio = 1.0f, _FOV = 90.0f;
    float _viewer_width = 0, _viewer_height = 0;
    PointCloud::PointCloudSize _pd_size;

    bool _rotate = false, _move = false;

private:
    void init();

protected:
    void initializeGL();

    void resizeGL(int w, int h);

    void paintGL();

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

public:
    Viewer(QWidget *parent);


    void load_data(const PointCloud::PointCloud &pd);


};