#include "ui/Viewer.hpp"
#include "ui/GLSL.hpp"


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

    setAutoFillBackground(false);
}

void Viewer::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.117647f, 0.156862f, 0.188235f, 1.0f);

    unsigned int vertex_shader;
    unsigned int fragment_shader;

    // glPointSize(8.0f); // 点大小
    glLineWidth(1.0f); // 线宽
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
    _uniforms[1] = glGetUniformLocation(shader_program, "row0");
    _uniforms[2] = glGetUniformLocation(shader_program, "row1");
    _uniforms[3] = glGetUniformLocation(shader_program, "row2");
    _uniforms[4] = glGetUniformLocation(shader_program, "row3");

    glUseProgram(shader_program);
    glUniform4f(_uniforms[1], 1.0f, 0.0f, 0.0f, 0.0f); // row0
    glUniform4f(_uniforms[2], 0.0f, 1.0f, 0.0f, 0.0f); // row1
    glUniform4f(_uniforms[3], 0.0f, 0.0f, 1.0f, 0.0f); // row2
    glUniform4f(_uniforms[4], 0.0f, 0.0f, 0.0f, 1.0f); // row3

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);

    glBindVertexArray(_VAO);
    glVertexAttribFormat(0, 6, GL_FLOAT, GL_FALSE, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
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
    _ctm0[10] = -1.0f;
    _ctm0[14] = -1.0f;
    _ctm0[15] = 1.0f;

    glUniform4f(_uniforms[1], _ctm0[0], _ctm0[1], _ctm0[2], _ctm0[3]); // row0
    glUniform4f(_uniforms[2], _ctm0[4], _ctm0[5], _ctm0[6], _ctm0[7]); // row1
    glUniform4f(_uniforms[3], _ctm0[8], _ctm0[9], _ctm0[10], _ctm0[11]); // row2
    glUniform4f(_uniforms[4], _ctm0[12], _ctm0[13], _ctm0[14], _ctm0[15]); // row3

    _viewer_width = w, _viewer_height = h;
}

void Viewer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glDrawArrays(GL_POINTS, 0, _data_count);
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
        break;
    default:
        break;
    }
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
    if (_rotate)
    {

    }
    else if (_move)
    {
        
    }
}

void Viewer::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
    {
        _ratio *= 1.25;
        _FOV *= 0.8;
    }
    else if (event->angleDelta().y() < 0)
    {
        _ratio *= 0.8;
        _FOV *= 1.25;
    }

    float v = std::tanf(PointCloud::PI * _FOV / 360);
    _ctm0[0] = _viewer_height / (_viewer_width * v);
    _ctm0[5] = 1.0f / v;
    _ctm0[10] = -1.0f;
    _ctm0[14] = -1.0f;
    _ctm0[15] = 1.0f;

    makeCurrent();
    glUniform4f(_uniforms[1], _ctm0[0], _ctm0[1], _ctm0[2], _ctm0[3]); // row0
    glUniform4f(_uniforms[2], _ctm0[4], _ctm0[5], _ctm0[6], _ctm0[7]); // row1
    glUniform4f(_uniforms[3], _ctm0[8], _ctm0[9], _ctm0[10], _ctm0[11]); // row2
    glUniform4f(_uniforms[4], _ctm0[12], _ctm0[13], _ctm0[14], _ctm0[15]); // row3
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
    _pd_size = pd.size;

    float v = std::tanf(PointCloud::PI * _FOV / 360);
    _ctm0[0] = _viewer_height / (_viewer_width * v);
    _ctm0[5] = 1.0f / v;
    _ctm0[10] = -1.0f;
    _ctm0[14] = -1.0f;

    makeCurrent();

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _data_count, &pd.data.front(), GL_STATIC_DRAW);

    glUniform3f(_uniforms[0], _viewer_width / 2, _viewer_height / 2, 1.74 * _pd_size.len); // size
    glUniform4f(_uniforms[1], _ctm0[0], _ctm0[1], _ctm0[2], _ctm0[3]); // row0
    glUniform4f(_uniforms[2], _ctm0[4], _ctm0[5], _ctm0[6], _ctm0[7]); // row1
    glUniform4f(_uniforms[3], _ctm0[8], _ctm0[9], _ctm0[10], _ctm0[11]); // row2
    glUniform4f(_uniforms[4], _ctm0[12], _ctm0[13], _ctm0[14], _ctm0[15]); // row3

    doneCurrent();
}