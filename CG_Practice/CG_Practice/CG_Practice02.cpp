#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.5, 0.5, 0.5); //È¸»ö
	glBegin(GL_POLYGON); //»ç°¢Çü
	glVertex3f(-0.5, -0.5, 0.0); //ÁÂÇÏ´Ü ÁÂÇ¥
	glVertex3f(0.5, -0.5, 0.0); //ÁÂÇÏ´Ü ÁÂÇ¥
	glVertex3f(0.5, 0.5, 0.0); //¿ì»ó´Ü ÁÂÇ¥
	glVertex3f(-0.5, 0.5, 0.0); //ÁÂ»ó´Ü ÁÂÇ¥
	glEnd();
	glFlush();
}
void MyReshape(int NewWidth, int NewHeight) {
	glViewport(0, 0, NewWidth, NewHeight);
	GLfloat WidthFactor = (GLfloat)NewWidth / (GLfloat)300;
	GLfloat HeightFactor = (GLfloat)NewHeight / (GLfloat)300;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0 * WidthFactor, 1.0 * WidthFactor, -1.0 * HeightFactor, 1.0 * HeightFactor, -1.0, 1.0);
}
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(300, 300);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("¹éÁØÇõ");
	glClearColor(1.0, 1.0, 1.0, 1.0); //ÃÊ±âÈ­ »öÀº ¹é»ö
	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
	glutMainLoop();
	return 0;
}
