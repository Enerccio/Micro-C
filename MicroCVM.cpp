#include "MicroCVM.h"

using namespace OP;

short readShort(unsigned char* a, unsigned int i)
{
	unsigned char* array = a + i;
	char c[2];
	c[0] = array[0];
	c[1] = array[1];
	short value = *((short*)array);
	return value;
}

MicroCVM::MicroCVM()
{
	this->frame = NULL;
}


MicroCVM::~MicroCVM()
{
}

void MicroCVM::Load(CuCFile* file)
{
	this->data = file;
	this->ip = 0;
	this->stack.clear();
}

bool MicroCVM::HasNext()
{
	return ip < data->code_len && ip >= 0;
}

void MicroCVM::Iterate()
{

}

OpCode MicroCVM::NextInstruction(short* addedValue)
{
	unsigned short* us = (unsigned short*)addedValue;
	short* ss = addedValue;

	OpCode o = (OpCode)this->data->code[this->ip++];

	switch (o)
	{
	case NOP:
	case PUSH_FRAME:
	case POP_FRAME:
	case POP:
	case DUP_STACK:
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case LSHIFT:
	case RSHIFT:
	case EQ:
	case LESS:
	case MORE:
	case LEQ:
	case MEQ:
	case BOR:
	case BAND:
	case BXOR:
	case MOD:
	case UMIN:
	case UNEG:
	case UBNEG:
	case PRINT:
		break;
	case JMP:
		*ss = readShort(this->data->code, this->ip);
		this->ip += 2;
		break;
	case JZERO:
		*ss = readShort(this->data->code, this->ip);
		this->ip += 2;
		break;
	case JNZERO:
		*ss = readShort(this->data->code, this->ip);
		this->ip += 2;
		break;
	case PUSH_VAR:
		*us = readShort(this->data->code, this->ip);
		this->ip += 2;
		break;
	case PUSH_CONST:
		*us = readShort(this->data->code, this->ip);
		this->ip += 2;
		break;
	case PRINT_LIT:
		*us = readShort(this->data->code, this->ip);
		this->ip += 2;
		break;
	case PRINT_FRM:
		*us = readShort(this->data->code, this->ip);
		this->ip += 2;
		break;
	case STORE_VAR:
		*us = readShort(this->data->code, this->ip);
		this->ip += 2;
		break;
	case SCAN:
		*us = readShort(this->data->code, this->ip);
		this->ip += 2;
		break;
	}

	return o;
}