#pragma once

#include "opcode.h"
#include "CuCFile.h"

#include <unordered_map>
#include <string>
#include <list>

using namespace std;

typedef unordered_map<string, int> VariableMap;
typedef list<int> Stack;

struct VarFrame
{
	VarFrame(){ previous = NULL; }

	VarFrame* previous;
	VariableMap map;
};

class MicroCVM
{
public:
	MicroCVM();
	virtual ~MicroCVM();

	void Load(CuCFile* file);
	bool HasNext();
	void Iterate();
private:
	OP::OpCode NextInstruction(short* addedValue);

	CuCFile* data;
	unsigned int ip;
	Stack stack;
	VarFrame* frame;
};

