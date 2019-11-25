#define _CRT_SECURE_NO_WARNINGS 1
#define TRACK_MAX 16
#define JUMP_FOR_DYNAMIC_ARRAY_OF_POINTS 1000;
#define EXTRA_LINE_LENGTH 64
#include <GL/glut.h>
#pragma comment(lib,"freeglut.lib")
#include <stdio.h>
//#include <stdlib.h>


#include <process.h>
#include <time.h>
#include <math.h>
struct xyz_structure {
	float x;
	float y;
	float z;
};
struct point_structure {
	xyz_structure coord;			// ���������� �����

	xyz_structure a;				// ������ ���������					 // ��������������� ������
	xyz_structure vec;				// ������ �����������				 // ��������� �� �����
	float t;						// ������							 // �� ������ �� ��� ����������
	char str[EXTRA_LINE_LENGTH];					// �������������� ������ ������������//

};
// ��������� ����������, �� ��������� �� ������� � ������ � ������ �����, 
struct trajectory {
	
	int number;				// ���������� �����
	int size_of_array;		//������ �������, ��� ������������� ���������
	int index;				// ������, �������� ������� ��������� ��� ���������
	point_structure* points;// ������ ����� � �������
	xyz_structure start_position;// ���������� ������
	xyz_structure color;
	bool active;			// ������� �� ������� ���������� ��������	1 ��, 0 ���
	bool show;				// ���������� ����������					1 ��, 0 ���
	char name[64];
};
// ��������� �����,


extern const xyz_structure  deltaCam , deltaMove ;
struct textureData
{
	GLsizei sizeX;
	GLsizei sizeY;
	GLubyte* data;
};

extern trajectory track[TRACK_MAX];
extern int number_of_track;
extern GLuint texture[3];// ������ ��� �������� ���������
				  //94.95 ����� 
extern bool keychar[256];
extern bool keyint[256]; 
extern float mapAngleNorth;
extern float mapAngle;
//extern float deltaAngleMap;
extern float verticalAngleNorth;
extern float horizontalAngleNorth;
extern xyz_structure color_track;
extern int delta_i;
extern xyz_structure cam, direct, angleCam, angle_rate;
//extern bool real_time;
//extern bool show_line;
extern bool draw_map_var;
//extern int current_i;
//extern int global_i;

textureData* LoadBMP(const char* imagepath);
GLvoid LoadGLTextures();
GLvoid InitGL();
void draw_map();
void draw_symbols();
void draw_lines();
void calculateScene();
void Display(void);