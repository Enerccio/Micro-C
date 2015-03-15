#pragma once

namespace OP {

	enum OpCode
	{
		// System

			// No arguments
		NOP = 0,
			// No arguments
		PUSH_FRAME = 1,
			// No arguments
		POP_FRAME = 2,

		// Flow
			// 16 bit signed number of target instruction
		JMP = 16,
			// 16 bit signed number of target instruction
		JZERO = 17,
			// 16 bit signed number of target instruction
		JNZERO = 18,

		// Stack push/pop
			// 16 bit unsigned number to ident table
		PUSH_VAR = 32,
			// 16 bit unsigned number to const table
		PUSH_CONST = 33,
			// No arguments
		POP = 34,
			// 16 bit unsigned number to ident table
		STORE_VAR = 35,
			// No argments
		DUP_STACK = 36,

		// Stack binary operations
			// all of these have no args, they need 2 on stack, pop result to stack
		ADD = 48,
		SUB = 49,
		MUL = 50,
		DIV = 51,
		LSHIFT = 52,
		RSHIFT = 53,
		EQ = 54,
		LESS = 55,
		MORE = 56,
		LEQ  = 57,
		MEQ = 58,
		BOR = 59,
		BAND = 60,
		BXOR = 61,
		MOD = 62,

		// Stack unary operations
			// all of these have no args, they need one on stack, pop result to stack
		UMIN = 64,
		UNEG = 65,
		UBNEG = 66,

		// Functions
			// pops stack and prints it
		PRINT = 128,
			// 16 bit unsigned number to literar table
		PRINT_LIT = 129,
			// 16 bit unsigned number to literar table, pops stack and prins it
		PRINT_FRM = 130,
			// 16 bit unsigned number to literar table
		SCAN = 131
	};

	struct OpData
	{
		static unsigned long long counter;

		OpData(){ code = NOP; cnt = ++counter; }
		OpData(OpCode c) : code(c) { cnt = ++counter; }
		OpData(OpCode c, short a) : code(c), sarg(a) { cnt = ++counter; }
		OpData(OpCode c, unsigned short a) : code(c), arg(a) { cnt = ++counter; }

		OpCode code;
		unsigned short arg;
		short sarg;
		char* entry;
		unsigned int pos;
		unsigned long long cnt;
	};
}