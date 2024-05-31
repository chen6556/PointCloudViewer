#include "ui/Viewer.hpp"
#include "ui/GLSL.hpp"
#include "base/Utils.hpp"


Viewer::Viewer(QWidget *parent)
    : QOpenGLWidget(parent)
{
    init();
}



void Viewer::init()
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);
}

void Viewer::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.117647f, 0.156862f, 0.188235f, 1.0f);

    unsigned int vertex_shader;
    unsigned int fragment_shader;

    glPointSize(2.0f); // 点大小
    glLineWidth(1.0f); // 线宽
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &GLSL::base_vss, NULL);
    glCompileShader(vertex_shader);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &GLSL::base_fss, NULL);
    glCompileShader(fragment_shader);

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glDeleteShader(fragment_shader);

    glDeleteShader(vertex_shader);
    _uniforms[0] = glGetUniformLocation(shader_program, "size");
    _uniforms[1] = glGetUniformLocation(shader_program, "projection");
    _uniforms[2] = glGetUniformLocation(shader_program, "model");

    glUseProgram(shader_program);
    glUniformMatrix4fv(_uniforms[1], 1, GL_TRUE, _ctm0); // projection
    glUniformMatrix4fv(_uniforms[2], 1, GL_TRUE, _ctm1); // model

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(2, _IBO);

    glBindVertexArray(_VAO);
    glVertexAttribFormat(0, 6, GL_FLOAT, GL_FALSE, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Viewer::resizeGL(int w, int h)
{
    glUniform3f(_uniforms[0], w / 2, h / 2, 1.74 * _pd_size.len); // size
    glViewport(0, 0, w, h);

    float v = std::tanf(PointCloud::PI * _FOV / 360);
    _ctm0[0] = h / (w * v);
    _ctm0[5] = 1.0f / v;

    glUniformMatrix4fv(_uniforms[1], 1, GL_TRUE, _ctm0); // projection

    _viewer_width = w, _viewer_height = h;
}

void Viewer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (_show_point)
    {
        glDrawArrays(GL_POINTS, 0, _data_count);
    }

    if (_show_surface && _surface_count > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO[0]); // surface
        glDrawElements(GL_POLYGON, _surface_count, GL_UNSIGNED_INT, NULL);
    }
    if (_show_edge && _edge_count > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO[1]); // edge
        glDrawElements(GL_LINE_LOOP, _edge_count, GL_UNSIGNED_INT, NULL);
    }
}

void Viewer::mousePressEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    case Qt::LeftButton:
        _rotate = true;
        break;
    case Qt::RightButton:
        _move = true;
        break;
    case Qt::MiddleButton:
        if (event->modifiers() == Qt::KeyboardModifier::ControlModifier)
        {
            _FOV = 90.0f;
            float v = std::tanf(PointCloud::PI / 4);
            _ctm0[0] = _viewer_height / (_viewer_width * v);
            _ctm0[5] = 1.0f / v;
            makeCurrent();
            glUniformMatrix4fv(_uniforms[1], 1, GL_TRUE, _ctm0); // projection
            doneCurrent();
        }
        else
        {
            std::fill_n(_ctm1, 15, 0.0f);
            _ctm1[0] = _ctm1[5] = _ctm1[10] = _ratio;
            _ctm1[15] = 1.0f;
            makeCurrent();
            glUniformMatrix4fv(_uniforms[2], 1, GL_TRUE, _ctm1); // model
            doneCurrent();
        }
        update();
        break;
    default:
        break;
    }
    _pos = event->pos();
}

void Viewer::mouseReleaseEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    case Qt::LeftButton:
        _rotate = false;
        break;
    case Qt::RightButton:
        _move = false;
        break;
    case Qt::MiddleButton:
        break;
    default:
        break;
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint pos = event->pos();
    if (_rotate)
    {
        int y_dir = pos.x() > _pos.x() ? 1 : -1;
        int x_dir = pos.y() > _pos.y() ? 1 : -1;
        if (std::abs(pos.x() - _pos.x()) > std::abs(pos.y() - _pos.y()))
        {
            x_dir = 0;
        }
        else
        {
            y_dir = 0;
        }
        float mat_x[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, std::cosf(x_dir * PointCloud::PI / 180), -std::sinf(x_dir * PointCloud::PI / 180), 0.0f,
            0.0f, std::sinf(x_dir * PointCloud::PI / 180), std::cosf(x_dir * PointCloud::PI / 180), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f },
            mat_y[16] = { std::cosf(y_dir * PointCloud::PI / 180), 0.0f, std::sinf(y_dir * PointCloud::PI / 180), 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                -std::sinf(y_dir * PointCloud::PI / 180), 0.0f, std::cosf(y_dir * PointCloud::PI / 180), 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f };
        float mat0[16], mat1[16];
        Utils::mul<4>(mat_x, mat_y, mat0);
        std::memmove(mat1, _ctm1, 16 * sizeof(float));
        Utils::mul<4>(mat0, mat1, _ctm1);
    }
    else if (_move)
    {
        float x_dir = pos.x() > _pos.x() ? 0.002f : -0.002f;
        float y_dir = pos.y() < _pos.y() ? 0.002f : -0.002f;
        if (std::abs(pos.x() - _pos.x()) > std::abs(pos.y() - _pos.y()))
        {
            y_dir = 0.0f;
        }
        else
        {
            x_dir = 0.0f;
        }
        float mat0[16] = { 1.0f, 0.0f, 0.0f, x_dir,
                           0.0f, 1.0f, 0.0f, y_dir,
                           0.0f, 0.0f, 1.0f, 0.0f,
                           0.0f, 0.0f, 0.0f, 1.0f };
        float mat1[16];
        std::memmove(mat1, _ctm1, 16 * sizeof(float));
        Utils::mul<4>(mat0, mat1, _ctm1);
    }
    _pos = pos;

    makeCurrent();
    glUniformMatrix4fv(_uniforms[2], 1, GL_TRUE, _ctm1); // model
    doneCurrent();

    update();
}

void Viewer::wheelEvent(QWheelEvent *event)
{
    float v;
    if (event->angleDelta().y() > 0)
    {
        v = 1.25f;
        if (event->modifiers() == Qt::KeyboardModifier::ControlModifier)
        {
            _ratio *= 1.25;
        }
        else
        {
            _FOV *= 0.8;
            if (_FOV < 0.001)
            {
                _FOV = 0.001;
            }
        }
    }
    else if (event->angleDelta().y() < 0)
    {
        v = 0.8f;
        if (event->modifiers() == Qt::KeyboardModifier::ControlModifier)
        {
            _ratio *= 0.8;
        }
        else
        {
            _FOV *= 1.25;
            if (_FOV > 180)
            {
                _FOV = 180;
            }
        }
    }

    if (event->modifiers() == Qt::KeyboardModifier::ControlModifier)
    {
        float mat[16] = {v, 0.0f, 0.0f, 0.0f,
                    0.0f, v, 0.0f, 0.0f,
                    0.0f, 0.0f, v, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f};
        float output[16];
        Utils::mul<4>(mat, _ctm1, output);
        std::memmove(_ctm1, output, 16 * sizeof(float));
    }
    else
    {
        v = std::tanf(PointCloud::PI * _FOV / 360);
        _ctm0[0] = _viewer_height / (_viewer_width * v);
        _ctm0[5] = 1.0f / v;
    }
    makeCurrent();
    if (event->modifiers() == Qt::KeyboardModifier::ControlModifier)
    {
        glUniformMatrix4fv(_uniforms[2], 1, GL_TRUE, _ctm1); // model
    }
    else
    {
        glUniformMatrix4fv(_uniforms[1], 1, GL_TRUE, _ctm0); // projection
    }
    doneCurrent();

    update();
}


void Viewer::load_data(const PointCloud::PointCloud &pd)
{
    if (pd.data.empty())
    {
        return;
    }

    _data_count = pd.data.size() / 6;
    _surface_count = pd.surface.size();
    _edge_count = pd.edge.size();
    _pd_size = pd.size;
    _FOV = std::atanf(std::min(_pd_size.len, 1.0f)) * 270 / PointCloud::PI;

    float v = std::tanf(PointCloud::PI * _FOV / 360);
    _ctm0[0] = _viewer_height / (_viewer_width * v);
    _ctm0[5] = 1.0f / v;

    if (_pd_size.len > 1.0f)
    {
        _ratio = _pd_size.len <= 1.0 ? 1.0f : 1.0f / _pd_size.len;
        float mat[16] = {_ratio, 0.0f, 0.0f, 0.0f,
                        0.0f, _ratio, 0.0f, 0.0f,
                        0.0f, 0.0f, _ratio, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f};
        float output[16];
        Utils::mul<4>(mat, _ctm1, output);
        std::memmove(_ctm1, output, 16 * sizeof(float));
    }

    makeCurrent();

    glBindBuffer(GL_ARRAY_BUFFER, _VBO); // point
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pd.data.size(), &pd.data.front(), GL_STATIC_DRAW);

    if (_surface_count > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO[0]); // surface
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * pd.surface.size(), &pd.surface.front(), GL_STATIC_DRAW);
    }

    if (_edge_count > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO[1]); // edge
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * pd.edge.size(), &pd.edge.front(), GL_STATIC_DRAW);
    }

    glUniform3f(_uniforms[0], _viewer_width / 2, _viewer_height / 2, 1.74 * _pd_size.len); // size
    glUniformMatrix4fv(_uniforms[1], 1, GL_TRUE, _ctm0); // projection
    glUniformMatrix4fv(_uniforms[2], 1, GL_TRUE, _ctm1); // model

    doneCurrent();
}




void Viewer::set_show_point(const bool value)
{
    _show_point = value;
    update();
}

void Viewer::set_show_surface(const bool value)
{
    _show_surface = value;
    update();
}

void Viewer::set_show_edge(const bool value)
{
    _show_edge = value;
    update();
}


