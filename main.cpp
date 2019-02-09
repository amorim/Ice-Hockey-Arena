#if _WIN32
  #ifndef _WIN32
    #include <windows.h>
  #endif
#endif
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
#include <GL/freeglut.h>
#include <cmath>

const int WIDTH = 500, HEIGHT = 800;
const bool pixel = true, bresenhamMode = true;
const double squareSizeDiv2 = 0.5;
const double penWidth = 50;

bool ready = false;

typedef struct {
    double x, y;
} Point;

typedef struct {
    Point pi, pf;
} Line;

typedef struct {
    Point center;
    double radious;
} Circle;

vector<Point> points;
double cornerRadious;
int pointsReceived = 0;

void init(void) {
    glClearColor(1, 1, 1, 1);
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
}

void drawPoint(double x, double y)
{
    if (pixel) glVertex2d(x, y);
    else glRectd(x - squareSizeDiv2, y - squareSizeDiv2, x + squareSizeDiv2, y + squareSizeDiv2);
}

void bresenham(Line line)
{
    if (pixel) glBegin(GL_POINTS);
    // If the line is steeper, we better swap x by y and continue drawing going up
    bool steep = abs(line.pf.y - line.pi.y) > abs(line.pf.x - line.pi.x);
    if (steep) swap(line.pi.x, line.pi.y), swap(line.pf.x, line.pf.y);
    if (line.pi.x > line.pf.x) swap(line.pi.x, line.pf.x), swap(line.pi.y, line.pf.y);
//    printf("(%3.3lf, %3.3lf) -> (%3.3lf, %3.3lf)\n", line.pi.x, line.pi.y, line.pf.x, line.pf.y);

    double dx, dy, E, NE, d, x, y, yStep = line.pi.y > line.pf.y ? -1 : 1;
    dx = line.pf.x - line.pi.x, dy = abs(line.pf.y - line.pi.y);
    d = 2 * dy - dx;
    E = 2 * dy, NE = 2 * (dy - dx);
    x = line.pi.x, y = line.pi.y;
    if (steep) drawPoint(y, x); else drawPoint(x, y);
    while (x < line.pf.x)
    {
        if (d <= 0) d += E, x ++;
        else d += NE, x ++, y += yStep;
//        printf("(%d, %d)\n", x, y);
        if (steep) drawPoint(y, x); else drawPoint(x, y);
    }
    if (pixel) glEnd();
}
void lineEquation(Line line)
{
    // y = m*x + b
    // m = (y1 - y0)/(x1 - x0)
    // b = y0 (in this case)
    if (pixel) glBegin(GL_POINTS);
    bool steep = abs(line.pf.y - line.pi.y) > abs(line.pf.x - line.pi.x);
    if (steep) swap(line.pi.x, line.pi.y), swap(line.pf.x, line.pf.y);
    if (line.pi.x > line.pf.x) swap(line.pi.x, line.pf.x), swap(line.pi.y, line.pf.y);
    double m = (line.pf.y - line.pi.y) / (line.pf.x - line.pi.x);
    for (double x = line.pi.x, y = line.pi.y; x <= line.pf.x; x ++)
    {
        if (steep) drawPoint(y, x); else drawPoint(x, y);
        // y += m;// + line.pi.y;
        y = (x - line.pi.x) * m + line.pi.y;
    }
    if (pixel) glEnd();
}
void drawLine(Line line)
{
    double t = atan(-(line.pi.x - line.pf.x) / (line.pi.y - line.pf.y));
    Point perpendicularVector = {cos(t), sin(t)};
    for (int j = 0; j < penWidth; j ++)
    {
        double p = (j - (penWidth - 1) / 2.0);
        Line aux = {{line.pf.x + p * perpendicularVector.x, line.pf.y + p * perpendicularVector.y},
                {line.pi.x + p * perpendicularVector.x, line.pi.y + p * perpendicularVector.y}};
        if (bresenhamMode) bresenham(aux);
        else lineEquation(aux);
    }
}

void circlePoints(double x, double y, bool q1, bool q2, bool q3, bool q4)
{
    if (q1) drawPoint(x, y), drawPoint(y, x);
    if (q2) drawPoint(-x, y), drawPoint(-y, x);
    if (q3) drawPoint(-x, -y), drawPoint(-y, -x);
    if (q4) drawPoint(y, -x), drawPoint(x, -y);
}
void midPointCircle(Circle circle, bool q1, bool q2, bool q3, bool q4)
{
    glPushMatrix();
        glTranslated(circle.center.x, circle.center.y, 0);
        if (pixel) glBegin(GL_POINTS);

        double x = 0, y = circle.radious;
        double d = (double) 5 / 4 - circle.radious;
        circlePoints(x, y, q1, q2, q3, q4);
        while (y > x)
        {
            if (d < 0) d += 2 * x + 3, x ++;
            else d += 2 * (x - y) + 5, x ++, y --;
            circlePoints(x, y, q1, q2, q3, q4);
        }

        if (pixel) glEnd();
    glPopMatrix();
}
void circleEquation(Circle circle, bool q1, bool q2, bool q3, bool q4)
{
    glPushMatrix();
        glTranslated(circle.center.x, circle.center.y, 0);
        if (pixel) glBegin(GL_POINTS);

        double end = circle.radious / sqrt(2) + 1;
        for (double x = 0, y = circle.radious; x <= end; x ++)
        {
            circlePoints(x, y, q1, q2, q3, q4);
            y = sqrt(pow(circle.radious, 2) - pow(x, 2));
        }

        if (pixel) glEnd();
    glPopMatrix();
}
void drawCircle(Circle circle, bool q1, bool q2, bool q3, bool q4)
{
    for (int j = 0; j < penWidth; j ++)
    {
        Circle aux = circle;
        aux.radious += j - (penWidth - 1) / 2.0;
        if (bresenhamMode) midPointCircle(aux, q1, q2, q3, q4);
        else circleEquation(aux, q1, q2, q3, q4);
    }
}

void drawRectangle(Point a, Point b)
{
  drawLine({{a.x, a.y}, {b.x, a.y}});
  drawLine({{b.x, a.y}, {b.x, b.y}});
  drawLine({{b.x, b.y}, {a.x, b.y}});
  drawLine({{a.x, b.y}, {a.x, a.y}});
}

double rot = 0;
int factor = 1;
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    // glClearColor(1, 1, 1, 1);

    if (ready) {
        glColor3ub(0, 0, 255);

        Point a = points[0], b = points[1];
        points[0].x = min(a.x, b.x), points[0].y = max(a.y, b.y);
        points[1].x = max(a.x, b.x), points[1].y = min(a.y, b.y);

        drawLine({{points[0].x + cornerRadious, points[0].y}, {points[1].x - cornerRadious, points[0].y}});
        drawLine({{points[1].x, points[0].y - cornerRadious}, {points[1].x, points[1].y + cornerRadious}});
        drawLine({{points[1].x - cornerRadious, points[1].y}, {points[0].x + cornerRadious, points[1].y}});
        drawLine({{points[0].x, points[1].y + cornerRadious}, {points[0].x, points[0].y - cornerRadious}});

        double displacement = cornerRadious;

        drawCircle({{points[1].x - displacement, points[0].y - displacement}, cornerRadious}, true, false, false, false);
        drawCircle({{points[0].x + displacement, points[0].y - displacement}, cornerRadious}, false, true, false, false);
        drawCircle({{points[0].x + displacement, points[1].y + displacement}, cornerRadious}, false, false, true, false);
        drawCircle({{points[1].x - displacement, points[1].y + displacement}, cornerRadious}, false, false, false, true);
    }

    glutSwapBuffers();
}

void timer(int val) {
    glutTimerFunc(1000/100, timer, 1);
    glutPostRedisplay();
}

void callback(int x, int y) {
    if (x < 400)
        factor = 1;
    else
        factor = -1;
}

void mouseClicked(int button, int state, int x, int y) {
    y = HEIGHT - y;
    if (state == GLUT_UP) {
        if (!ready) {
            points.push_back({(double) x, (double) y});
            printf("%d\n", points.size());
            cornerRadious = (abs(points[0].x - points[1].x) / 2.0 + abs(points[0].y - points[1].y) / 2.0) / 5.0;
            if (points.size() == 2)
                ready = true;
        }
        printf("Start Points: %lf %lf and %lf %lf | cornerRadious: %lf\n", points[0].x, points[0].y, points[1].x,
               points[1].y, cornerRadious);
    }
}

int main(int argc, char *argv[]) {

    /* inicializa o sitema GLUT */
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 25);
    glutCreateWindow("Quadra");

    //chama a funcao init e inicia as propriedades View Port
    init();
    /* Callback para mostrar objetos na tela */
    glutDisplayFunc(display);
    glutTimerFunc(1000/60, timer, 1);
    glutPassiveMotionFunc(callback);
    glutMouseFunc(mouseClicked);
    //glutReshapeFunc(reshape);
    /* Callback para para interagir com teclado */
    //glutKeyboardFunc(keyboard);
    printf("%s\n", glGetString(GL_VERSION));
    /* inicia o processamento dos eventos... */
    glutMainLoop();

}
