#include <GL/glut.h> // or <GL/freeglut.h>
void mydisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_QUADS);
		glVertex2f(-0.5, -0.5);
		glVertex2f(-0.5, 0.5);
		glVertex2f(0.5, 0.5);
		glVertex2f(0.5, -0.5);
	glEnd();
	glFlush();
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutCreateWindow("20223651 백준혁");
	glutDisplayFunc(mydisplay);
	glutMainLoop();
}