#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

int MyListID;

void MyCreateList() {
    MyListID = glGenLists(1);
    glNewList(MyListID, GL_COMPILE);
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

    // num 1
    glPushMatrix();
    glColor3f(1, 0, 0);
    glRotatef(30, 0, 0, 1);
    glCallList(MyListID);
    glPopMatrix();

    // num 2
    glPushMatrix();
    glColor3f(0, 1, 0); 
    glRotatef(30, 0, 0, 1); 
    glTranslatef(2.0, 0, 0); 
    glCallList(MyListID); 
    glPopMatrix(); 

    // num 3
    glPushMatrix();
    glColor3f(0, 0, 1); 
    glTranslatef(2.0, 0, 0); 
    glRotatef(30, 0, 0, 1);
    glCallList(MyListID); 
    glPopMatrix(); 

    // num 4
    glPushMatrix(); 
    glColor3f(1, 1, 0); 
    glTranslatef(-2.0, 0, 0); 
    glRotatef(30, 0, 0, 1);
    glScalef(2, 2, 0); 
    glCallList(MyListID); 
    glPopMatrix();

    glFlush();
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(300, 300);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("백준혁");

    glClearColor(1.0, 1.0, 1.0, 1.0); 

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-4.0, 4.0, -4.0, 4.0, 1.0, -1.0);

    glutDisplayFunc(MyDisplay);
    MyCreateList();
    glutMainLoop();
    return 0;
}