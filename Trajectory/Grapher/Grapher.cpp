


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
point_structure parse_YAML_point(char * str)
{
	point_structure point;
	point.a = { 0,0,0 };
	point.coord = { 0,0,0 };
	point.vec = { 0,0,0 };
	point.t = 0;
	memset(point.str, 0, sizeof(char)* EXTRA_LINE_LENGTH);
	char buftmp[32];
	char* pointer[5];
	char* sptr = 0;
	pointer[0] = strstr(str, "str:");
	if (pointer[0])//сначала обрабатываем str, ибо в ней может содержаться все чо угодно
	{
		sptr = strchr(pointer[0], '\"');
		if (sptr == 0 || strchr(sptr + 1, '\"') == 0)
		{
			strcpy(point.str, "#!error: there isn't \" symbols after str mark");
			return point;
		}
		strncpy(point.str, sptr + 1, strchr(sptr + 1, '\"')-(sptr + 1));
		for (int i = 1; sptr[i] != '\"'; i++)
			sptr[i] = ' ';
	}
	pointer[1] = strstr(str, "a:");
	pointer[2] = strstr(str, "vec:");
	pointer[3] = strstr(str, "coord:");
	pointer[4] = strstr(str, "t:");
	if (pointer[1]) { pointer[1][-1] = 0; pointer[1] += 2; }
	if (pointer[2]) { pointer[2][-1] = 0; pointer[2] += 4; }
	if (pointer[3]) { pointer[3][-1] = 0; pointer[3] += 6; }
	if (pointer[4]) { pointer[4][-1] = 0; pointer[4] += 2; }
	for(int h=1;h<4;h++)
		if (pointer[h])	//обработка маркеров a: vec: coord:
		{
			int n = strlen(pointer[h]);
			xyz_structure* xyzptr = 0;
			switch (h)
			{
			case 1: xyzptr = &point.a; break;
			case 2: xyzptr = &point.vec; break;
			case 3: xyzptr = &point.coord; break;
			}
			for (int i = 0; i < n; i++)
			{
				while (i < n && ((pointer[h][i] != 'x' && pointer[h][i] != 'y' && pointer[h][i] != 'z') || pointer[h][i + 1] != ':'))
					i++;
				sptr = pointer[h] + i + 2;
				if (i >= n)break;
				float* fptr = 0;
				switch (pointer[h][i])
				{
				case 'x':fptr = &xyzptr->x; break;
				case 'y':fptr = &xyzptr->y; break;
				case 'z':fptr = &xyzptr->z; break;
				}
				while (i < n && pointer[h][i] != ',' && pointer[h][i] != '}' && pointer[h][i] != '\n')
					i++;
				pointer[h][i] = 0;
				sscanf(sptr, "%f", fptr);
			}
		}
	if (pointer[4])	// обработка маркера t:
	{
		int i = 0;
		while (pointer[4][i]!=0 && pointer[4][i] != ',' && pointer[4][i] != '}' && pointer[4][i] != '\n')
			i++;
		pointer[4][i] = 0;
		sscanf(pointer[4], "%f", &point.t);
	}
	return point;
	exit_parse_YAML_point:
	strcpy(point.str, "#!error: bad point");
	return { {0,0,0}, {0,0,0}, {0,0,0}, -1, "error: bad point" };
}

/*int read_and_parse_trajectory(char* b, const int as, int* asc)
{
	
}*/
int ReadData(char* pParam)	// считывание данных из файла, возвращает не ноль если успешно
{							// формат данных - YAML
	FILE* fdata = fopen(pParam, "r");
	int exit_code = 0;
	if (fdata == 0)
	{
		printf("\nERROR, can't open file");
		return 0;
	}


	const int arr_size = 2048;//для выделения памяти
	//int arr_size_count = 1;

	
	char* buf = (char*)malloc((arr_size + 1) * sizeof(char));	//
	memset(buf, 0, (arr_size + 1) * sizeof(char));

	int string_counter = 0;     //чтобы выводить на какой строчке ошибка
	int space_counter = 0;		//для правильной табуляции
	int local_space_counter = 0;
	int tmp = 0;	// просто промежуточные вычисления
	fgets(buf, arr_size, fdata);
	string_counter++;
	for (local_space_counter = 0;
		(buf[local_space_counter] == ' ' || buf[local_space_counter] == '\t') && buf[local_space_counter] != EOF && buf[local_space_counter] != 0;
		local_space_counter++);
	while (!feof(fdata))
	{
		space_counter = local_space_counter;
		while ((strchr(buf, ':') == 0 || buf[space_counter] == 0) && buf[space_counter] != EOF)		// поиск ключа траектории и подсчет пробелов
		{
			fgets(buf, arr_size, fdata);
			string_counter++;
			for (space_counter = 0;
				(buf[space_counter] == ' ' || buf[space_counter] == '\t') && buf[space_counter] != EOF && buf[space_counter] != 0;
				space_counter++);
		} 
		


		if (buf[space_counter] == EOF)break;	// файл заканчивается пустой строкой либо строкой с пробелами
		if (buf[space_counter] == 0)continue;   //строка с пробелами

		
		for (tmp = strchr(buf, ':') - buf - 1; tmp >= 0 && (buf[tmp] == ' ' || buf[tmp] == '\t'); tmp--);
		if (tmp >= 0)
			if ((tmp - space_counter + 1) < 64) 
			{
				strncpy(track[number_of_track].name, buf + space_counter, tmp - space_counter + 1);
				track[number_of_track].name[tmp - space_counter + 1] = 0;
			}
			else 
			{
				strncpy(track[number_of_track].name, buf + space_counter, 63);
				track[number_of_track].name[63] = 0;
			}
		memset(buf, 0, sizeof(char) * strlen(buf));


		char* pointer = buf; //для перемещения по буферу
		do {//считывания всего блока данных, определяя границу по табуляции
			pointer = pointer + strlen(pointer);
			if ((pointer - buf) >= arr_size)
			{
				printf("error, so much character\n");
				return 0;
			}
			fgets(pointer, arr_size - (pointer - buf), fdata);
			string_counter++;
			for (local_space_counter = 0;
				(pointer[local_space_counter] == ' ' || pointer[local_space_counter] == '\t') && pointer[local_space_counter] != EOF && pointer[local_space_counter] != 0;
				local_space_counter++);

			if (pointer[local_space_counter] == EOF)               break;	// файл заканчивается пустой строкой либо строкой с пробелами
			if (pointer[local_space_counter] == 0 && !feof(fdata)) continue;   //строка с пробелами
			tmp = pointer - buf + local_space_counter;
			if (pointer[local_space_counter] == '-' || local_space_counter <= space_counter || feof(fdata))
			{
				if (buf[tmp] == '-' && (strchr(buf, '-') == pointer + local_space_counter))
						continue;						//первое считывание
				else
					if (pointer == buf)
						break;						//первое считывание
					
				while (buf[tmp] != '\n')
					tmp--;
				if (track[number_of_track].number == track[number_of_track].size_of_array)//  растягиваемое выделение памяти
				{
					track[number_of_track].size_of_array += JUMP_FOR_DYNAMIC_ARRAY_OF_POINTS;
					track[number_of_track].points = (point_structure*)realloc(track[number_of_track].points, sizeof(point_structure) * track[number_of_track].size_of_array);
				}

				buf[tmp] = 0;			// подготовка к парсингу точки
				strchr(buf, '-')[0] = ' ';
				track[number_of_track].points[track[number_of_track].number] = parse_YAML_point(buf);

				if (track[number_of_track].points[track[number_of_track].number].str[0] == '#' && track[number_of_track].points[track[number_of_track].number].str[1] == '!')
					printf("%s . Before %d string\n", track[number_of_track].points[track[number_of_track].number].str, string_counter);
				else
					track[number_of_track].number++;
				
				//for (int i = 0; buf[tmp + i] != 0; i++)
				//	buf[i] = buf[tmp + i];
				tmp++;
				strcpy(buf, buf + tmp);
				pointer = buf;
			}
		

			
		} while (!feof(fdata) && local_space_counter > space_counter  && pointer[local_space_counter] != EOF);
		if (track[number_of_track].number > 0)
		{
			track[number_of_track].active = true;
			track[number_of_track].index = track[number_of_track].number;
			number_of_track++;
			
		}
			//pointer[-1] = 0;



		//exit_code = read_and_parse_trajectory(buf, arr_size, &arr_size_count);


		
		



		/*while (!feof(fdata))
		{
			pointer = buf;
			fgets(buf, 255, fdata);

			for (int i = 0; i < 255 && buf[i] != EOF && buf[i] != '\n' && buf[i] != 0; i += 2)//цикл обработки каждой строки
			{
				while (buf[i] != ':' && buf[i] != EOF && buf[i] != '\n' && buf[i] != 0 && i < 255)
					i++;
				buf[i] = 0;
				switch (pointer[0])//парсинг по ключам
				{
				case 'x':sscanf(pointer + 2, "%f", &track[current_i].x); break;
				case 'y':sscanf(pointer + 2, "%f", &track[current_i].y); break;
				case 'z':sscanf(pointer + 2, "%f", &track[current_i].z); break;
				case 't':sscanf(pointer + 2, "%d", &Time_MGA[current_i]); break;
				case 'a':break;
				default:printf("bad file format\n");
				}
				if (buf[i + 1] == 0 || buf[i + 1] == EOF)
					break;
				buf[i] = ':';
				pointer = &buf[i] - 1;
			}
			current_i++;
		}*/
	}
	free(buf);
	//global_i = current_i;
	return exit_code;
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
	
	//case '0':current_i = 2; real_time = false; break;
	case 'm':
	case 'M':draw_map_var = !draw_map_var; break;
	case 'r':
	case 'R':color_track.x = 1; color_track.y = 0; color_track.z = 0; break;
	case 'g':
	case 'G':color_track.x = 0; color_track.y = 1; color_track.z = 0; break;
	case 'b':
	case 'B':color_track.x = 0; color_track.y = 0; color_track.z = 1; break;
	//case 'o':
	//case 'O': show_line = !show_line;
		//case 27:if(polniy_ikran){glutReshapeWindow(1200, 1000);polniy_ikran=false;}else{ glutFullScreen();polniy_ikran=true;} break;
		//case ' ':prig = true; break;
		//case 168:
		//case '~':glutHideWindow();printf("вы собрали %d чайников",q);

	}
}
void keyboard_char_up(unsigned char key, int x, int y) 
{
	keychar[key] = false;
	
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
	//if (keychar['1'] || keychar['2'] || keychar['3'])real_time = false;
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

	glutTimerFunc(40, timef, 0); // рекурсия(нет)
}
void mause(int button, int state, int x, int y) {
	printf("%d %d %d %d\n", button, state, x, y);
}
int main(int argc, char** argv) {
	//setlocal(LC_ALL,"rus");
	float k = 50;
	FILE* fp;
	for (int i = 0; i < TRACK_MAX; i++)
	{
		track[i].number = 0;
		track[i].size_of_array = 0;
		track[i].index = 0;
		track[i].show = 0;
		track[i].active = 0;
		track[i].color = { 0.5, 0.5, 0.5 };
	}
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