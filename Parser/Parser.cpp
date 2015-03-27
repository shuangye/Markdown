// Parser.cpp : Defines the entry point for the console application.
//

#include <tchar.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include "lib_api.h"
using std::ifstream;
using std::string;

int _tmain(int argc, _TCHAR* argv[])
{
	char *result;
	if (argc < 2)
		return 0;
		
	ifstream *ifs = new ifstream(argv[1], std::ifstream::in);	
	ifs->seekg(0, std::ios_base::end);
	long file_length = ifs->tellg();	
	ifs->seekg(0, std::ios_base::beg);

	char *text = (char*)calloc(1, file_length);
	ifs->read(text, file_length);
	ifs->close();

	delete ifs, ifs = nullptr;

	result = markdown_to_string(text, 0, HTML_FORMAT);

	free(result), result = nullptr;
	free(text), text = nullptr;

	return 0;
}

