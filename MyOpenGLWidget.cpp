#include "myopenglwidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>
#include <myopenglwidget.h>

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
float vertices[] = {
    // positions         // colors
     0.5f, -0.5f, -1.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  // bottom left
     0.0f,  0.5f, -1.0f,  0.0f, 0.0f, 1.0f   // top

};

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    //  The following is not compulsory
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format); // must be called before the widget or its parent window gets shown
}

MyOpenGLWidget::~MyOpenGLWidget(){
    if(m_vertex.isCreated())
        m_vertex.destroy();
    if(m_vao.isCreated())
        m_vao.destroy();
    if(m_program)
        delete m_program;
}


void MyOpenGLWidget::initializeGL()
{
    //  Initialize the OpenGL
    initializeOpenGLFunctions();

    //  Create a shader, don't release before the creation of vertex
    m_program = new QOpenGLShaderProgram();
    bool result = m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
    result = m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
    result = m_program->link();
    result = m_program->bind();

    //  Create VAO (Vertex Array Object)
    m_vao.create();
    m_vao.bind();

    //  Create VBO (Vertex Buffer Object)
    m_vertex.create();
    m_vertex.bind();
    m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertex.allocate(vertices, sizeof(vertices));

    // position attribute
    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(GLfloat)*6);
    m_program->enableAttributeArray(0);
    // color attribute
    m_program->setAttributeBuffer(1, GL_FLOAT, sizeof(GLfloat)*3, 3, sizeof(GLfloat)*6);
    m_program->enableAttributeArray(1);

    m_vao.release();
    m_vertex.release();
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
    if(0 == h)
        h = 1;

    glViewport(0, 0, w, h);

    QMatrix4x4 projMatrix;
    projMatrix.setToIdentity();
    projMatrix.perspective(60.0f,  (GLfloat)w / (GLfloat)h, 0.01f, 100.0f);

    m_program->bind();
    int projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_program->setUniformValue(projMatrixLoc, projMatrix);
    m_program->release();
}

void MyOpenGLWidget::paintGL()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.12,0.12,0.12,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    glDisable(GL_DEPTH_TEST);

    QPainter qp;

    qp.begin(this);

    qp.setRenderHint(QPainter::Antialiasing);
    qp.setRenderHint(QPainter::TextAntialiasing);
    qp.setRenderHint(QPainter::SmoothPixmapTransform);

    cardinalSpline.drawLines(qp);
    cardinalSpline.drawCurves(qp);

    if(doAnimation)
    {
        cardinalSpline.drawAnimation(qp, clock, refresh_ms);
    }

    qp.end();
}

void MyOpenGLWidget::enterEvent(QEvent *)
{

}

void MyOpenGLWidget::leaveEvent(QEvent *)
{

}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent * event)
{
    bool onPoint=false;
    for(QVector<Point>::iterator iter = cardinalSpline.points.begin(); iter != cardinalSpline.points.end(); iter++)
    {
        if(mouseHitPoint(iter,event)){
            onPoint=true;
        }
    }
    if(toolMode==Pen)
    {
        if(onPoint==false)
            setCrossCursor();
        else
            setArrowCursor();
    }
    else if(toolMode==Eraser || Anchor)
    {
        if(onPoint==false)
            setArrowCursor();
        else
            setCrossCursor();

        if(mouseMode==Press){
            chosedPoint->x=qMin(qMax(event->pos().x(), 0),width());
            chosedPoint->y=qMin(qMax(event->pos().y(), 0),height());
            refreshCardinalSpline();
            update();
        }
    }
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    // If Left is pressed
    if(event->button() == Qt::LeftButton)
   {
        mouseLeftPressEvent(event);
    }
    // If Right is pressed
    else if(event->button() == Qt::RightButton)
   {
        setCursor(Qt::ArrowCursor);
    }
}

void MyOpenGLWidget::mouseLeftPressEvent(QMouseEvent *event)
{
    for(QVector<Point>::iterator iter = cardinalSpline.points.begin(); iter != cardinalSpline.points.end(); iter++)
    {
        if(mouseHitPoint(iter,event)){
            if(toolMode==Eraser)
            {
                iter = cardinalSpline.points.erase(iter);
                refreshCardinalSpline();
                setCrossCursor();
                return;
            }
            else if(toolMode==Anchor)
            {
                chosedPoint=iter;
                mouseMode=Press;
                setCrossCursor();
                return;
            }
            return;
        }
    }
    if(toolMode==Pen)
    {
        cardinalSpline.points.push_back(Point(event->pos().x(),event->pos().y()));
        refreshCardinalSpline();
        update();
    }
}

void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent *event){// 鼠标释放事件
    if(toolMode==Anchor)
        {
            mouseMode=Release;
            setArrowCursor();
        }
}

void MyOpenGLWidget::setArrowCursor()
{
    setCursor(Qt::ArrowCursor);
    update();
}

void MyOpenGLWidget::setCrossCursor()
{
    setCursor(Qt::CrossCursor);
    update();
}

 void MyOpenGLWidget::refreshCardinalSpline()
 {
     cardinalSpline.doEntiretyInterpolation();
     cardinalSpline.refreshAnimationPoints();
 }

 bool MyOpenGLWidget::mouseHitPoint(PointIter& iter, QMouseEvent *event)
 {
    return fabs(event->pos().x()-iter->x)<=5&& fabs(event->pos().y()-iter->y)<=5;
 }

 void MyOpenGLWidget::drawTriangle()
 {
     QMatrix4x4 modelMatrix;
     modelMatrix.translate(0, 0, -1);
     int modelMatrixLoc = m_program->uniformLocation("modelMatrix");
     m_program->setUniformValue(modelMatrixLoc, modelMatrix);

     m_vao.bind();
     glDrawArrays(GL_TRIANGLES, 0, 3);
     m_vao.release();

     m_program->release();
 }
