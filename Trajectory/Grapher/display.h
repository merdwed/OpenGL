#define _CRT_SECURE_NO_WARNINGS 1
#include <GL/glut.h>
#pragma comment(lib,"freeglut.lib")

#include <stdio.h>
//#include <stdlib.h>


#include <process.h>
#include <time.h>
#include <math.h>
struct coord {
	float x;
	float y;
	float z;
};

extern const coord  deltaCam , deltaMove ;
struct textureData
{
	GLsizei sizeX;
	GLsizei sizeY;
	GLubyte* data;
};
extern coord track[1000000];
extern int Time_MGA[1000000];
extern GLuint texture[3];//массив для хранения текстурок
				  //94.95 метра 
extern bool keychar[256];
extern bool keyint[256]; 
extern float mapAngleNorth;
extern float mapAngle;
//extern float deltaAngleMap;
extern float verticalAngleNorth;
extern float horizontalAngleNorth;
extern coord color_track;
extern int delta_i;
extern coord cam, direct, angleCam, angle_rate;
extern bool real_time;
extern bool show_line;
extern bool draw_map_var;
extern int current_i;
extern int global_i;

textureData* LoadBMP(const char* imagepath);
GLvoid LoadGLTextures();
GLvoid InitGL();
void draw_map();
void draw_symbols();
void draw_lines();
void calculateScene();
void Display(void);