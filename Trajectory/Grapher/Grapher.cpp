


#include "display.h"

//#pragma comment(lib,"legacy_stdio_definitions.lib")

//float angle = 0.0;// угол поворота камеры
// XZ позиция камеры
//float x = 0, z = 5, y=1;

bool init_vertical = false;
int time_start = 0;




//cam координаты камеры, direct сферические координаты, angleCam поворот камеры, deltaCam временная переменная, deltaMove временно для движения
textureData* LoadBMP(const char* imagepath)
{
	Sleep(100);
	// Данные, прочитанные из заголовка BMP-файла
	unsigned char header[54]; // Каждый BMP-файл начинается с заголовка, длиной в 54 байта
	unsigned int dataPos;     // Смещение данных в файле (позиция данных)
	unsigned int width, height;
	unsigned int imageSize;   // Размер изображения = Ширина * Высота * 3
	// RGB-данные, полученные из файла
	unsigned char* data;
	FILE* file = fopen(imagepath, "rb");
	if (!file) {
		printf("Изображение не может быть открыто\n");
		return 0;
	}
	if (fread(header, 1, 54, file) != 54) { // Если мы прочитали меньше 54 байт, значит возникла проблема
		printf("Некорректный BMP-файл\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Некорректный BMP-файл\n");
		return 0;
	}
	// Читаем необходимые данные
	dataPos = *(int*) & (header[0x0A]); // Смещение данных изображения в файле
	imageSize = *(int*) & (header[0x22]); // Размер изображения в байтах
	width = *(int*) & (header[0x12]); // Ширина
	height = *(int*) & (header[0x16]); // Высота
	// Некоторые BMP-файлы имеют нулевые поля imageSize и dataPos, поэтому исправим их
	if (imageSize == 0)    imageSize = width * height * 3; // Ширину * Высоту * 3, где 3 - 3 компоненты цвета (RGB)
	if (dataPos == 0)      dataPos = 54; // В таком случае, данные будут следовать сразу за заголовком
	// Создаем буфер
	data = new unsigned char[imageSize];
	int a=ftell(file);
	fseek(file, dataPos, SEEK_SET);
	a = ftell(file);
	// Считываем данные из файла в буфер
	fread(data, 1, imageSize, file);

	// Закрываем файл, так как больше он нам не нужен
	fclose(file);
	textureData* p = new textureData();
	p->sizeX = width;
	p->sizeY = height;
	p->data = data;
	return p;
}
void Reshape(int w, int h) {
	// предотвращение деления на ноль
	int width = w, height = h;
	if (h == 0)
		h = 1;
	float ratio = w * 1 / h;
	// используем матрицу проекции

	glMatrixMode(GL_PROJECTION);
	// обнуляем матрицу
	glLoadIdentity();
	//glViewport(-(1000 - w) / 2, -(1000 - h) / 2, w / h * 1000, 1000);
	glViewport(0, 0, w, h);
	// установить параметры вьюпорта

	// установить корректную перспективу
	gluPerspective(45, 1, 0.1, 100);
	// вернуться к матрице проекции
	glMatrixMode(GL_MODELVIEW);
}
void ReadData(char* pParam)
{
	FILE* fdata = fopen(pParam, "r");
	if (fdata == 0)
	{
		printf("\nERROR, can't open file");
		return;
	}
	char* buf = new char[100];

	while (!feof(fdata))
	{
		for (int i = 0; i < 100; i++)
		{
			buf[i] = fgetc(fdata);
			if (buf[i] == EOF || buf[i] == '\n')break;
		}
		sscanf(buf, "%f%f%f%d", &track[current_i].x, &track[current_i].y, &track[current_i].z, &Time_MGA[current_i]);
		current_i++;
	}
	delete[] buf;
	global_i = current_i;
}
/*HANDLE initCOM(char c[64])
{

	//sprintf(sPortName, ""
	//std::string s = c;
	//std::wstring example;
	//example.assign(s.begin(), s.end());
	//LPCWSTR sPortName = example.c_str();

	HANDLE hSerial = ::CreateFileA(c, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			printf("serial port does not exist.\n");
		else
			printf("some other error occurred.\n");
		return HANDLE(0);
	}


	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams))
	{
		printf("getting state error\n");
		return HANDLE(0);
	}
	dcbSerialParams.BaudRate = CBR_57600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(hSerial, &dcbSerialParams))
	{
		printf("error setting serial port state\n");
		return HANDLE(0);
	}
	return hSerial;

}
package getpackage_File(FILE* fp)
{
	static package temp;
	static char c;
	c = 0;

	while(c!='M' && c!='G' && c!='A' && c!='H' && !feof(fp))
		fscanf(fp,"%c",&c);
	temp.s = c;
	while(c!=':' && !feof(fp))
		fscanf(fp,"%c",&c);
	if(feof(fp))
		temp.s = 0;
	else
		fscanf(fp,"%f %f %f %d", &temp.data.x, &temp.data.y, &temp.data.z, &temp.t);
	return temp;
}
package getpackage_COM(HANDLE h, FILE* fpc)
{
	DWORD iSize;
	unsigned char sReceivedChar;
	bool b = true;
	static unsigned char lastpacket[30];
	static unsigned char packet[31];
	static int l = 30;
	package temp;
	if (l != 30)
	{
		for (int i = l; i<30; i++)
		{
			lastpacket[i - l] = lastpacket[i];
			packet[i - l + 1] = lastpacket[i];
		}
	}
	if(b)
		while (true)
		{
			ReadFile(h, &sReceivedChar, 1, &iSize, 0);
			fputc(sReceivedChar, fpc);
			if (iSize > 0 && sReceivedChar == 'B')
			{
				packet[0] = sReceivedChar;
				break;
			}
		}

	for (int i = 30 - l; i<30;)
	{
		ReadFile(h, &sReceivedChar, 1, &iSize, 0);
		fputc(sReceivedChar, fpc);
		if (iSize > 0)
		{
			lastpacket[i] = sReceivedChar;
			packet[++i] = sReceivedChar;
		}
	}
	l = 30;
	int k = 0;
	for (int i = 0; i<27; i++)k += packet[i];
	if (k * 44111 == *((int*)(&packet[27])))
	{
		temp.s = packet[10];
		temp.data.x = *((float*)(&packet[11]));
		temp.data.y = *((float*)(&packet[15]));
		temp.data.z = -*((float*)(&packet[19]));
		temp.t = *((int*)(&packet[23]));
		return temp;
	}
	else
	{
		l = 0;
		for (; l<30 ; l++)
			if (lastpacket[l] == 'B')
			{
				packet[0] = lastpacket[l++];
				b = false; break;
			}
	}
	return temp;
}
FILE* tryOpenFile(char* name)
{
	char str[64];

	strcpy(str, name);
	FILE *fp = fopen(str, "r");
	if(fp == 0)
	{
		sprintf(str, "%s%s", name, ".txt");
		fp=fopen(str,"r");
		if(fp == 0)
		{
			sprintf(str, "%s%s", "log/", name);
			fp=fopen(str,"r");
			if(fp == 0)
			{
				sprintf(str, "%s%s%s", "log/", name, ".txt");
				fp=fopen(str,"r");
			}
		}
	}
	if(fp)
		printf("file %s is opened\n", str);
	return fp;
}
int getNumberNewLog()
{
	int number=0;
	FILE *fp=fopen("log/number.txt","r");
	if(fp==0)
	{
		fp=fopen("log/number.txt","w");
		fprintf(fp,"0");
	}
	else
		fscanf(fp,"%d",&number);
	fclose(fp);
	fp=fopen("log/number.txt","w");
	fprintf(fp,"%d",number+1);
	fclose(fp);
	return number+1;
}
void ReadData(void* pParam)
{

	HANDLE handle;
	bool log = 0;
	FILE *fpdump = 0;
	FILE *fp=tryOpenFile((char*)pParam);
	if(fp == 0)
	{
		printf("file does not exist.\n");
		if(int(handle=initCOM((char*)pParam)) == 0)
		{
				Sleep(2000);
				exit(-1);
		}
		else
		{
			printf("COM port %s is opened\n", (char*)pParam);
			int numberNewLog=getNumberNewLog();
			char str[64]={0};
			sprintf(str,"%s%d%s", "log/",numberNewLog, ".txt");
			fp=fopen(str,"w");
			sprintf(str,"%s%d%s", "log/dump",numberNewLog, ".txt");
			fpdump=fopen(str,"w");
		}
	}
	else
		log=true;




	package pm={0,{0,0,0},0}, pg={0,{0,0,0},10}, pa={0,{0,0,0},100}, pack;
	while(true)
	{
		if(log)
			pack=getpackage_File(fp);
		else
			pack=getpackage_COM(handle, fpdump);
		if(pack.s==0)
			break;
		switch(pack.s)
		{
			case 'M': pm=pack;break;
			case 'G': pg=pack;break;
			case 'A': pa=pack;
				//pa.data.x/=9.81;
				//pa.data.y/=9.81;
				//pa.data.z/=9.81;
				break;
			case 'H':
				htv[global_i_htv]=pack.data;
				KalmanX.Correct(htv[global_i_htv].x);
				htv[global_i_htv].x = KalmanX.State;
				Time_HTV[global_i_htv]=pack.t;
				global_i_htv++;
				break;
		}
		if(abs(pm.t-pg.t)<=1 && abs(pg.t-pa.t)<=1)
		{

			//if (!init_vertical && abs(sqrt(pa.data.z*pa.data.z + pa.data.x*pa.data.x) / pa.data.y) <= 0.15)
			//{
			//	init_vertical = true;
			//	if(pm.data.z!=0)
			//	mapAngleNorth = 180 / 3.14159*atan(pm.data.x / pm.data.z);
			//	if (pm.data.z <= 0 && pm.data.x >= 0)
			//		mapAngleNorth += 180;
			//	if (pm.data.z <= 0 && pm.data.x <= 0)
			//		mapAngleNorth -= 180;
			//	mapAngleNorth += horizontalAngleNorth;//calculate for concrete place
			//}
			axcel[global_i]=pa.data;

			if(global_i==0)
				time_start=(pm.t+pg.t+pa.t)/3;
			else
			{
				Time_MGA[global_i]=(pm.t+pg.t+pa.t)/3-time_start;
				track[global_i] = gyr.perform_one_iteration(pg.data);
			}
			global_i++;

			if(!log)//сохранение пакетов в лог
			{
				fprintf(fp,"%c: %f %f %f %d\n", pm.s, pm.data.x, pm.data.y, pm.data.z, pm.t);
				fprintf(fp,"%c: %f %f %f %d\n", pa.s, pa.data.x, pa.data.y, pa.data.z, pa.t);
				fprintf(fp,"%c: %f %f %f %d\n", pg.s, pg.data.x, pg.data.y, pg.data.z, pg.t);
			}
			pm.t=0;
			pg.t=10;
			pa.t=100;
		}


	}
	fclose(fp);
	if(fpdump!=0)
		fclose(fpdump);
}*/

void keyboard_char(unsigned char key, int x, int y) 
{
	keychar[key] = true;
	switch (key) {
	

	
	
	/*case '1':delta_i = -100; real_time = false; break;
	case '2':delta_i = -10; real_time = false; break;
	case '3':delta_i = -1; real_time = false; break;
	case '4':delta_i = 1; real_time = false; break;
	case '5':delta_i = 10; real_time = false; break;
	case '6':delta_i = 100; real_time = false; break;*/
	case '0':current_i = 2; real_time = false; break;
	case 'm':
	case 'M':draw_map_var = !draw_map_var; break;
	case 'r':
	case 'R':color_track.x = 1; color_track.y = 0; color_track.z = 0; break;
	case 'g':
	case 'G':color_track.x = 0; color_track.y = 1; color_track.z = 0; break;
	case 'b':
	case 'B':color_track.x = 0; color_track.y = 0; color_track.z = 1; break;
	case 'o':
	case 'O': show_line = !show_line;
		//case 27:if(polniy_ikran){glutReshapeWindow(1200, 1000);polniy_ikran=false;}else{ glutFullScreen();polniy_ikran=true;} break;
		//case ' ':prig = true; break;
		//case 168:
		//case '~':glutHideWindow();printf("вы собрали %d чайников",q);

	}
}
void keyboard_char_up(unsigned char key, int x, int y) 
{
	keychar[key] = false;
	/*switch (key) {

	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':delta_i = 0; break;
	}*/
}
void keyboard_int(int key, int xx, int yy) 
{
	keyint[key] = true;
}
void keyboard_int_up(int key, int x, int y) 
{
	keyint[key] = false;
}
void keyboardParse() //обработка всех кнопок управления с клавиатуры
{
	//переметка траектории, отслеживание 
	if (keychar['1'] || keychar['2'] || keychar['3'])real_time = false;;
	delta_i = (keychar['1']*(-100)+ keychar['2'] * (-10) + keychar['3'] * (-1) + keychar['4'] * (1) + keychar['5'] * (10) + keychar['6'] * (100));
	//вращение карты вокруг центра
	if (keychar[244] || keychar[212] || keychar['A'] || keychar['a']) mapAngle += deltaMove.x;
	if (keychar[194] || keychar[226] || keychar['D'] || keychar['d']) mapAngle -= deltaMove.x;
	if (mapAngle > 360)mapAngle -= 360;
	if (mapAngle < -360)mapAngle += 360;
	//перемещение камеры от центра и к центру
	if (keychar[251] || keychar[219] || keychar['S'] || keychar['s']) cam.z += deltaMove.z;
	if (keychar[214] || keychar[246] || keychar['W'] || keychar['w']) cam.z -= deltaMove.z;
	if (cam.z > 100) cam.z -= deltaMove.z;
	if (cam.z < 0) cam.z += deltaMove.z;
	//перемещение камеры вверх вниз 
	if (keychar['q'] || keychar['Q']) cam.y += deltaMove.y;
	if (keychar['e'] || keychar['E']) cam.y -= deltaMove.y;
	if (cam.y > 100) cam.y -= deltaMove.y;
	if (cam.y < -20) cam.y += deltaMove.y;
	//вращение камеры вокруг своих локальных осей
	if (keyint[GLUT_KEY_UP])   angleCam.x += deltaCam.x;
	if (keyint[GLUT_KEY_DOWN]) angleCam.x -= deltaCam.x;
	if (keyint[GLUT_KEY_RIGHT])angleCam.y += deltaCam.y;
	if (keyint[GLUT_KEY_LEFT]) angleCam.y -= deltaCam.y;
	if (angleCam.x >  1.57) angleCam.x -= deltaCam.x;
	if (angleCam.x < -1.57) angleCam.x += deltaCam.x;
	direct.z = -cos(angleCam.x) * cos(angleCam.y);
	direct.y = sin(angleCam.x);
	direct.x = cos(angleCam.x) * sin(angleCam.y);

}
void timef(int value) {
	keyboardParse();
	glutPostRedisplay();  // перерисовывает окно

	glutTimerFunc(40, timef, 0); // рекурсия
}
void mause(int button, int state, int x, int y) {
	printf("%d %d %d %d\n", button, state, x, y);
}
int main(int argc, char** argv) {
	//setlocale(LC_ALL,"rus");
	float k = 50;
	FILE* fp;

	fp = fopen("angleNorth.txt", "r");
	if (fp != 0)
	{
		fscanf(fp, "%f%f", &verticalAngleNorth, &horizontalAngleNorth);
		fclose(fp);
	}
	char str[56];
	scanf("%s", str);
	ReadData(str);
	//_beginthread(ReadData, 0, str);
	//ReadLog(str1);
	//initCOM(str2);

	// инициализация GLUT и создание окна
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1000, 1000);
	glutCreateWindow("Trajectory");
	InitGL();
	glutDisplayFunc(Display);//рисовалка
	glutReshapeFunc(Reshape);

	glutTimerFunc(20, timef, 0);
	glutSpecialFunc(keyboard_int);//для обработки нажатий стрелок
	glutSpecialUpFunc(keyboard_int_up);//для обработки отжатий стрелок
	glutKeyboardFunc(keyboard_char);//для обработки нажатий символов (w,a,s,d)
	glutKeyboardUpFunc(keyboard_char_up);//для обработки отжатий символов (w,a,s,d)
										 //glutIgnoreKeyRepeat(0);// что это блин?
	glutMouseFunc(mause);


	glEnable(GL_DEPTH_TEST);// OpenGL - инициализация функции теста
	srand(time(0));


	//	PlaySound(LPCSTR("музон.wav"),NULL,SND_ASYNC);
	glutMainLoop();
	return 0;
}