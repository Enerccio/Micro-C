#pragma once

#include <fstream>

#include "CuCFile.h"
#include "opcode.h"

using namespace std;

class Disassembler
{
public:
	Disassembler();
	virtual ~Disassembler();

	void Dissassemble(ofstream& o, CuCFile* code);
};

