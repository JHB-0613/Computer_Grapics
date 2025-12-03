#include <GL/glut.h>
GLsizei winWidth = 400, winHeight = 300; // Initial display-window size.
void init(void)
{
	glClearColor(0.0, 0.0, 1.0, 1.0); // Set display-window color to blue.
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 200.0, 0.0, 150.0);
}
void plotPoint(GLint x, GLint y) {
	glBegin(GL_POINTS); glVertex2i(x, y); glEnd();
}
void displayFcn(void) {
	glClear(GL_COLOR_BUFFER_BIT); // Clear display window.
	glColor3f(1.0, 0.0, 0.0); // Set point color to red.
	glPointSize(3.0); // Set point size to 3.0.
}
/* Move cursor while pressing c key enables freehand curve drawing. */
void curveDrawing(GLubyte curvePlotKey, GLint xMouse, GLint yMouse) {
	GLint x = xMouse;
	GLint y = winHeight - yMouse;
	switch (curvePlotKey) {
	case 'c':
		plotPoint(x, y); break;
	default:
		break;
	}
	glFlush();
}
void winReshapeFcn(int newWidth, int newHeight) {
	glViewport(0, 0, newWidth, newHeight); /* Reset viewport and projection
	parameters */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, GLdouble(newWidth), 0.0, GLdouble(newHeight));
	winWidth = newWidth; /* Reset display-window size parameters. */
	winHeight = newHeight;
}
void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("πÈ¡ÿ«ı");
	init();
	glutDisplayFunc(displayFcn);
	glutReshapeFunc(winReshapeFcn);
	glutKeyboardFunc(curveDrawing);
	glutMainLoop();
}