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

unsigned int readInt(std::ifstream* in)
{
	char input[4];
	in->read(input, 4);
	unsigned int value = *((unsigned int*)input);
	return value;
}

std::string readString(std::ifstream* in)
{
	unsigned int len = readInt(in);
	char* str = new char[len];
	in->read(str, len);
	std::string s(str, len);
	delete str;
	return s;
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
	unsigned int hr = readInt(in);
	if (hr != header)
	{
		printf("Incorrect header! \n");
		return NULL;
	}

	CuCFile* file = new CuCFile();
	file->code_len = readInt(in);
	file->code = new unsigned char[file->code_len];
	unsigned int constNum = readInt(in);
	unsigned int litNum = readInt(in);
	unsigned int stringNum = readInt(in);
	file->name = readString(in);
	in->read((char*)file->code, file->code_len);

	// read tables
	{
		for (unsigned int i = 0; i < constNum; i++)
		{
			file->consts.push_back(readInt(in));
		}
	}
	{
		for (unsigned int i = 0; i < litNum; i++)
		{
			file->literars.push_back(readString(in));
		}
	}
	{
		for (unsigned int i = 0; i < stringNum; i++)
		{
			file->strings.push_back(readString(in));
		}
	}

	return file;
}