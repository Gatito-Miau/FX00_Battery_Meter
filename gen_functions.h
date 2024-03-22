// gen_functions.h © Dario Napole 2010

#ifndef GEN_FUNCTIONS_H
#define GEN_FUNCTIONS_H
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <limits.h>

typedef unsigned long long ullong;
typedef unsigned int uint;
typedef unsigned char uchar;

using namespace std;

// Prototypes

// Manipulacion de char arrays
char* new_char (unsigned long long);				// Crea un array de chars, inicializados a \0 y con un bytes \0 extra para evitar confuciones o "+1" en los tamaños
char fchar (char*);									// Devuelve el primer caracter de un array de caracteres
void char_zero(char*, uint);						// Inicializa un array de caracteres a \0

// Depuración
namespace debug{
	void DumpMem (char*,int,int,int,int);
	void DumpMemASCII (char*,int,int,int,int);
}

//General Functions
#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
  #include <windows.h>
  inline void delay( unsigned long ms )
    {Sleep( ms );}

#else  /* presume POSIX */
  #include <unistd.h>
  inline void delay( unsigned long ms )
    {usleep( ms * 1000 );}
#endif

bool IsDivisible (int a, int b){
    if (!a==0 && a % b == 0){return true;}else{return false;}
}
int dividir_int (int x, int y){
	int res = 0;
	while (x > y){res++; x-=y;}
return res;
}
void gotoxy(int x, int y) {
	/*
	** gotoxy() Implementation in the *nix environment
	**
	** Note: I've heard the curses library can be useful
	** when trying to implement the handy DOS-only tools of
	** gotoxy() and clrscr() using move() and initscr().
	** Though, there is a way to write your own gotoxy()
	** in the Linux environment. This topic isn't discussed
	** often, so I'd like to bring a few new ideas to the table.
	**
	** Concept: We will use two ANSI C standard functions
	** to accomplish our task. We will use specific
	** string manuvers, according to the man pages, that
	** will allow us to execute each part of the program.
	*/
        char essq[100];		/* String variable to hold the escape sequence */
        char xstr[100];		/* Strings to hold the x and y coordinates */
        char ystr[100];		/* Escape sequences must be built with characters */

        /*
        ** Convert the screen coordinates to strings
        */
        sprintf(xstr, "%d", x);
        sprintf(ystr, "%d", y);

        /*
        ** Build the escape sequence (vertical move)
        */
        essq[0] = '\0';
        strcat(essq, "\033[");
        strcat(essq, ystr);

        /*
        ** Described in man terminfo as vpa=\E[%p1%dd
        ** Vertical position absolute
        */
        strcat(essq, "d");

        /*
        ** Horizontal move
        ** Horizontal position absolute
        */
        strcat(essq, "\033[");
        strcat(essq, xstr);
        /* Described in man terminfo as hpa=\E[%p1%dG */
        strcat(essq, "G");

        /*
        ** Execute the escape sequence
        ** This will move the cursor to x, y
        */
        printf("%s", essq);
}
void gotox(int x) {
        char essq[100];
        char xstr[100];

        sprintf(xstr, "%d", x);
        essq[0] = '\0';

		strcat(essq, "\033[");
        strcat(essq, xstr);
        strcat(essq, "G");

        printf("%s", essq);
}
char fchar (char* s){  //Devuelve solo el primer caracter de un array de chars
	char a = 0;
	memcpy ((void*)&a, (void*)&s[0],1);
	return a;
}
void char_zero(char* array, uint len){	//borra toda una array de chars a \0
	for(uint i = 0; i < len; i++){
		array[i] = 0;
	}
}
void char_X(char* array, uint len, char character){	//borra toda una array de chars a X
	for(uint i = 0; i < len; i++){
		array[i] = character;
	}
}
char* Address_GetName (char * Input){	//obtiene el nombre de una direccion de archivo o direcctorio, depende de fchar(char*)
ullong pos;
ullong len = strlen(Input);
char off = 1;
uint lmts = 0;
char* out = NULL;
char lmt [] = "\\/";	//limitantes \ y /

if (strlen (Input) <= 0){return NULL;}

for (int i = len -1; i > -1; i--){
	if (!((fchar((char*)&Input[i]) == lmt[0] || fchar((char*)&Input[i]) == lmt[1]) && i + lmts == len-1)){
		if ( fchar((char*)&Input[i]) == lmt[0] || fchar((char*)&Input[i]) == lmt[1] || i == 0){
			pos = i;
			if ( !(fchar((char*)&Input[i]) == lmt[0] || fchar((char*)&Input[i]) == lmt[1])){off = 0;} // si el primer caracter no es \ o /, reduce el offset (off) a 0 y devuelve todo el nombre
			out = new_char ((strlen((char*)&Input[pos+off])-lmts));
			memcpy(out, (char*)&Input[pos+off], len-(pos+off+lmts));	//copia desde donde empiesa, (longitud - posision + off + lmts) bytes (chars)
			i = -1;
		}
	}else{lmts++;}
}
return out;
}
char* Address_GetExt (char * Input){	//obtiene la extension de una direccion de archivo, depende de fchar(char*)
ullong pos;
ullong len = strlen(Input);
char off = 1;
uint lmts = 0;
char* out = NULL;
char lmt [] = "\\/.";	//limitantes .

if (strlen (Input) <= 0){return NULL;}

for (int i = len -1; i > -1; i--){
	if (!((fchar((char*)&Input[i]) == lmt[0] || fchar((char*)&Input[i]) == lmt[1] || fchar((char*)&Input[i]) == lmt[2]) && i + lmts == len-1)){
		if ( fchar((char*)&Input[i]) == lmt[2] || i == 0){
			pos = i;
			if ( !( fchar((char*)&Input[i]) == lmt[0] || fchar((char*)&Input[i]) == lmt[1] || fchar((char*)&Input[i]) == lmt[2]  ) ){off = 0;} // si el primer caracter no es \, / o "." , reduce el offset (off) a 0 y devuelve todo el nombre

			out = new_char ((strlen((char*)&Input[pos+off])-lmts));
			memcpy(out, (char*)&Input[pos+off], len-(pos+off+lmts));	//copia desde donde empiesa, (longitud - posision + off + lmts) bytes (chars)
			i = -1;
		}
	}else{lmts++;}
}
return out;
}
bool isLegible (char * Input){	//comprueba es legible segun alphanum[]
    const char alphanum [] =
    "ABCDEFGHIJKLMNOPKRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "1234567890 -+=_.()[]{}¡!¿?\"#$%&/";
	int hit = 0;
	int hit_count = 0;
    for (int i = 0; i < (signed)strlen(Input); i++){ //por cada letra de la enrtada

		for (int j = 0; j < (signed)strlen(alphanum); j++){ 	//compara input[i] con alphanum[j]
			if (Input[i] == alphanum[j]){hit++;}
			if (hit > 0){
				j = (signed)strlen(alphanum);  //comprueba si coincidio alguna letra hasta ahora y si "si" termina el for actual
				hit = 0; hit_count++;}
		}

    }
	if (hit_count == (signed)strlen(Input) && (signed)strlen(Input) != 0 ){return true;}
return false;
}
bool isLegible (char * Input, int len){	//comprueba es legible segun alphanum[], overload con longitud del string
    const char alphanum [] =
    "ABCDEFGHIJKLMNOPKRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "1234567890 -+=_.()[]{}¡!¿?\"#$%&/";
	int hit = 0;
	int hit_count = 0;
    for (int i = 0; i < len; i++){ //por cada letra de la enrtada

		for (int j = 0; j < (signed)strlen(alphanum); j++){ 	//compara input[i] con alphanum[j]
			if (Input[i] == alphanum[j]){hit++;}
			if (hit > 0){
				j = (signed)strlen(alphanum);  //comprueba si coincidio alguna letra hasta ahora y si "si" termina el for actual
				hit = 0; hit_count++;}
		}

    }
	if (hit_count == len){return true;}
return false;
}
streamsize GetFileSize (ifstream* file ){ //devuelve el tamaño de un ifstream*
	streamsize i = file->tellg();
	file->seekg(0, ios::end);
	streamsize o = file->tellg();
	file->seekg(i);
return o;
}
char* Address_TrimSlashes (char* in){	//si termina con lmt[] los remueve y devuelve un char* corregido
ullong len = strlen(in);
uint lmts = 0;
char* out = (char*)malloc(len+1); char_zero(out,len+1);
char* temp;
strcpy (out, in);
char lmt [] = "\\/";	//limitantes \ y /

for (int i = len -1; i > -1; i--){
	if (!((fchar((char*)&out[i]) == lmt[0] || fchar((char*)&out[i]) == lmt[1]) && i + lmts == len-1)){
		len = strlen(out);
		temp = (char*)malloc(len+1); char_zero(temp,len+1);
		strcpy (temp, out);
		free(out);

		out = (char*)malloc(len+1); char_zero(out,len+1);
		strcpy (out, temp);
		free(temp);

		return out;
	}else{lmts++;out[i]=0;}
}
return out;
/*Address_TrimSlashes*/}
char* Fix_C_Name(char* in){

	if (strlen(in)==0){
	char* error = (char*)malloc(0);
	error[0] = 0;
	return error;
	}
    const char alphanum [] =
    "_ABCDEFGHIJKLMNOPKRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "1234567890";

    const char numbers [] =
    "0123456789";

	char reem = 95;	// 95 = "_"

    //char* out = (char*)malloc(strlen(in)+1); char_zero(out, strlen(in)+1);
    char* out = in;

	int out_len = (signed)strlen(out);

	int hit_num_cnt = 0;
	int miss_aphnum = 0;

    for (int i = 0; i < out_len; i++){ //por cada letra de la entrada, i = indice para out
		miss_aphnum = 0;

		if (out_len + hit_num_cnt - i == out_len){ 			//Comprueba q los primeros caracteres no sean numeros, si son los reemplaza por "_"
			for (int k = 0; k < (signed)strlen(numbers);k++){
				if (out[i] == numbers[k]){out[i] = reem;hit_num_cnt++;k = (signed)strlen(numbers);}
			}
		}

		for (int j = 0; j < (signed)strlen(alphanum); j++){ 	//compara out[i] con alphanum[j]
			if (out[i] != alphanum[j]){miss_aphnum++;}	// Si coincide, aumenta miss_aphnum para demostrar q es algun caracter de alphanum
		}

		if ( miss_aphnum == (signed)strlen(alphanum)) { miss_aphnum = 0; out[i] = reem;}  // Si out[i] no es ningun caracter de alphanum entonces lo reemplaza por "_"
    }

    return out;

}

int dtoi(double d){

	double tmp = d;
		int r = 0;

	if ( d > INT_MAX){r = INT_MAX; return r;}
	if ( d < INT_MIN){r = INT_MIN; return r;}

	if (tmp > 0){
		while (tmp >= 1){
				tmp = tmp - 1;
				r++;
		}
		if (tmp >= 0.500000){ r++;}
		return r;
	}else if (tmp < 0){
		while (tmp <= -1){
			tmp = tmp + 1;
			r--;
		}
		if (tmp <= -0.500000 ){ r--;}
		return r;
	}
	return r;
}
int ctoi (char s){
int num;
switch (s){
	case '0':
		num = 0;
		return num;
		break;
	case '1':
		num = 1;
		return num;
		break;
	case '2':
		num = 2;
		return num;
		break;
	case '3':
		num = 3;
		return num;
		break;
	case '4':
		num = 4;
		return num;
		break;
	case '5':
		num = 5;
		return num;
		break;
	case '6':
		num = 6;
		return num;
		break;
	case '7':
		num = 7;
		return num;
		break;
	case '8':
		num = 8;
		return num;
		break;
	case '9':
		num = 9;
		return num;
		break;
	default:
		return -1;
	}
}
char* new_char (unsigned long long size){
char* temp = new char [size +1];
char_zero (temp, size+1);
return temp;
}
class average{
	private:
	double *numbers;
	Uint32 count;
	Uint32 max;
	Uint32 current;
	bool fill;
	bool random;
	public:
	Uint32 avg;
	double davg;

	average (Uint32 cnt, bool feel = false, double with = 0, bool randomize = false){
		numbers = new double [cnt];
		current = 0;
		count = 0;
		max = cnt;
		avg = 0;
		davg = 0;
		fill = feel;
		if (fill){for(Uint32 i = 0; i < max;i++){add(with);}}
		random = randomize;
	}
	~average(){
		delete numbers;
	}

	void add(double num){
		if (count>=max && random){
			numbers[rand()%max] = num;
		}else{
		numbers[current] = num;
		}
		if(current+1 > max-1){current=0;}else{current++;}
		if( !(count >= max) ){count++;}
		avg=0;
		davg=0;
		for (Uint32 i = 0; i < count; i++){
			//avg += numbers[i];
			davg += numbers[i];
		}

		davg = davg / count;
		avg = dtoi(davg);

	}
};

class DirPath{
  private:
	char lmt [3]; //Limitantes \ y /
	//bool UseSlashAtEnd;	//defaults to false '\000'
	//bool UseSlashAtBegining;
	char* CurPath;
	char* temp;		//siempre hay q dejarla liberada
  public:
	int Address_len;
	int Name_len;
	char* q_barra;


  public:
	DirPath(char* Address){				//Constructor
		strcpy((char*)&lmt, "\\/");						// Copya \ y / a lmt[]

		/*
		if (Address[0]==lmt[0] || Address[0]==lmt[1]){	//Comprueba si Address empiesa con alguna barra
		UseSlashAtBegining = true;
		}else{UseSlashAtBegining = false;}

		if(Address[strlen(Address)-1]==lmt[0] || Address[strlen(Address)-1]==lmt[1]){	//comprueba si Address termina en alguna barra
		UseSlashAtEnd = true;
		}else{UseSlashAtEnd = false;}
		UseSlashAtEnd = false;
		*/

		CurPath = Address_TrimSlashes(Address);			//CurPath es Address filtrado de las barras terminantes
		Address_len = strlen(CurPath);					//Longitud de CurPath

		Name_len = 0;									//Longitud de Name para ChangeDir()
		#ifdef __linux__						//variable segun el OS en el q se compile
		q_barra = (char*)&"/\0";
		#else
		q_barra = (char*)&"\\\0";
		#endif

		}
	~DirPath(){							//Destructor
	free(CurPath);
		}

	void ChangeDir (char* Name){	//Cambia de directorio basado en una direccion en char[], Depende de isLegible
		Name_len = strlen(Name);

		/*
		**	En caso de q sea ".." vuelve atras un direcctorio
		*/
		if (strcmp(Name, "..")==0){
			if (Address_len==0){return;}
			int a = 0;	//coreccion por si empiesa con alguna barrra y se hace .. q quede la barra en ves de vacio
			for (int i=Address_len -1; i > -1; i--){
				//cout << CurPath[i] << "\t" << (CurPath[i] == lmt[0]) << "\t" << (CurPath[i] == lmt[1]) << endl;

				if(((CurPath[i] == lmt[0] || CurPath[i] == lmt[1]) || i == 0) && i!=Address_len -1 ){
					if (((CurPath[i] == lmt[0] || CurPath[i] == lmt[1]) && i == 0) /*|| UseSlashAtEnd*/){		//si la barra esta en el principio Y i==0 ..
					a=1;
					temp = (char*)malloc(i+a+1); char_zero(temp,i+a+1);
					memcpy (temp, CurPath, i+a);
					free (CurPath);
					}else{																//sino...
					temp = (char*)malloc(i+a+1); char_zero(temp,i+a+1);
					memcpy (temp, CurPath, i);
					free (CurPath);
					}

					CurPath = (char*)malloc(i+a+1); char_zero(CurPath,i+a+1);
					strcpy (CurPath,temp);
					free (temp);

					i=0;
					Address_len = strlen(CurPath);
				}
			}
		}

		/*
		**	Si "." no hace nada
		*/
		else if(strcmp(Name, ".")==0){return;}

		/*
		**	Para nombre comun siempre y cuando sea legible
		*/
		else if(isLegible(Name)){
			char* Name2 = Address_TrimSlashes(Name);
			Name_len = strlen(Name2);
				bool barra = true;
				int a = 2;

			if (Address_len==0 || (CurPath[Address_len-1] == lmt[0] || CurPath[Address_len-1] == lmt[1]) || (Name2[0] == lmt[0] || Name2[0] == lmt[1])){
				barra = false;a=1;}

			temp = (char*)malloc(Address_len+Name_len+a); char_zero(temp, Address_len+Name_len+a);
			strcpy (temp, CurPath);
			if(barra){strcat (temp, q_barra);}
			strcat (temp, Name2);
			free (CurPath);

			CurPath = (char*)malloc(Address_len+Name_len+a); char_zero(CurPath,Address_len+Name_len+a);
			strcpy (CurPath,temp);
			free (temp);

			Address_len = strlen(CurPath);
		}
	}

	char* GetPath(){
		return CurPath;
	}

};//Fin de DirPath


namespace debug{
void DumpMem (char* Addr, int pre, int post, int mark, int jump){	//Addr= (char*) a la direccion, len= mostrar post-Addr, pre= mostrar pre-Addr,
																	//mark= longitud de la variable para resaltar esa seccion, jump= cada cuantos bytes el salto de linea
	cout << "\nDumping from " << (hex) << (uint)Addr << " to " << (uint)Addr + mark <<  " - " << (dec) << mark << " Bytes" <<  " - " << mark*8 << " Bits" << endl << (hex);
	bool a = 0;
	bool b = 0;
	int offset;	//if(pre<8){offset=pre+8;}else{offset=pre;}
		offset=pre;

	for (int i = 0; i < post + mark + offset; i++){
		if(!a){if(i >= offset){cout << "\x1b[1m";a++;}}
		if(!b){if(i >= mark +offset){cout<<"\x1b[m";b++;}}
		if (i != 0 && i%jump==0){cout<<endl;}
		gotox((i%jump)*4);
		uchar uc;
		memcpy((uchar*)&uc, (char*)&Addr[i-offset],1);
		if ((uint)uc < 16){cout << "0";}
		cout << (uint)uc ;
			}
	cout << (dec) << endl;
}
void DumpMemASCII (char* Addr, int pre, int post, int mark, int jump){	//Addr= (char*) a la direccion, len= mostrar post-Addr, pre= mostrar pre-Addr,
																	//mark= longitud de la variable para resaltar esa seccion, jump= cada cuantos bytes el salto de linea
	cout << "\nDumping from " << (hex) << (uint)Addr << " to " << (uint)Addr + mark <<  " - " << (dec) << mark << " Bytes" <<  " - " << mark*8 << " Bits" << endl << (hex);
	bool a = 0;
	bool b = 0;
	int offset;	if(pre<8){offset=pre+8;}else{offset=pre;}

	for (int i = 0; i < post + mark + offset; i++){
		if(!a){if(i >= offset){cout << "\x1b[1m";a++;}}
		if(!b){if(i >= mark +offset){cout<<"\x1b[m";b++;}}
		if (i != 0 && i%jump==0){cout<<endl;}
		gotox((i%jump)*4);
		uchar uc;
		memcpy((uchar*)&uc, (char*)&Addr[i-offset],1);

		cout << (char)uc ;

			}
	cout << (dec) << endl;
}

/*fin debug*/}
#endif
