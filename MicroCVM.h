#pragma once

#include "opcode.h"
#include "CuCFile.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

typedef unordered_map<unsigned short, int> VariableMap;
typedef vector<int> Stack;

#define ERROR(str) \
	do \
	{\
		cout << str << endl; this->ip = -1; return;\
	} while(0)

#define DEBUG(str) if (this->debug) cout << str << endl

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
	bool FindValue(unsigned short id, VarFrame* frame, int* v);

	void PushFrame();
	void PopFrame();
	void PopStack();
	void DupStack();
	void UnaryOperation(OP::OpCode operation);
	void BinaryOperation(OP::OpCode operation);
	void PrintStack();
	void Jmp(short pcMod);
	void JumpIfZero(short pcMod);
	void JumpIfNotZero(short pcMod);
	void PushVar(unsigned short id);
	void PushConst(unsigned short id);
	void PrintLiterar(unsigned short id);
	void PrintFormatted(unsigned short id);
	void StoreVar(unsigned short id);
	void Scan(unsigned short id);

	CuCFile* data;
	int ip;
	Stack stack;
	VarFrame* frame;
	bool debug;
};

