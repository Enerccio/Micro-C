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
#include <list>

#include "ast.h"
#include "opcode.h"
#include "CuCFile.h"

using namespace OP;
using namespace std;

typedef unordered_map<unsigned long long, unsigned short> Backmap;
typedef list<OpData*> DList;

struct Frame
{
	Frame(){ startPos = 0; parent = NULL; frameCount = 0; }

	Frame*  parent;
	DList endHooks;
	int frameCount;
	unsigned int startPos;
};

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
	void PushFrame();
	void PopFrame();

	FILE* input;
	CuCFile* output;
	bool debug;
	char* fnamein;

	Block* ast;

	Frame* frame;

	Backmap forward_map;

	void Add(unsigned short s)
	{
		for (Backmap::iterator iter = forward_map.begin(); iter != forward_map.end(); ++iter)
		{
			iter->second += s;
		}
		Frame* frm = frame;
		while (frm != NULL){
			frm->startPos += s;
			frm = frm->parent;
		}
	}
};

