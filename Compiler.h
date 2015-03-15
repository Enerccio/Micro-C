#pragma once

#include <cstdio>
#include <iostream>
#include <map>
#include <unordered_map>
#include <string>
#include <cstring>
#include <iomanip>
#include <sstream> 
#include <vector>
#include <unordered_map>

#include "ast.h"
#include "opcode.h"
#include "CuCFile.h"

using namespace OP;
using namespace std;

class Compiler
{
public:
	Compiler();
	virtual ~Compiler();

	bool Compile(FILE* in, CuCFile* out, char* fnamein, bool debug, bool checkTimestamp);

private:
	bool CheckTimestamp();
	void Parse();
	bool CheckErrors();
	bool CheckForCorrectLoops(Statement* block, unsigned int loopCount);
	bool Compile();

	void Optimize();

	void CompileBlock(Block* b, vector<OpData*>* v);
	void CompileStatement(Statement* st, vector<OpData*>* v);

	FILE* input;
	CuCFile* output;
	bool debug;
	char* fnamein;

	Block* ast;

	unordered_map<unsigned long long, unsigned short> forward_map;

	void Add(unsigned short s)
	{
		for (unordered_map<unsigned long long, unsigned short>::iterator iter = forward_map.begin(); iter != forward_map.end(); ++iter)
		{
			iter->second += s;
		}
	}
};

