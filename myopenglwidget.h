#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QVector>
#include <QTimer>
#include <QTime>
#include <QElapsedTimer>
#include "cardinalspline.h"

enum ToolMode
{
    Pen,
    Eraser,
    Anchor
};

enum MouseMode
{
    Press,
    Release
};

typedef QVector<Point>::iterator PointIter;

class MyOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    MyOpenGLWidget(QWidget* parent);
    ~MyOpenGLWidget();

    // Function: Interaction
    void setToolModePen(){toolMode=Pen;qDebug()<<tr("Set Mode Pen");}
    void setToolModeEraser(){toolMode=Eraser;}
    void setToolModeAnchor(){toolMode=Anchor;}

    //Function: Animation
    void startAnimation(){timer->start(10);doAnimation=true;}
    void endAnimation(){timer->stop();doAnimation=false;update();}

    // Parameter reset
    void resetTau(double tau){cardinalSpline.resetTau(tau);refreshCardinalSpline();update();}
    void resetGrain(int grain){cardinalSpline.resetGrain(grain);refreshCardinalSpline();update();};
    void resetFps(int fps){refresh_ms=1000/fps;update();};
    void resetTimePerRun(int timePerRun){cardinalSpline.aniSegment=(1000/refresh_ms)*timePerRun;cardinalSpline.refreshAnimationPoints();update();};
    void resetBondary(){cardinalSpline.resetBoundary();refreshCardinalSpline();update();}

private:
    // Field: Animation
    int refresh_ms = 16;
    bool doAnimation = false;
    QElapsedTimer clock;
    QTimer *timer;

    // Field: Interaction
    ToolMode toolMode = Pen;
    MouseMode mouseMode = Release;
    PointIter chosedPoint;

    // Field: OpenGL
    QOpenGLBuffer m_vertex;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram *m_program;

    // Field: Spline
    CardinalSpline cardinalSpline;

    // Function: to ease codes
    void setArrowCursor();
    void setCrossCursor();
    void refreshCardinalSpline();
    bool mouseHitPoint(PointIter& iter, QMouseEvent *event);
    void drawTriangle();

protected:
    // Function: OpenGL
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    // Widget Enter / Leave event
    void enterEvent(QEvent *) override;
    void leaveEvent(QEvent *) override;

    // Mouse Move Event
    void mouseMoveEvent(QMouseEvent * event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseLeftPressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // MYOPENGLWIDGET_H
