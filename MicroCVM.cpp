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
	this->debug = false;
}

bool MicroCVM::HasNext()
{
	return ip < (int)data->code_len && ip >= 0;
}

void MicroCVM::Iterate()
{
	short addedValue;
	OpCode o = NextInstruction(&addedValue);

	switch (o)
	{
	case NOP:
		break;
	case PUSH_FRAME:
		PushFrame();
		break;
	case POP_FRAME:
		PopFrame();
		break;
	case POP:
		PopStack();
		break;
	case DUP_STACK:
		DupStack();
		break;
	case ADD:
		BinaryOperation(ADD);
		break;
	case SUB:
		BinaryOperation(SUB);
		break;
	case MUL:
		BinaryOperation(MUL);
		break;
	case DIV:
		BinaryOperation(DIV);
		break;
	case LSHIFT:
		BinaryOperation(LSHIFT);
		break;
	case RSHIFT:
		BinaryOperation(RSHIFT);
		break;
	case EQ:
		BinaryOperation(EQ);
		break;
	case LESS:
		BinaryOperation(LESS);
		break;
	case MORE:
		BinaryOperation(MORE);
		break;
	case LEQ:
		BinaryOperation(LEQ);
		break;
	case MEQ:
		BinaryOperation(MEQ);
		break;
	case BOR:
		BinaryOperation(BOR);
		break;
	case BAND:
		BinaryOperation(BAND);
		break;
	case BXOR:
		BinaryOperation(BXOR);
		break;
	case MOD:
		BinaryOperation(MOD);
		break;
	case UMIN:
		UnaryOperation(UMIN);
		break;
	case UNEG:
		UnaryOperation(UNEG);
		break;
	case UBNEG:
		UnaryOperation(UBNEG);
		break;
	case PRINT:
		PrintStack();
		break;
	case JMP:
		Jmp(addedValue);
		break;
	case JZERO:
		JumpIfZero(addedValue);
		break;
	case JNZERO:
		JumpIfNotZero(addedValue);
		break;
	case PUSH_VAR:
		PushVar((unsigned short)addedValue);
		break;
	case PUSH_CONST:
		PushConst((unsigned short)addedValue);
		break;
	case PRINT_LIT:
		PrintLiterar((unsigned short)addedValue);
		break;
	case PRINT_FRM:
		PrintFormatted((unsigned short)addedValue);
		break;
	case STORE_VAR:
		StoreVar((unsigned short)addedValue);
		break;
	case SCAN:
		Scan((unsigned short)addedValue);
		break;
	}
}

void MicroCVM::PushFrame()
{
	VarFrame* newFrame = new VarFrame();
	newFrame->previous = this->frame;
	this->frame = newFrame;
}

void MicroCVM::PopFrame()
{
	if (this->frame == NULL)
		ERROR("Variable Frame Stack is empty but POP_STACK called");
	VarFrame* oldFrame = frame;
	frame = oldFrame->previous;
	delete oldFrame;
}

void MicroCVM::PopStack()
{
	if (stack.size() == 0)
		ERROR("Stack is empty but POP called");
}

void MicroCVM::DupStack()
{
	if (stack.size() == 0)
		ERROR("Stack empty but DUP_STACK called");
	
	int var = *stack.rend();
	stack.push_back(var);
}

void MicroCVM::UnaryOperation(OpCode o)
{
	if (stack.size() < 1)
		ERROR("Stack is empty but unary operation was called");

	int op = *stack.rbegin();
	stack.pop_back();
	int result;

	switch (o)
	{
	case UMIN:
		result = -op;
		break;
	case UNEG:
		result = !op;
		break;
	case UBNEG:
		result = ~op;
		break;
	}

	stack.push_back(result);
}

void MicroCVM::BinaryOperation(OpCode o)
{
	if (stack.size() < 2)
		ERROR("Stack contains less than 2 items but binary operation was called");

	int op2 = *stack.rbegin();
	stack.pop_back();
	int op1 = *stack.rbegin();
	stack.pop_back();
	int result;

	switch (o)
	{
	case ADD:
		result = op1 + op2;
		break;
	case SUB:
		result = op1 - op2;
		break;
	case MUL:
		result = op1 * op2;
		break;
	case DIV:
		result = op1 / op2;
		break;
	case LSHIFT:
		result = op1 << op2;
		break;
	case RSHIFT:
		result = op1 >> op2;
		break;
	case EQ:
		result = op1 == op2;
		break;
	case LESS:
		result = op1 < op2;
		break;
	case MORE:
		result = op1 > op2;
		break;
	case LEQ:
		result = op1 <= op2;
		break;
	case MEQ:
		result = op1 >= op2;
		break;
	case BOR:
		result = op1 | op2;
		break;
	case BAND:
		result = op1 & op2;
		break;
	case BXOR:
		result = op1 ^ op2;
		break;
	case MOD:
		result = op1 % op2;
		break;
	}

	stack.push_back(result);
}

void MicroCVM::PrintStack()
{
	if (stack.size() < 1)
		ERROR("Stack is empty but unary operation was called");

	int value = *stack.rbegin();
	stack.pop_back();

	DEBUG(value);

	cout << value;
}

void MicroCVM::Jmp(short pcMod)
{
	if (pcMod < 0)
		pcMod -= 3;
	this->ip += pcMod;
	if (!HasNext())
		ERROR("Illegal jump");
}

void MicroCVM::JumpIfZero(short pcMod)
{
	if (stack.size() < 1)
		ERROR("Stack is empty but JZERO was called");

	int val = *stack.rbegin();
	stack.pop_back();
	if (val == 0)
	{
		Jmp(pcMod);
	}
}

void MicroCVM::JumpIfNotZero(short pcMod)
{
	if (stack.size() < 1)
		ERROR("Stack is empty but JNZERO was called");
	
	int val = *stack.rbegin();
	stack.pop_back();
	if (val != 0)
	{
		Jmp(pcMod);
	}
}

void MicroCVM::PushVar(unsigned short id)
{
	string var;
	if (!this->data->GetIdentifier(id, &var))
		ERROR("Corrupter identifier data");

	int value = 0;
	if (!FindValue(var, this->frame, &value))
		ERROR("Unknown variable");
	stack.push_back(value);
}

void MicroCVM::PushConst(unsigned short id)
{
	int val;
	if (!this->data->GetConstant(id, &val))
		ERROR("Corrupter constant data");
	stack.push_back(val);
}

void MicroCVM::PrintLiterar(unsigned short id)
{
	string lit;
	if (!this->data->GetString(id, &lit))
		ERROR("Corrupter literar data");

	cout << lit.c_str();
}

void MicroCVM::PrintFormatted(unsigned short id)
{
	string lit;
	if (!this->data->GetString(id, &lit))
		ERROR("Corrupter literar data");

	if (stack.size() < 1)
		ERROR("Stack is empty but PRINT_FRM was called");

	int value = *stack.rbegin();
	stack.pop_back();

	printf(lit.c_str(), value);
}

void MicroCVM::StoreVar(unsigned short id)
{

	if (this->frame == NULL)
		ERROR("VarFrame is empty, cannot store variables");

	if (stack.size() < 1)
		ERROR("Stack is empty but STORE_VAR was called");

	string var;
	if (!this->data->GetIdentifier(id, &var))
		ERROR("Corrupter identifier data");

	int value = *stack.rbegin();
	stack.pop_back();

	this->frame->map[var] = value;
}

void MicroCVM::Scan(unsigned short id)
{
	if (this->frame == NULL)
		ERROR("VarFrame is empty, cannot store variables");

	string var;
	if (!this->data->GetIdentifier(id, &var))
		ERROR("Corrupter identifier data");

	int value;
	cin >> value;

	this->frame->map[var] = value;
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

bool MicroCVM::FindValue(string s, VarFrame* stack, int* v)
{
	if (stack == NULL)
		return false;
	VariableMap::iterator it = stack->map.find(s);
	if (it != stack->map.end())
	{
		*v = it->second;
		return true;
	}
	return FindValue(s, stack->previous, v);
}