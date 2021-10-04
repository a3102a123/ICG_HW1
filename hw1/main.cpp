#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Object.h"
#include "glut.h"
#include <math.h>
#include <windows.h>
using namespace std;

class Vertex {
	public:
		float x;
		float y;
		float z;
		void normalize() {
			float magnitude;
			magnitude = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
			*this = *this / magnitude;
		}
		Vertex operator + (Vertex const& obj) {
			Vertex temp;
			temp.x = this->x + obj.x;
			temp.y = this->y + obj.y;
			temp.z = this->z + obj.z;
			return temp;
		}
		Vertex operator - (Vertex const& obj) {
			Vertex temp;
			temp.x = this->x - obj.x;
			temp.y = this->y - obj.y;
			temp.z = this->z - obj.z;
			return temp;
		}
		Vertex operator / (float const& num) {
			Vertex temp;
			temp.x = this->x / num;
			temp.y = this->y / num;
			temp.z = this->z / num;
			return temp;
		}
		friend ostream& operator<<(ostream& os, const Vertex& v) {
			os << v.x << "," << v.y << "," << v.z;
			return os;
		}
};

int windowSize[2];
unsigned int texture;
int width, height, nrChannels;
unsigned char* pikachuTexture;

void light();
void display();
void keyboard(unsigned char key, int x, int y);
void idle();
void reshape(GLsizei, GLsizei);
void InitTexture();
void LoadModel(Object*, bool is_Pikachu);
void DrawBase();
void DrawSphere1(float radius, float slice, float stack, float r, float g, float b);
void DrawSphere2(float radius, float slice, float stack, float r, float g, float b);
void AddDegree(int* degree_var, int degree);

int minute = 0, hour = 0;
int rotate_degree = 0, pika_degree = 0, moon_degree = 0;
bool rotate_flag = false, pika_flag = false , moon_flag = false;
Object* Clock = new Object("clock.obj");
Object p("Pikachu.obj");
Object* Pikachu = &p;
GLUquadricObj* bar = gluNewQuadric();

GLfloat default_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f};
GLfloat default_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f};


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("HW1");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	InitTexture();
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

	GLfloat clock_diffuse[] = { 1, 1, 1, 1 };
	GLfloat l_hand_diffuse[] = { 1, 0, 1, 1 };
	GLfloat s_hand_diffuse[] = { 0, 1, 1, 1 };
	GLfloat base_diffuse[] = { 1, 0, 0, 1 };

	// rotate & translate whole clock & base
	glPushMatrix();
	glRotatef(rotate_degree, 0, 1, 0);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, base_diffuse);
	DrawBase();
	glTranslatef(0.0f, 7.5f, 0.0f);

	// render clock model
	glPushMatrix();
	glScalef(0.08f, 0.08f, 0.08f);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, clock_diffuse);
	LoadModel(Clock,false);
	glPopMatrix();

	// adjust hand to x-y surface
	glPushMatrix();
	glRotatef(90, 0, 1, 0);

	// render minute hand of clock
	glPushMatrix();
	//glRotatef(minute, 1, 0, 0);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, l_hand_diffuse);
	gluCylinder(bar, 0.3, 0.3, 5, 30, 30);
	glPopMatrix();

	// render hour hand of clock
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	//glRotatef(hour, 1, 0, 0);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s_hand_diffuse);
	gluCylinder(bar, 0.3, 0.3, 3, 30, 30);
	glPopMatrix();

	glPopMatrix();

	glPopMatrix();

	// render pikachu
	glPushMatrix();
	glRotatef(moon_degree, 0, 1, 0);
	glTranslatef(14.0f, 0.0f, 0.0f);
	glRotatef(45, 0, 1, 0);
	glRotatef(pika_degree, 0, 1, 0);
	glScalef(10.0f, 10.0f, 10.0f);
	LoadModel(Pikachu,true);
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

	if (pika_flag) {
		AddDegree(&pika_degree, 1);
	}

	if (moon_flag) {
		AddDegree(&moon_degree, 2);
	}
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

void InitTexture() {
	stbi_set_flip_vertically_on_load(true);
	pikachuTexture = stbi_load("Pikachu.png", &width, &height, &nrChannels, 0);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pikachuTexture);
	glDisable(GL_TEXTURE_2D);
}

void LoadModel(Object* Model,bool is_Pikachu) {
	if (is_Pikachu) {
		GLfloat pikachu_diffuse[] = { 1, 1, 0, 1 };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pikachu_diffuse);
		glEnable(GL_TEXTURE_2D);
	}
	for (size_t i = 0; i < Model->fNum; ++i)
	{
		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j < 3; ++j)
		{
			if (is_Pikachu) {
				int idx = i * 3 * 2 + j * 2;
				glTexCoord2f(Model->texcoords[idx],Model->texcoords[idx+1]);
			}
			glNormal3fv(Model->nList[Model->faceList[i][j].nIndex].ptr);
			glVertex3fv(Model->vList[Model->faceList[i][j].vIndex].ptr);
		}
		glEnd();
	}
	if (is_Pikachu) {
		glDisable(GL_TEXTURE_2D);
		// set amibent back to default
		glMaterialfv(GL_FRONT, GL_AMBIENT, default_ambient);
	}
}
void DrawBase() {
	float edge = 5;
	float radius = 10;
	float tip_radius = radius * sqrt(3);
	float adjust_angle = 36 * M_PI / 180.0f;
	vector<Vertex> centerPolygon;
	vector<Vertex> starTip;
	
	//// calculate the vertex position of star base
	Vertex temp{0,0,0};
	// calculate the center Pentagon
	for (int i = 0; i < edge; i++) {
		temp.x = radius * sin(i / edge * 2 * M_PI);
		temp.y = 0;
		temp.z = radius * cos(i / edge * 2 * M_PI);
		centerPolygon.push_back(temp);
	}
	// calculate the tip position of star
	for (int i = 0; i < edge; i++) {
		temp.x = tip_radius * sin(i / edge * 2 * M_PI + adjust_angle);
		temp.y = 0;
		temp.z = tip_radius * cos(i / edge * 2 * M_PI + adjust_angle);
		starTip.push_back(temp);
	}

	//// render base
	// draw the star polygon on top
	glBegin(GL_POLYGON);
	for (int i = 0; i < edge; i++) {
		glNormal3d(0, 1, 0);
		glVertex3d(centerPolygon[i].x, centerPolygon[i].y, centerPolygon[i].z );
	}
	glEnd();

	for (int i = 0; i < edge; i++) {
		glBegin(GL_TRIANGLES);
		glNormal3d(0, 1, 0);
		glVertex3d(starTip[i].x,starTip[i].y,starTip[i].z);
		glNormal3d(0, 1, 0);
		glVertex3d(centerPolygon[i].x, centerPolygon[i].y, centerPolygon[i].z);
		int idx = (i + 1) % (int)edge;
		glNormal3d(0, 1, 0);
		glVertex3d(centerPolygon[idx].x, centerPolygon[idx].y, centerPolygon[idx].z);
		glEnd();
	}

	// draw the side of base
	for (int i = 0; i < edge; i++) {
		for (int j = 0; j < 2; j++) {
			int idx = i;
			Vertex v1, v2,v3,center,normal;
			if (j) {
				idx = (idx + 1) % (int)edge;
				v3 = centerPolygon[i];
			}
			else {
				v3 = centerPolygon[(idx + 1) % (int)edge];
			}
			v1 = centerPolygon[idx];
			v2 = starTip[i];
			center = (v2 + v1) / 2;
			normal = center - v3;
			normal.normalize();
			glBegin(GL_POLYGON);
			glNormal3d(normal.x,normal.y,normal.z);
			glVertex3d(v1.x, v1.y, v1.z);
			glVertex3d(v2.x, v2.y, v2.z);
			glVertex3d(v2.x, -5, v2.z);
			glVertex3d(v1.x, -5, v1.z);
			glEnd();
		}
	}

	// draw the star polygon on bottom
	glBegin(GL_POLYGON);
	for (int i = 0; i < edge; i++) {
		glNormal3d(0, -1, 0);
		glVertex3d(centerPolygon[i].x, -5, centerPolygon[i].z);
	}
	glEnd();

	for (int i = 0; i < edge; i++) {
		glBegin(GL_TRIANGLES);
		glNormal3d(0, -1, 0);
		glVertex3d(starTip[i].x, -5, starTip[i].z);
		glNormal3d(0, -1, 0);
		glVertex3d(centerPolygon[i].x, -5, centerPolygon[i].z);
		int idx = (i + 1) % (int)edge;
		glNormal3d(0, -1, 0);
		glVertex3d(centerPolygon[idx].x, -5, centerPolygon[idx].z);
		glEnd();
	}
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