#include "Disassembler.h"

using namespace OP;

short readShort(unsigned char* array)
{
	char c[2];
	c[0] = array[0];
	c[1] = array[1];
	short value = *((short*)array);
	return value;
}


Disassembler::Disassembler()
{
}


Disassembler::~Disassembler()
{
}

const char* format_single = "%i\t%s\t%02x \t\t%s";
const char* format_tripple = "%i\t%s\t%02x %02x %02x\t%s";

void Disassembler::Dissassemble(ofstream& out, CuCFile* code)
{
	char buffer[2048];
	char buf2[2048];
	short addr;
	unsigned short param;
	int iparam;
	std::string ident;

	out << "C\tOP\t\tHEX\t\tDESCRIPTION" << endl;

	unsigned int codePtr = 0;
	while (codePtr < code->code_len)
	{
		OpCode o = (OpCode)code->code[codePtr++];

		switch (o)
		{
		case NOP:
			sprintf(buffer, format_single, codePtr - 1, "NOP\t", (int)o, "NO OPERATION");
			break;
		case PUSH_FRAME:
			sprintf(buffer, format_single, codePtr - 1, "PUSH_FRAME", (int)o, "PUSH FRAME onto stack");
			break;
		case POP_FRAME:
			sprintf(buffer, format_single, codePtr - 1, "POP_FRAME", (int)o, "POP FRAME from stack");
			break;
		case POP:
			sprintf(buffer, format_single, codePtr - 1, "POP\t", (int)o, "POP value off stack");
			break;
		case DUP_STACK:
			sprintf(buffer, format_single, codePtr - 1, "DUP_STACK", (int)o, "DUPLICATE top of the stack");
			break;
		case ADD:
			sprintf(buffer, format_single, codePtr - 1, "ADD\t", (int)o, "ADD operation");
			break;
		case SUB:
			sprintf(buffer, format_single, codePtr - 1, "SUB\t", (int)o, "SUB operation");
			break;
		case MUL:
			sprintf(buffer, format_single, codePtr - 1, "MUL\t", (int)o, "MUL operation");
			break;
		case DIV:
			sprintf(buffer, format_single, codePtr - 1, "DIV\t", (int)o, "DIV operation");
			break;
		case LSHIFT:
			sprintf(buffer, format_single, codePtr - 1, "LSHIFT", (int)o, "LEFT SHIFT operation");
			break;
		case RSHIFT:
			sprintf(buffer, format_single, codePtr - 1, "RSHIFT", (int)o, "RIGHT SHIFT operation");
			break;
		case EQ:
			sprintf(buffer, format_single, codePtr - 1, "EQ\t", (int)o, "EQ operation");
			break;
		case LESS:
			sprintf(buffer, format_single, codePtr - 1, "LESS\t", (int)o, "LESS operation");
			break;
		case MORE:
			sprintf(buffer, format_single, codePtr - 1, "MORE\t", (int)o, "MORE operation");
			break;
		case LEQ:
			sprintf(buffer, format_single, codePtr - 1, "LEQ\t", (int)o, "LESS OR EQUAL operation");
			break;
		case MEQ:
			sprintf(buffer, format_single, codePtr - 1, "MEQ\t", (int)o, "MORE OR EQUAL operation");
			break;
		case BOR:
			sprintf(buffer, format_single, codePtr - 1, "BOR\t", (int)o, "BYTE OR operation");
			break;
		case BAND:
			sprintf(buffer, format_single, codePtr - 1, "BAND\t", (int)o, "BYTE AND operation");
			break;
		case BXOR:
			sprintf(buffer, format_single, codePtr - 1, "BXOR\t", (int)o, "BYTE XOR operation");
			break;
		case MOD:
			sprintf(buffer, format_single, codePtr - 1, "MOD\t", (int)o, "MODULO operation");
			break;
		case UMIN:
			sprintf(buffer, format_single, codePtr - 1, "UMIN\t", (int)o, "Unary MINUS");
			break;
		case UNEG:
			sprintf(buffer, format_single, codePtr - 1, "UNED\t", (int)o, "Unary LOGIC NEGATION");
			break;
		case UBNEG:
			sprintf(buffer, format_single, codePtr - 1, "UBNEG\t", (int)o, "Unary BYTE NEGATION");
			break;
		case PRINT:
			sprintf(buffer, format_single, codePtr - 1, "PRINT\t", (int)o, "PRINTs top of the stack");
			break;
		case JMP:
			addr = readShort(code->code + codePtr);
			if (addr >= 0)
				addr += codePtr + 2;
			else
				addr += codePtr - 1;
			sprintf(buf2, "JMP to <%i>", addr);
			sprintf(buffer, format_tripple, codePtr - 1, "JMP\t", (int)o, (code->code + codePtr)[0], (code->code + codePtr + 1)[0], buf2);
			codePtr += 2;
			break;
		case JZERO:
			addr = readShort(code->code + codePtr);
			if (addr >= 0)
				addr += codePtr + 2;
			else
				addr += codePtr - 1;
			sprintf(buf2, "JUMP on ZERO on stack to <%i>", addr);
			sprintf(buffer, format_tripple, codePtr - 1, "JZERO\t", (int)o, (code->code + codePtr)[0], (code->code + codePtr + 1)[0], buf2);
			codePtr += 2;
			break;
		case JNZERO:
			addr = readShort(code->code + codePtr);
			if (addr >= 0)
				addr += codePtr + 2;
			else
				addr += codePtr - 1;
			sprintf(buf2, "JUMP on not ZERO on stack to <%i>", addr);
			sprintf(buffer, format_tripple, codePtr - 1, "JNZERO\t", (int)o, (code->code + codePtr)[0], (code->code + codePtr + 1)[0], buf2);
			codePtr += 2;
			break;
			break;
		case PUSH_VAR:
			param = readShort(code->code + codePtr);
			code->GetIdentifier(param, &ident);
			sprintf(buf2, "PUSH variable <%i>=\"%s\" on stack", param, ident.c_str());
			sprintf(buffer, format_tripple, codePtr - 1, "PUSH_VAR", (int)o, (code->code + codePtr)[0], (code->code + codePtr + 1)[0], buf2);
			codePtr += 2;
			break;
		case PUSH_CONST:
			param = readShort(code->code + codePtr);
			code->GetConstant(param, &iparam);
			sprintf(buf2, "PUSH integer const <%i>=\"%i\" on stack", param, iparam);
			sprintf(buffer, format_tripple, codePtr - 1, "PUSH_CONST", (int)o, (code->code + codePtr)[0], (code->code + codePtr + 1)[0], buf2);
			codePtr += 2;
			break;
		case PRINT_LIT:
			param = readShort(code->code + codePtr);
			code->GetString(param, &ident);
			sprintf(buf2, "PRINT string literar <%i>=\"%s\"", param, ident.c_str());
			sprintf(buffer, format_tripple, codePtr - 1, "PRINT_LIT", (int)o, (code->code + codePtr)[0], (code->code + codePtr + 1)[0], buf2);
			codePtr += 2;
			break;
		case PRINT_FRM:
			param = readShort(code->code + codePtr);
			code->GetString(param, &ident);
			sprintf(buf2, "PRINT formatted string with format <%i>=\"%s\" and stack", param, ident.c_str());
			sprintf(buffer, format_tripple, codePtr - 1, "PRINT_FRM", (int)o, (code->code + codePtr)[0], (code->code + codePtr + 1)[0], buf2);
			codePtr += 2;
			break;
		case STORE_VAR:
			param = readShort(code->code + codePtr);
			code->GetIdentifier(param, &ident);
			sprintf(buf2, "STORE into variable <%i>=\"%s\"", param, ident.c_str());
			sprintf(buffer, format_tripple, codePtr - 1, "STORE_VAR", (int)o, (code->code + codePtr)[0], (code->code + codePtr + 1)[0], buf2);
			codePtr += 2;
			break;
		case SCAN:
			param = readShort(code->code + codePtr);
			code->GetIdentifier(param, &ident);
			sprintf(buf2, "SCAN into variable <%i>=\"%s\"", param, ident.c_str());
			sprintf(buffer, format_tripple, codePtr - 1, "SCAN\t", (int)o, (code->code + codePtr)[0], (code->code + codePtr + 1)[0], buf2);
			codePtr += 2;
			break;
		}

		out << buffer << endl;
		buffer[0] = '\0';
	}
}
