#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
void MyDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.3, 0.3, 0.7);
	glutWireTeapot(1.0);
	glFlush();
}
void MyReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (double)w / (double)h, 1, 50);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(5, 5, 5, 0, 0, 0, 0, 1, 0);
	glutPostRedisplay();
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutCreateWindow("πÈ¡ÿ«ı");
	glClearColor(1, 1, 1, 1);
	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
	glutMainLoop();
	return 0;
}