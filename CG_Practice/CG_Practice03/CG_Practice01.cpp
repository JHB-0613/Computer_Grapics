#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>

GLfloat MyVertices[8][3] = {
	{-0.25, -0.25, 0.25 },
	{-0.25, 0.25, 0.25 },
	{ 0.25, 0.25, 0.25 },
	{ 0.25, -0.25, 0.25 },
	{-0.25, -0.25, -0.25 },
	{-0.25, 0.25, -0.25 },
	{ 0.25, 0.25, -0.25 },
	{ 0.25, -0.25, -0.25 }
};

GLfloat MyColors[8][3] = {
	{ 0.2, 0.2, 0.2 },
	{ 1.0, 0.0, 0.0 },
	{ 1.0, 1.0, 0.0 },
	{ 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 1.0 },
	{ 1.0, 0.0, 1.0 },
	{ 1.0, 1.0, 1.0 },
	{ 0.0, 1.0, 1.0 }
};

GLubyte MyVertexList[24] = {
	0, 3, 2, 1,
	2, 3, 7, 6,
	0, 4, 7, 3,
	1, 2, 6, 5,
	4, 5, 6, 7,
	0, 1, 5, 4
};

GLuint MyCubeListID;

void MyCreateList() {
	MyCubeListID = glGenLists(1);
	glNewList(MyCubeListID, GL_COMPILE);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glColorPointer(3, GL_FLOAT, 0, MyColors);
	glVertexPointer(3, GL_FLOAT, 0, MyVertices);

	for (GLint i = 0; i < 6; i++) {
		glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, &MyVertexList[4 * i]);
	}

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glEndList();
}

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glRotatef(30.0, 1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(-0.6, 0.0, 0.0);
	glCallList(MyCubeListID);
	glPopMatrix();

	glPushMatrix();
	glCallList(MyCubeListID);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.6, 0.0, 0.0);
	glCallList(MyCubeListID);
	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glFlush();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(300, 300);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("¹éÁØÇõ");

	glClearColor(1.0, 1.0, 1.0, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	MyCreateList();

	glutDisplayFunc(MyDisplay);

	glutMainLoop();
	return 0;
}