#define _USE_MATH_DEFINES
#include "Object.h"
#include "glut.h"
#include <math.h>
#include <windows.h>
using namespace std;

int windowSize[2];

void light();
void display();
void keyboard(unsigned char key, int x, int y);
void idle();
void reshape(GLsizei, GLsizei);
void LoadModel(Object*);
void DrawBasis();
void DrawSphere1(float radius, float slice, float stack, float r, float g, float b);
void DrawSphere2(float radius, float slice, float stack, float r, float g, float b);
void AddDegree(int* degree_var, int degree);

int minute = 0, hour = 0;
int rotate_degree = 0, sun_degree = 0,pika_degree = 0, moon_degree = 0;
bool rotate_flag = false, sun_flag = false,pika_flag = false , moon_flag = false;
Object* elephant = new Object("elephant.obj");
Object* cube = new Object("cube.obj");
Object* teapot = new Object("teapot.obj");
Object* Clock = new Object("clock.obj");
Object* Pikachu = new Object("Pikachu.obj");
GLUquadricObj* bar = gluNewQuadric();

GLfloat default_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f};
GLfloat default_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f};


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("HW1");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void light()
{
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_position[] = { 10.0, 10.0, 5.0, 0.0 };
	glShadeModel(GL_SMOOTH);
	// z buffer enable
	glEnable(GL_DEPTH_TEST);
	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_specular);
	GLfloat mat_shininess[] = { 100 };
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], 1.0, 1000.0);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 15.0, 40.0,// eye
		0.0, 0.0, 0.0,     // center
		0.0, 1.0, 0.0);    // up

	light();

	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat clock_diffuse[] = { 1, 1, 1, 0.5 };
	GLfloat bar_diffuse[] = { 1, 0, 1, 0.5 };
	GLfloat pikachu_diffuse[] = { 1, 1, 0, 1 };

	// adjust model to face to front
	glPushMatrix();

	// rotate all clock 
	glPushMatrix();
	glRotatef(rotate_degree, 0, 1, 0);

	// render clock model
	glPushMatrix();
	glScalef(0.08f, 0.08f, 0.08f);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, clock_diffuse);
	LoadModel(Clock);
	glPopMatrix();

	// adjust hand to x-y surface
	glPushMatrix();
	glRotatef(90, 0, 1, 0);

	// render minute hand of clock
	glPushMatrix();
	glRotatef(minute, 1, 0, 0);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, bar_diffuse);
	gluCylinder(bar, 0.3, 0.3, 5, 30, 30);
	glPopMatrix();

	// render hour hand of clock
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glRotatef(hour, 1, 0, 0);
	gluCylinder(bar, 0.3, 0.3, 3, 30, 30);
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();

	// render pikachu
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pikachu_diffuse);
	glRotatef(moon_degree, 0, 1, 0);
	glTranslatef(12.0f, 0.0f, 0.0f);
	glRotatef(pika_degree, 0, 1, 0);
	glScalef(5.0f, 5.0f, 5.0f);
	LoadModel(Pikachu);
	glPopMatrix();
	// set amibent back to default
	glMaterialfv(GL_FRONT, GL_AMBIENT, default_ambient);

	//render sun
	glPushMatrix();
	glRotatef(sun_degree, 0, 0, 1);
	glTranslatef(0.0f, 18.0f, 0.0f);
	DrawSphere2(4, 240, 60, 1, 0, 0);
	glPopMatrix();

	//glPushMatrix();
	//glRotatef(moon_degree, 0, 1, 0);
	//glTranslatef(0.0f, 0.0f, 12.5f);
	//DrawSphere1(2,240,60,1,1,0);
	//glPopMatrix();

	glPopMatrix();

	minute++;
	minute %= 360;
	if (! (minute % 30)) {
		hour++;
		hour %= 360;
	}

	if (rotate_flag) {
		AddDegree(&rotate_degree, 1);
	}

	if (sun_flag) {
		AddDegree(&sun_degree,1);
	}

	if (pika_flag) {
		AddDegree(&pika_degree, 1);
	}

	if (moon_flag) {
		AddDegree(&moon_degree, 2);
	}
	//glutPostRedisplay();
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'r') {
		rotate_flag = !rotate_flag;
	}
	else if (key == 's') {
		sun_flag = !sun_flag;
	}
	else if (key == 'p') {
		pika_flag = !pika_flag;
	}
	else if (key == 'm') {
		moon_flag = !moon_flag;
	}
}

void idle() {
	glutPostRedisplay();
}

void LoadModel(Object* Model) {
	for (size_t i = 0; i < Model->fNum; ++i)
	{
		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j < 3; ++j)
		{
			glNormal3fv(Model->nList[Model->faceList[i][j].nIndex].ptr);
			glVertex3fv(Model->vList[Model->faceList[i][j].vIndex].ptr);
		}
		glEnd();
	}
}
void DrawBasis() {
	float edge = 20;
	float radius = 10;
	float pi = 3.1415926;

	glBegin(GL_POLYGON);
	for (int i = 0; i < edge; i++) {
		glNormal3d(0, 1, 0);
		glVertex3d(radius * sin(i / edge * 2 * pi), 0, radius * cos(i / edge * 2 * pi));
	}
	glEnd();

	for (int i = 0; i < edge; i++) {
		glBegin(GL_POLYGON);
		glNormal3d(sin(i / edge * 2 * pi), 0, cos(i / edge * 2 * pi));
		glVertex3d(radius * sin(i / edge * 2 * pi), 0, radius * cos(i / edge * 2 * pi));
		glVertex3d(radius * sin(i / edge * 2 * pi), -5, radius * cos(i / edge * 2 * pi));
		glVertex3d(radius * sin((i + 1) / edge * 2 * pi), -5, radius * cos((i + 1) / edge * 2 * pi));
		glVertex3d(radius * sin((i + 1) / edge * 2 * pi), 0, radius * cos((i + 1) / edge * 2 * pi));
		glEnd();
	}
	glBegin(GL_POLYGON);
	for (int i = 0; i < edge; i++) {
		glNormal3d(0, -1, 0);
		glVertex3d(radius * sin(i / edge * 2 * pi), -5, radius * cos(i / edge * 2 * pi));
	}
	glEnd();
}

void DrawSphere1(float radius, float slice, float stack, float r, float g, float b) {
	GLfloat mat_diffuse[] = { r, g, b, 0.5 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	float theta, phi, xy_step = 360 / slice, z_step = 180 / stack;
	float x, y, z;
	for (phi = -90; phi <= 90; phi += z_step) {
		glBegin(GL_TRIANGLE_STRIP);
		for (theta = 0; theta <= 360; theta += xy_step) {
			x = radius * sin(theta * M_PI / 180) * cos(phi * M_PI / 180);
			y = radius * cos(theta * M_PI / 180) * cos(phi * M_PI / 180);
			z = radius * sin(phi * M_PI / 180);
			glNormal3f(x / radius, y / radius, z / radius);
			glVertex3d(x, y, z);
			x = radius * sin(theta * M_PI / 180) * cos((phi + z_step) * M_PI / 180);
			y = radius * cos(theta * M_PI / 180) * cos((phi + z_step) * M_PI / 180);
			z = radius * sin((phi + z_step) * M_PI / 180);
			glNormal3f(x / radius, y / radius, z / radius);
			glVertex3d(x, y, z);
		}
		glEnd();
	}
}
void DrawSphere2(float radius, float slice, float stack, float r, float g, float b) {
	GLfloat mat_diffuse[] = { r, g, b, 0.5 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	float x, y, z;
	for (int j = 0; j <= stack; j++) {
		glBegin(GL_TRIANGLE_STRIP);
		for (int i = 0; i <= slice; i++) {
			float dx = i / slice;
			float dy = j / stack;
			x = radius * cos(2 * M_PI * dx) * sin(dy * M_PI);
			y = radius * sin(2 * M_PI * dx) * sin(dy * M_PI);
			z = radius * cos(dy * M_PI);
			glNormal3f(x / radius, y / radius, z / radius);
			glVertex3d(x, y, z);
			dy = (j + 1) / stack;
			x = radius * cos(2 * M_PI * dx) * sin(dy * M_PI);
			y = radius * sin(2 * M_PI * dx) * sin(dy * M_PI);
			z = radius * cos(dy * M_PI);
			glNormal3f(x / radius, y / radius, z / radius);
			glVertex3d(x, y, z);
		}
		glEnd();
	}
}
void AddDegree(int* degree_var, int degree) {
	*degree_var += degree;
	*degree_var %= 360;
}