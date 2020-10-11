#ifndef CARDINALSPLINE_H
#define CARDINALSPLINE_H

#include<QVector>
#include<QPainter>
#include <QElapsedTimer>

struct Point
{
public:
    double x;
    double y;

    Point():x(0),y(0){}
    Point(double xx, double yy):x(xx),y(yy){}

    friend Point operator*(const double& a,const Point& b);
    friend Point lerp(const Point& a,const Point& b, double alpha);
    Point operator+(const Point& b);
    Point operator-(const Point& b);
};

class CardinalSpline
{
public:
    CardinalSpline();
    // Function: Draw
    void drawLines(QPainter& painter);
    void drawCurves(QPainter& painter);
    void drawAnimation(QPainter& painter, QElapsedTimer& clock, int ms);
    // Function: Reset parameters
    void resetTau(double tau){setMatrix(tau);}
    void resetGrain(int grain){this->grain = grain;}
    void refreshAnimationPoints();
    void resetBoundary(){useSpecialBoundary=!useSpecialBoundary;}
    // Field: Point datas
    void doEntiretyInterpolation();
    QVector<Point> points;
    QVector<Point> interplatedPoints;
    QVector<Point> animationPoints;
    int aniSegment = 300;
private:
    // Feild: parameters & matrix
    int grain=20;
    double Matrix[16];
    bool useSpecialBoundary = false;

    // Function: generate curve
    void setMatrix(double tau);
    Point getOneInterpolation(Point& p1, Point& p2, Point& p3, Point& p4, double u);
    QVector<Point>::iterator iter;
};

#endif // CARDINALSPLINE_H
