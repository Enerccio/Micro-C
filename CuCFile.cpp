#include "CuCFile.h"

void writeInt(std::ofstream* out, unsigned int i)
{
	out->write((const char*)&i, 4);
}

void writeString(std::ofstream* out, std::string str)
{
	writeInt(out, str.length());
	out->write((const char*)str.c_str(), str.length());
}

CuCFile::CuCFile()
{

}

CuCFile::~CuCFile()
{
	delete code;
}

unsigned int header = 0x1234434d;

void CuCFile::save(CuCFile* file, std::ofstream* out)
{
	// write header
	writeInt(out, header);
	writeInt(out, file->code_len);
	writeInt(out, file->consts.size());
	writeInt(out, file->literars.size());
	writeInt(out, file->strings.size());

	// write filename
	writeString(out, file->name);

	// write code
	writeInt(out, file->code_len);
	out->write((const char*)file->code, file->code_len);

	// write tables
	{
		std::vector<int>::iterator it;
		for (it = file->consts.begin(); it != file->consts.end(); it++)
			writeInt(out, *it);
	}
	{
		std::vector<std::string>::iterator it;
		for (it = file->literars.begin(); it != file->literars.end(); it++)
			writeString(out, *it);
	}
	{
		std::vector<std::string>::iterator it;
		for (it = file->strings.begin(); it != file->strings.end(); it++)
			writeString(out, *it);
	}
}

CuCFile* CuCFile::parse(std::ifstream* in)
{
	return NULL;
}