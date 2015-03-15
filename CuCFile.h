#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class VM;
class Compiler;

class CuCFile
{
	friend class VM;
	friend class Compiler;
public:
	CuCFile();
	virtual ~CuCFile();

	static CuCFile* parse(std::ifstream* in);
	static void save(CuCFile* file, std::ofstream* out);

	bool GetConstant(unsigned short pos, int* constant)
	{
		if (consts.size() <= pos)
		{
			*constant = 0;
			return false;
		} else 
		{
			*constant = consts[pos];
			return true;
		}
	}

	bool GetIdentifier(unsigned short pos, std::string* literar)
	{
		if (literars.size() <= pos)
		{
			return false;
		}
		else
		{
			*literar = literars[pos];
			return true;
		}
	}

	bool GetString(unsigned short pos, std::string* string)
	{
		if (strings.size() <= pos)
		{
			return false;
		}
		else
		{
			*string = strings[pos];
			return true;
		}
	}

	void SetName(std::string name)
	{
		this->name = name;
	}

private:
	unsigned char* code;
	unsigned int code_len;

	std::string name;

	unsigned short AddConstant(int constant)
	{
		std::vector<int>::iterator it;
		unsigned int i = 0;
		for (it = consts.begin(); it != consts.end(); it++)
		{
			if (*it == constant)
				return i;
			++i;
		}
		consts.push_back(constant);
		return i;
	}

	unsigned short AddString(std::string constant)
	{
		std::vector<std::string>::iterator it;
		unsigned int i = 0;
		for (it = strings.begin(); it != strings.end(); it++)
		{
			if (*it == constant)
				return i;
			++i;
		}
		strings.push_back(constant);
		return i;
	}

	unsigned short AddLiterar(std::string constant)
	{
		std::vector<std::string>::iterator it;
		unsigned int i = 0;
		for (it = literars.begin(); it != literars.end(); it++)
		{
			if (*it == constant)
				return i;
			++i;
		}
		literars.push_back(constant);
		return i;
	}

	std::vector<std::string> literars;
	std::vector<std::string> strings;
	std::vector<int>         consts;
};

