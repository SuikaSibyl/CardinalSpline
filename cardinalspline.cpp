#include "cardinalspline.h"
#include <QDebug>

Point operator*(const double& a,const Point& b)
{
        return Point(b.x*a, b.y*a);
}

Point Point::operator+(const Point& b)
{
    return Point(this->x+b.x, this->y+b.y);
}

Point Point::operator-(const Point& b)
{
    return Point(this->x-b.x, this->y-b.y);
}

Point lerp(const Point& a,const Point& b, double alpha)
{
    double x = (b.x)*alpha + (a.x)*(1-alpha);
    double y = (b.y)*alpha + (a.y)*(1-alpha);
    return Point(x,y);
}

CardinalSpline::CardinalSpline()
{
    setMatrix(0.5);
}

void CardinalSpline::drawLines(QPainter& painter)
{
    // Do nothing if there is no point
    if(points.size()==0)
        return;

    // Draw lines
    painter.setPen(QColor::fromRgb(118,121,124));
    for(QVector<Point>::iterator iter = points.begin(); (iter+1)!=points.end(); iter++)
    {
        QLine line(iter->x, iter->y, (iter+1)->x, (iter+1)->y);
        painter.drawLine(line);
    }

    // Draw points
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    for(QVector<Point>::iterator iter = points.begin(); iter!=points.end(); iter++)
    {
        QRect rect(iter->x, iter->y,5,5);
        painter.drawEllipse(rect);
    }
}

void CardinalSpline::drawCurves(QPainter& painter)
{
    if(points.size()<=1)
        return;

    // Draw lines
    painter.setPen(QColor::fromRgb(61,174,233));
    if(interplatedPoints.size()==0)
        return;
    for(QVector<Point>::iterator iter = interplatedPoints.begin(); (iter+1)!=interplatedPoints.end(); iter++)
    {
        QLine line(iter->x, iter->y, (iter+1)->x, (iter+1)->y);
        painter.drawLine(line);
    }

    // Draw points
    painter.setPen(QColor::fromRgb(61,174,233));
    painter.setBrush(QColor::fromRgb(61,174,233));
    for(QVector<Point>::iterator iter = interplatedPoints.begin(); iter!=interplatedPoints.end(); iter++)
    {
        QRect rect(iter->x, iter->y,2,2);
        painter.drawEllipse(rect);
    }
}
void CardinalSpline::drawAnimation(QPainter& painter, QElapsedTimer& clock, int ms)
{
    if(animationPoints.size()<=1)
        return;

    QImage image(":/image/images/car.png");
    double angle = 0;
    double angle_a = 0;
    QImage rotated_image = image;

    if((iter+1)==animationPoints.end())
        iter=animationPoints.begin();

    if((iter+1)!=animationPoints.end())
    {
        double dx = (iter+1)->x - iter->x;
        double dy = (iter+1)->y - iter->y;
        if(dx>=0)
            angle_a= atan(-dy/dx);
        else
            angle_a= atan(-dy/dx)+3.14;
        angle = angle_a/3.14*180;
        QMatrix matrix;
        matrix.rotate(-angle);
        rotated_image=image.transformed(matrix, Qt::FastTransformation);
    }

    painter.drawImage(iter->x-55-sin(angle_a)*32,iter->y-32-cos(angle_a)*32, rotated_image);
    if(clock.elapsed()>=ms)
    {
        iter++;
        clock.restart();
    }
}

void CardinalSpline::doEntiretyInterpolation()
{
    // Don't draw if there is only one point
    if(points.size()<=1)
        return;

    interplatedPoints.clear();
    interplatedPoints.push_back(points.front());

    if(points.size()>=3 && useSpecialBoundary==true)
    {
        int back = points.size();
        points.push_front(points[2]+points[0]-points[1]);
        points.push_back(points[back]-points[back-1]+points[back-2]);
    }
    else
    {
        points.push_front(points.front());
        points.push_back(points.back());
    }

    //  Use loop to renew interplatedPoints
    double step = 1.0/grain;
    int loopDestination = points.size()-2;
    for(int i=1; i<loopDestination; i++)
    {
        for(int j=1; j<=grain; j++)
        {
            interplatedPoints.push_back(getOneInterpolation(points[i-1], points[i], points[i+1], points[i+2], j*step));
        }
    }

    points.pop_front();
    points.pop_back();
}

Point CardinalSpline::getOneInterpolation(Point& p1, Point& p2, Point& p3, Point& p4, double u)
{
        Point tmp1 = Matrix[0]*p1+ Matrix[1]*p2+Matrix[2]*p3+Matrix[3]*p4;
        Point tmp2 = Matrix[4]*p1+ Matrix[5]*p2+Matrix[6]*p3+Matrix[7]*p4;
        Point tmp3 = Matrix[8]*p1+ Matrix[9]*p2+Matrix[10]*p3+Matrix[11]*p4;
        Point tmp4 = Matrix[12]*p1+ Matrix[13]*p2+Matrix[14]*p3+Matrix[15]*p4;
        return Point(tmp4+u*(tmp3+u*(tmp2+u*tmp1)));
}

void CardinalSpline::setMatrix(double tau)
{
    Matrix[0]=-tau; Matrix[1]=2.-tau; Matrix[2]=tau-2.; Matrix[3]=tau;
    Matrix[4]=2.*tau; Matrix[5]=tau-3.; Matrix[6]=3.-2*tau; Matrix[7]=-tau;
    Matrix[8]=-tau; Matrix[9]=0.; Matrix[10]=tau; Matrix[11]=0.;
    Matrix[12]=0.; Matrix[13]=1.; Matrix[14]=0.; Matrix[15]=0.;
}

void CardinalSpline::refreshAnimationPoints()
{
    if(interplatedPoints.size()<=1)
        return;

    animationPoints.clear();

    QVector<double> accumulatedLength;
    accumulatedLength.push_back(0);
    for(QVector<Point>::iterator iter = interplatedPoints.begin(); (iter+1)!=interplatedPoints.end(); iter++)
    {
        double dx = (iter->x)-((iter+1)->x);
        double dy = (iter->y)-((iter+1)->y);
        accumulatedLength.push_back(accumulatedLength.back() + sqrt(dx*dx + dy*dy));
    }

    double stepLength = accumulatedLength.back()/aniSegment;
    double takedLength = 0;
    int beyongdInterpoint = 0;
    animationPoints.push_back(interplatedPoints.front());

    for( int i=1; i<= aniSegment; i++)
    {
        takedLength += stepLength;
        while((beyongdInterpoint<accumulatedLength.size()-2) && (takedLength>accumulatedLength[beyongdInterpoint+1]))
            beyongdInterpoint++;
        double u = (takedLength - accumulatedLength[beyongdInterpoint])/(accumulatedLength[beyongdInterpoint+1]-accumulatedLength[beyongdInterpoint]);
        Point newAniPoint = lerp(interplatedPoints[beyongdInterpoint], interplatedPoints[beyongdInterpoint+1], u);
        animationPoints.push_back(newAniPoint);
    }

    iter=animationPoints.begin();
}
