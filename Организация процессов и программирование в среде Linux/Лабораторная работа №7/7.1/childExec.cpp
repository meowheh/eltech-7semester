#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fstream>

using namespace std;

// argv[1] - дескриптор для записи в канал, argv[2] - имя файла 
int main(int argc, char* argv[])
{
    ifstream file(argv[2]);

		if (file.is_open()) {
			string s;
			while(getline(file, s)){
				s+="\n";
				write(*argv[1], &s[0], strlen(s.c_str()));
			}
			file.close();
			close(*argv[1]);
		}

    exit(EXIT_SUCCESS);
}