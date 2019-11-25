#define _CRT_SECURE_NO_WARNINGS 1

#include "display.h"

GLuint texture[3];//массив для хранения текстурок
				  //94.95 метра 
const xyz_structure deltaCam = { 0.025,0.025,0.025 }, deltaMove = { 0.4,0.1,0.1 };
bool keychar[256];
bool keyint[256];
trajectory track[16];
int number_of_track = 0;
float mapAngleNorth = 0;
float mapAngle = 0;
//float deltaAngleMap = 0;
float verticalAngleNorth = 0;
float horizontalAngleNorth = 0;
xyz_structure color_track = { 0,1,0 };
int delta_i = 0;
xyz_structure cam = { 0,1,5 }, direct = { 0,0,-1 }, angleCam = { 0,0,0 }, angle_rate = { 0,0,0 };
//bool real_time = true;
//bool show_line = true;
bool draw_map_var = true;
//int current_i = 0;
//int global_i = 0;


GLvoid LoadGLTextures()
{
	glEnable(GL_TEXTURE_2D);// Разрешение наложение текстуры
	glGenTextures(3, &texture[0]);

	textureData* textureA[3];
	
	if (textureA[0] = LoadBMP("map.bmp"))
	{
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, textureA[0]->sizeX, textureA[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, textureA[0]->data);
		free(textureA[0]->data);
		free(textureA[0]);
	}
	if (textureA[1] = LoadBMP("white.bmp"))
	{
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, textureA[1]->sizeX, textureA[1]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, textureA[1]->data);
		free(textureA[1]->data);
		free(textureA[1]);
	}
	if (textureA[2] = LoadBMP("font-texture.bmp"))
	{
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, textureA[2]->sizeX, textureA[2]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, textureA[2]->data);
		free(textureA[2]->data);
		free(textureA[2]);
	}
	




}
GLvoid InitGL()
{
	LoadGLTextures();// Загрузка текстур
	glEnable(GL_TEXTURE_2D);// Разрешение наложение текстуры
	glClearColor(0, 0, 0, 0);
	glClearDepth(1);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}
void draw_symbols()
{
	glPushMatrix();
	glColor3f(0, 0, 0);
	glRasterPos3f(-0.0004, 0, -0.1);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '+');
	glRasterPos3f(0.015, -0.02, -0.1);
	char str_for_character[50];
	sprintf(str_for_character, "%0.3f", (float)track[0].points[track[0].index].t / 1000.0);
	for (int i = 0; i < strlen(str_for_character); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str_for_character[i]);
	glRasterPos3f(0.02, -0.02, -0.1);
	/*if (real_time)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'R');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'e');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'a');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'l');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 't');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'i');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'm');
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'e');


	}*/
	sprintf(str_for_character, "%f", track[0].points[track[0].index].coord.y);
	glRasterPos3f(-0.024, -0.02, -0.1);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'H');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'e');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'i');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'g');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'h');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 't');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ':');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
	for (int i = 0; i < strlen(str_for_character); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str_for_character[i]);
	//sprintf(str_for_character, "%f", abs(sqrt(axcel[current_i - 1].x*axcel[current_i - 1].x + axcel[current_i - 1].y*axcel[current_i - 1].y + axcel[current_i - 1].z*axcel[current_i-1].z) - 0.982));
	glRasterPos3f(-0.024, 0.01, -0.1);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'a');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ':');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
	for (int i = 0; i < strlen(str_for_character); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str_for_character[i]);
	glPopMatrix();
}
void draw_map() {
	glPushMatrix();
	glRotatef(mapAngleNorth + mapAngle, 0, 1, 0);
	// нарисуем "землю"
	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(1,0);
	//glTexxyz_structure2f(1, 0); 
	glVertex3f(27.929 - 0.5, 0, 24.929 - 0.5); //право низ 
	glTexCoord2f(0, 0);
	//glTexxyz_structure2f(0, 0); 
	glVertex3f(-23.929 - 0.5, 0, 24.929 - 0.5);//лево  Низ
	glTexCoord2f(0, 1);
	//glTexxyz_structure2f(0, 1); 
	glVertex3f(-23.929 - 0.5, 0, -26.929 - 0.5); //лево Верх 
	glTexCoord2f(1, 1);
	//glTexxyz_structure2f(1, 1); 
	glVertex3f(27.929 - 0.5, 0, -26.929 - 0.5);  //право Верх 
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glPopMatrix();
}
void draw_lines()
{
	glPushMatrix();
	glRotatef(mapAngleNorth + mapAngle, 0, 1, 0);
	glBegin(GL_LINES);
	glColor3d(0, 0, 1);
	for (int i = -30; i < 30; i++)
		for (int j = -30; j < 30; j++)
			glVertex3f(i, 0, -30), glVertex3f(i, 0, 30), glVertex3f(-30, 0, j), glVertex3f(30, 0, j);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glRotatef(mapAngle, 0, 1, 0);
	glBegin(GL_LINES);
	glColor3f(0, 0, 0);
	glVertex3d(0, 0.5, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0.5, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 0.5);
	glVertex3d(0, 0, 0);
	glEnd();
	glPopMatrix();
}
void calculateScene()
{
	/*if (!real_time)
	{
		current_i += delta_i;
		if (current_i < 0)current_i = 0;
		if (current_i > global_i - 1)
		{
			current_i = global_i;
			real_time = true;
		}
		//angle[current_i].y = angle[current_i-1].y + gyro[current_i].y*(float)(Time_MGA[current_i] - Time_MGA[current_i - 1]) / 1000.0 + (gyro[current_i].y - gyro[current_i - 1].y)*(float)(Time_MGA[current_i] - Time_MGA[current_i - 1]) / 2000.0;
		//angle[current_i].x = angle[current_i - 1].x + gyro[current_i].x*(float)(Time_MGA[current_i] - Time_MGA[current_i - 1]) / 1000.0 + (gyro[current_i].x - gyro[current_i - 1].x)*(float)(Time_MGA[current_i] - Time_MGA[current_i - 1]) / 2000.0;
		//angle[current_i].z = angle[current_i - 1].z + gyro[current_i].z*(float)(Time_MGA[current_i] - Time_MGA[current_i - 1]) / 1000.0 + (gyro[current_i].z - gyro[current_i - 1].z)*(float)(Time_MGA[current_i] - Time_MGA[current_i - 1]) / 2000.0;
	}
	else
		current_i = global_i;*/
	/*if (deltaMove.z)
	{
		cam.z += deltaMove.z * 0.1;
		if (cam.z > 100 || cam.z < 0)
			cam.z -= deltaMove.z * 0.1;
	}*/
	/*if (deltaMove.y)
	{
		cam.y += deltaMove.y * 0.1;
		if (cam.y > 100 || cam.y < -20)
			cam.y -= deltaMove.y * 0.1;
	}*/

	/*if (deltaCam.x || deltaCam.y)
	{
		angleCam.x += deltaCam.x;
		if (angleCam.x > 1.57 || angleCam.x < -1.57)
			angleCam.x -= deltaCam.x;
		angleCam.y += deltaCam.y;
		//if(angleCam.y>1.57 || angleCam.y<-1.57)
		//	angleCam.y -= deltaCam.y;
		direct.z = -cos(angleCam.x) * cos(angleCam.y);
		direct.y = sin(angleCam.x);
		direct.x = cos(angleCam.x) * sin(angleCam.y);
	}*/
}
void Display(void) {
	calculateScene();
	glClearColor(0.3, 0.8, 0.8, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//очистить буфер цвета и глубины
	glLoadIdentity();// обнулить трансформацию



	draw_symbols();
	gluLookAt(cam.x, cam.y, cam.z,
		cam.x + direct.x, cam.y + direct.y, cam.z + direct.z,
		0, 1, 0);// установить камеру 
				 /*gluLookAt(cam.x, cam.y, cam.z,
				 cam.x + magnito[global_i-1].z, cam.y - magnito[global_i-1].y, cam.z + magnito[global_i-1].z,
				 0, 1, 0);*/



	glLineWidth(1);
	//if (show_line)
	//	draw_lines();
	if (draw_map_var)
		draw_map();


	glRotatef(mapAngle, 0, 1, 0);
	glPushMatrix();
	glBegin(GL_LINE_STRIP);
	glColor3f(1, 0, 0);

	glLineWidth(2);
	for (int h = 0; h < TRACK_MAX; h++)
	{
		if(track[h].active)
		for (int i = 1; i < track[h].index; i++)
		{
			glColor3f(color_track.x, color_track.y, color_track.z);
			glVertex3f(track[h].points[i].coord.x / 10, /*htv[i].x / 10*/track[h].points[i].coord.y / 10, track[h].points[i].coord.z / 10);
			glColor3f(0, 0, 0);
			glVertex3f(track[h].points[i].coord.x / 10, /*htv[i].x / 10+0.1*/track[h].points[i].coord.y / 10 + 0.01, track[h].points[i].coord.z / 10);
		}
	}
	glEnd();

	//glBegin(GL_POINTS);
	//glPointSize(200);
	//glColor3f(1, 0, 0);
	//glVertex3d(0, 3, 0);
	//glVertex3f(track[current_i].x / 10, htv[current_i].x / 10/*track[i].y/10+0.5*/, track[current_i].z / 10);
	//glEnd();






	/*glPushMatrix();
		glTranslatef(0, 10, 0);

		glLineWidth(0.5);
		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		glVertex3d(0, 0.5, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0.5, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 0.5);
		glVertex3d(0, 0, 0);
		glEnd();
		glTranslatef(0, -5, 0);
		/*glBegin(GL_LINES);
		glVertex3d(0, 0.5, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0.5, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 0.5);
		glVertex3d(0, 0, 0);
		glEnd();

		glBegin(GL_LINE_STRIP);
		glColor3f(0,1,0);
		glPointSize(500);
		for(int i=0;i<global_i;i++)
		glVertex3f(magnito[i].x,magnito[i].y,magnito[i].z);
		glEnd();
		glBegin(GL_LINES);
		glColor3f(0,1,0);
		glVertex3f(0,0,0);
		glVertex3d(magnito[current_i].x,magnito[current_i].y,magnito[current_i].z);
		glEnd();/

		glTranslatef(0, 5, 0);
		//glBegin(GL_LINE_STRIP);
		glColor3f(0, 0, 0);
		glPointSize(1);
		//for (int i = 0; i<current_i; i++)
		//	glVertex3f(gyro[i].x, gyro[i].y, gyro[i].z);
		//glEnd();
		//glBegin(GL_LINES);
		glColor3f(0, 1, 0);
		//glVertex3f(0, 0, 0);
		//glVertex3d(gyro[current_i].x, gyro[current_i].y, gyro[current_i].z);
		//glEnd();

	glPopMatrix();*/
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	/*glLineWidth(0.5);
	glPushMatrix();
		glTranslatef(0, 5, 0);
		glRotatef(90, 0, 0, 1);
		glRotatef(angle[current_i - 1].x, 1, 0, 0);
		glRotatef(angle[current_i - 1].z, 0, 1, 0);
		glRotatef(-angle[current_i - 1].y, 0, 0, 1);
		glColor3f(0.5, 0.5, 0.5);
		glBegin(GL_LINES);
		xyz_structure angle_r={0,0,0};
		//angle_r.x=angle.x/360*2*3.1415;
		//angle_r.y=angle.y/360*2*3.1415;
		//angle_r.z=angle.z/360*2*3.1415;
		glVertex3f(0,0,0);
		glVertex3f(axcel[current_i].y*cos(angle_r.x)*sin(angle_r.z)*cos(-angle_r.y)+axcel[current_i].x*cos(angle_r.z)*cos(-angle_r.y)-axcel[current_i].z*cos(angle_r.x)*sin(-angle_r.y),0,0);
		glVertex3f(0,0,0);
		glVertex3f(0,axcel[current_i].y*sin(angle_r.x)*cos(angle_r.z) - axcel[current_i].x*cos(angle_r.z) + axcel[current_i].z*sin(angle_r.x)*cos(angle_r.z),0);
		glVertex3f(0,0,0);
		glVertex3f(0,0,axcel[current_i].y*cos(angle_r.x)*sin(angle_r.z)*sin(-angle_r.y)+axcel[current_i].x*cos(angle_r.z)*sin(-angle_r.y)-axcel[current_i].z*cos(angle_r.x)*cos(-angle_r.y));
		//glVertex3d(gyro[current_i].x,gyro[current_i].y,gyro[current_i].z);
		glEnd();
		glutSolidTeapot(0.5);
	glPopMatrix();*/


	glPopMatrix();


	glutSwapBuffers();
}