#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

int MyListID;

void MyCreateList() {
    MyListID = glGenLists(1);
    glNewList(MyListID, GL_COMPILE);

    // glColor3f(0.5, 0.5, 0.5);

    glBegin(GL_POLYGON);
    glVertex3f(-0.5, -0.5, 0.0);
    glVertex3f(0.5, -0.5, 0.0);
    glVertex3f(0.5, 0.5, 0.0);
    glVertex3f(-0.5, 0.5, 0.0);
    glEnd();

    glEndList();
}

void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 300, 300);

    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glRotatef(30, 0, 0, 1);
    glCallList(MyListID);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.0, 0.0, 1.0);
    glRotatef(30, 1, 0, 0);
    glTranslatef(0.3, 0, 0);
    glCallList(MyListID);
    glPopMatrix();

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(300, 300);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("πÈ¡ÿ«ı");

    glClearColor(1.0, 1.0, 1.0, 1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glutDisplayFunc(MyDisplay);

    MyCreateList();

    glutMainLoop();
    return 0;
}