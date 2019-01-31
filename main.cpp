#if _WIN32
  #ifndef _WIN32
    #include <windows.h>
  #endif
#endif
#include <iostream>
#include <algorithm>
using namespace std;
#include <GL/freeglut.h>
#include <cmath>

const int WIDTH = 500, HEIGHT = 800;
const bool pixel = true, bresenhamMode = true;
const double squareSizeDiv2 = 0.5;

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

Point startPoints[2];
double cornerRadious;
int pointsReceived = 0;

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    gluOrtho2D(0, WIDTH, 0, HEIGHT);
}

void drawPoint(double x, double y)
{
    if (pixel) glVertex2d(x, y);
    else glRectd(x - squareSizeDiv2, y - squareSizeDiv2, x + squareSizeDiv2, y + squareSizeDiv2);
}

void bresenham(Line line)
{
    glBegin(GL_POINTS);
    Line aux = line;
    // If the line is steeper, we better swap x by y and continue drawing going up
    bool steep = abs(line.pf.y - line.pi.y) > abs(line.pf.x - line.pi.x);
    if (steep) swap(line.pi.x, line.pi.y), swap(line.pf.x, line.pf.y);
    if (line.pi.x > line.pf.x) swap(line.pi.x, line.pf.x), swap(line.pi.y, line.pf.y);
//    printf("(%3.3lf, %3.3lf) -> (%3.3lf, %3.3lf)\n", line.pi.x, line.pi.y, line.pf.x, line.pf.y);

    int dx, dy, E, NE, d, x, y, yStep = line.pi.y > line.pf.y ? -1 : 1;
    dx = line.pf.x - line.pi.x, dy = line.pf.y - line.pi.y;
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
    line = aux;
    glEnd();
}
void lineEquation(Line line)
{
    // y = m*x + b
    // m = (y1 - y0)/(x1 - x0)
    // b = 0 (in this case)
    bool steep = abs(line.pf.y - line.pi.y) > abs(line.pf.x - line.pi.x);
    if (steep) swap(line.pi.x, line.pi.y), swap(line.pf.x, line.pf.y);
    if (line.pi.x > line.pf.x) swap(line.pi.x, line.pf.x), swap(line.pi.y, line.pf.y);
    double m = (line.pf.y - line.pi.y) / (line.pf.x - line.pi.x);
    for (double x = line.pi.x, y = line.pi.y; x <= line.pf.x; x ++)
    {
        if (steep) drawPoint(y, x); else drawPoint(x, y);
        y = m * x;
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
    glBegin(GL_POINTS);
    int x = 0, y = circle.radious;
    double d;
    d = (double) 5 / 4 - circle.radious;

    circlePoints(x, y, q1, q2, q3, q4);
    while (y > x)
    {
        if (d < 0) d += 2 * x + 3, x ++;
        else d += 2 * (x - y) + 5, x ++, y --;
        circlePoints(x, y, q1, q2, q3, q4);
    }
    glEnd();
    glPopMatrix();
}

void drawLine(Line line)
{
    if (bresenhamMode) bresenham(line);
    else lineEquation(line);
}
void drawCircle(Circle circle, bool q1, bool q2, bool q3, bool q4)
{
    if (bresenhamMode) midPointCircle(circle, q1, q2, q3, q4);
    else;
}

double rot = 0;
int factor = 1;
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);

    if (ready) {
        glColor3ub(0, 0, 255);
        drawLine({{startPoints[0].x + cornerRadious, startPoints[0].y}, {startPoints[1].x - cornerRadious, startPoints[0].y}});
        drawLine({{startPoints[1].x, startPoints[0].y - cornerRadious}, {startPoints[1].x, startPoints[1].y + cornerRadious}});
        drawLine({{startPoints[1].x - cornerRadious, startPoints[1].y}, {startPoints[0].x + cornerRadious, startPoints[1].y}});
        drawLine({{startPoints[0].x, startPoints[1].y + cornerRadious}, {startPoints[0].x, startPoints[0].y - cornerRadious}});

        double displacement = cornerRadious;

        drawCircle({{startPoints[1].x - displacement, startPoints[0].y - displacement}, cornerRadious}, true, false, false, false);
        drawCircle({{startPoints[0].x + displacement, startPoints[0].y - displacement}, cornerRadious}, false, true, false, false);
        drawCircle({{startPoints[0].x + displacement, startPoints[1].y + displacement}, cornerRadious}, false, false, true, false);
        drawCircle({{startPoints[1].x - displacement, startPoints[1].y + displacement}, cornerRadious}, false, false, false, true);
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
            startPoints[pointsReceived++] = {(double) x, (double) y};
            cornerRadious = (abs(startPoints[0].x - startPoints[1].x) / 2.0 + abs(startPoints[0].y - startPoints[1].y) / 2.0) / 5.0;
            if (pointsReceived == 2)
                ready = true;
        }
        printf("Start Points: %d %d and %d %d | cornerRadious: %lf\n", startPoints[0].x, startPoints[0].y, startPoints[1].x,
               startPoints[1].y, cornerRadious);
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