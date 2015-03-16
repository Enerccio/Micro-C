#include "Compiler.h"

extern FILE* yyin;
extern int yyparse();
extern int yydebug;
extern Block* program;
unsigned long long OpData::counter = 0;

using namespace std;

void writeByte(char b, char* pnt)
{
	*pnt = b;
}

void writeShort(short s, char* pnt)
{
	char* t = (char*)&s;
	pnt[0] = t[0];
	pnt[1] = t[1];
}

Compiler::Compiler()
{
	frame = NULL;
}


Compiler::~Compiler()
{
}

bool Compiler::Compile(FILE* in, CuCFile* out, char* fnamein, bool debug, bool checkTimestamp)
{
	input = in;
	output = out;
	this->debug = debug;
	this->fnamein = fnamein;
	if (checkTimestamp && CheckTimestamp())
		return false;

	if (debug)
		cout << "uC Compiler (C++): " << endl;

	Parse();
	if (CheckErrors())
		return false;

	forward_map.clear();
	return Compile();
}

void Compiler::Parse()
{
	if (debug)
		cout << "Parsing source code into a tree. " << endl;

	yyin = input;
	yydebug = debug ? 1 : 0;
	yyparse();

	ast = program;
}

bool Compiler::CheckTimestamp()
{
	return false;
}

bool Compiler::CheckErrors()
{
	if (ast == NULL)
	{
		cout <<  "Failed to compile file." << endl;
		return true;
	}

	return CheckForCorrectLoops(ast, 0);
}

bool Compiler::CheckForCorrectLoops(Statement* st, unsigned int jmpCount)
{
	if (st == NULL)
		return false;

	if (st->GetType() == BREAK_STATEMENT || st->GetType() == CONTINUE_STATEMENT)
	{
		if (jmpCount == 0)
		{
			cout << "Break/continue statement outside the loop" << endl;
			return true;
		}
	}

	bool failed = false;
	StatementList::iterator it;
	Block* block;

	switch (st->GetType())
	{
	case BLOCK:
		block = (Block*)st;
		for (it = block->statements.begin(); it != block->statements.end(); it++)
		{
			failed = CheckForCorrectLoops(*it, jmpCount);
			if (failed == true)
				break;
		}
		break;
	case IF_STATEMENT:
		 failed = CheckForCorrectLoops(((IfStatement*)st)->exec, jmpCount);
		 if (failed)
			 return failed;
		 failed = CheckForCorrectLoops(((IfStatement*)st)->fail, jmpCount);
		 break;
	case FOR_STATEMENT:
		failed = CheckForCorrectLoops(((ForStatement*)st)->exec, jmpCount+1);
		break;
	case DO_STATEMENT:
		failed = CheckForCorrectLoops(((DoStatement*)st)->exec, jmpCount + 1);
		break;
	case WHILE_STATEMENT:
		failed = CheckForCorrectLoops(((WhileStatement*)st)->exec, jmpCount + 1);
		break;
	}

	return failed;
}

bool Compiler::Compile()
{
	Optimize();

	vector<OpData*> v;
	CompileBlock(ast, &v);

	unsigned int outputLen = 0;
	for (unsigned int i = 0; i < v.size(); i++)
	{
		OpData* data = v[i];
		switch (data->code)
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
			outputLen += 1;
			break;
		case JMP:
		case JZERO:
		case JNZERO:
		case PUSH_VAR:
		case PUSH_CONST:
		case PRINT_LIT:
		case PRINT_FRM:
		case STORE_VAR:
		case SCAN:
			outputLen += 3;
		}
	}

	char* bytecode = new char[outputLen];
	char* entry = bytecode;
	unsigned int ppos = 0;
	for (unsigned int i = 0; i < v.size(); i++)
	{
		OpData* data = v[i];
		switch (data->code)
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
			data->entry = bytecode;
			data->pos = ppos++;
			writeByte(data->code, bytecode++);
			break;
		case JMP:
		case JZERO:
		case JNZERO:
			data->entry = bytecode;
			data->pos = ppos++;
			writeByte(data->code, bytecode++);
			writeShort(data->sarg, bytecode);
			bytecode += 2;
			break;
		case PUSH_VAR:
		case PUSH_CONST:
		case PRINT_LIT:
		case PRINT_FRM:
		case STORE_VAR:
		case SCAN:
			data->entry = bytecode;
			data->pos = ppos++;
			writeByte(data->code, bytecode++);
			writeShort(data->arg, bytecode);
			bytecode += 2;
			break;
		}
	}

	output->code = (unsigned char*)entry;
	output->code_len = outputLen;

	for (unsigned int i = 0; i < v.size(); i++)
	{
		delete v[i];
	}

	delete ast;
	return true;
}

void Compiler::Optimize()
{

}

void Compiler::CompileBlock(Block* block, vector<OpData*>* v)
{
	PUSH_STATIC_FRAME();
	StatementList::iterator it;
	for (it = block->statements.begin(); it != block->statements.end(); it++)
	{
		CompileStatement(*it, v);
	}
	POP_STATIC_FRAME();
}

void Compiler::CompileStatement(Statement* st, vector<OpData*>* v)
{
	switch (st->GetType())
	{
	case BLOCK:
	{
				  CompileBlock((Block*)st, v);
	} break;
	case NUMBER_EXP:
	{
					   Int* i = (Int*)st;
					   v->push_back(new OpData(PUSH_CONST, output->AddConstant(i->value)));
					   Add(3);
	} break;
	case IDENTIFIER_EXP:
	{
						   Identifier* i = (Identifier*)st;
						   v->push_back(new OpData(PUSH_VAR, output->AddLiterar(i->ident)));
						   Add(3);
	} break;
	case UNARY_OPERATION:
	{
							UnaryOperation* op = (UnaryOperation*)st;
							
							switch (op->type)
							{
							case O_ADD_BEFORE: 
							case O_SUB_BEFORE:
							{
												 unsigned short ident = output->AddLiterar(((Identifier*)op->expression)->ident);
												 unsigned short one = output->AddConstant(1);
												 v->push_back(new OpData(PUSH_VAR, ident));
												 v->push_back(new OpData(PUSH_CONST, one));
												 if (op->type == O_ADD_BEFORE)
													 v->push_back(new OpData(ADD));
												 else
													 v->push_back(new OpData(SUB));
												 v->push_back(new OpData(STORE_VAR, ident));
												 v->push_back(new OpData(PUSH_VAR, ident));
												 Add(13);
							} break;
							case O_ADD_AFTER:
							case O_SUB_AFTER:
							{
												 unsigned short ident = output->AddLiterar(((Identifier*)op->expression)->ident);
												 unsigned short one = output->AddConstant(1);
												 v->push_back(new OpData(PUSH_VAR, ident));
												 v->push_back(new OpData(PUSH_VAR, ident));
												 v->push_back(new OpData(PUSH_CONST, one));
												 if (op->type == O_ADD_BEFORE)
													 v->push_back(new OpData(ADD));
												 else
													 v->push_back(new OpData(SUB));
												 v->push_back(new OpData(STORE_VAR, ident));
												 Add(13);
							} break;
							default:
							{
									   CompileStatement(op->expression, v);
									   switch (op->type)
									   {
									   case O_UNARY_MINUS: v->push_back(new OpData(UMIN)); break;
									   case O_LOG_NEG: v->push_back(new OpData(UNEG)); break;
									   case O_BIT_NEG: v->push_back(new OpData(UBNEG)); break;
									   }
									   Add(1);
							} break;
							}

							if (op->isStatement)
							{
								v->push_back(new OpData(POP));
								Add(1);
							}
	} break;
	case BINARY_OPERATION:
	{
							 BinaryOperation* bop = (BinaryOperation*)st;

							 switch (bop->type)
							 {
							 case O_LOG_AND:
							 case O_LOG_OR:
							 {
											  CompileStatement(bop->expression1, v);
											  OpData* jmp = new OpData;
											  OpData* sjmp = new OpData;
											  OpData* tjmp = new OpData;
											  v->push_back(jmp);
											  if (bop->type == O_LOG_AND) jmp->code = JZERO; else jmp->code = JNZERO;
											  Add(3);
											  forward_map.insert(make_pair(jmp->cnt, (unsigned short)0));
											  CompileStatement(bop->expression2, v);
											  v->push_back(sjmp);
											  Add(3);
											  forward_map.insert(make_pair(sjmp->cnt, (unsigned short)0));
											  if (bop->type == O_LOG_AND) sjmp->code = JZERO; else sjmp->code = JNZERO;
											  v->push_back(new OpData(PUSH_CONST, output->AddConstant(1)));
											  v->push_back(new OpData(JMP, (signed short)4));
											  Add(9);
											  v->push_back(new OpData(PUSH_CONST, output->AddConstant(0)));
											  jmp->sarg = (signed short)forward_map[jmp->cnt];
											  sjmp->sarg = (signed short)forward_map[sjmp->cnt];
											  forward_map.erase(jmp->cnt);
											  forward_map.erase(sjmp->cnt);
											  v->push_back(new OpData());
							 } break;
							 case O_SET:
							 {
												CompileStatement(bop->expression2, v);
												v->push_back(new OpData(STORE_VAR, output->AddLiterar(((Identifier*)bop->expression1)->ident)));
												v->push_back(new OpData(PUSH_VAR, output->AddLiterar(((Identifier*)bop->expression1)->ident)));
												Add(1);
							 } break;
							 case O_SET_MUL:
							 case O_SET_DIV:
							 case O_SET_ADD:
							 case O_SET_MOD:
							 case O_SET_AND:
							 case O_SET_OR:
							 case O_SET_SHIFT_L:
							 case O_SET_SHIFT_R:
							 case O_SET_XOR:
							 {
											   v->push_back(new OpData(PUSH_VAR, output->AddLiterar(((Identifier*)bop->expression1)->ident)));
											   Add(3);
											   CompileStatement(bop->expression2, v);
											   switch (bop->type)
											   {
											   case O_SET_MUL: v->push_back(new OpData(MUL)); break;
											   case O_SET_DIV: v->push_back(new OpData(DIV)); break;
											   case O_SET_ADD: v->push_back(new OpData(ADD)); break;
											   case O_SET_MOD: v->push_back(new OpData(MOD)); break;
											   case O_SET_AND: v->push_back(new OpData(BAND)); break;
											   case O_SET_OR: v->push_back(new OpData(BOR)); break;
											   case O_SET_SHIFT_L: v->push_back(new OpData(LSHIFT)); break;
											   case O_SET_SHIFT_R: v->push_back(new OpData(RSHIFT)); break;
											   case O_SET_XOR: v->push_back(new OpData(BXOR)); break;
											   }
											   v->push_back(new OpData(STORE_VAR, output->AddLiterar(((Identifier*)bop->expression1)->ident)));
											   Add(4);
							 } break;
							 default:
							 {
										CompileStatement(bop->expression1, v);
										CompileStatement(bop->expression2, v);
										switch (bop->type)
										{
										case O_PLUS: v->push_back(new OpData(ADD)); break;
										case O_MINUS: v->push_back(new OpData(SUB)); break;
										case O_MUL: v->push_back(new OpData(MUL)); break;
										case O_DIV: v->push_back(new OpData(DIV)); break;
										case O_SHIFT_LEFT: v->push_back(new OpData(LSHIFT)); break;
										case O_SHIFT_RIGHT: v->push_back(new OpData(RSHIFT)); break;
										case O_EQUALS: v->push_back(new OpData(EQ)); break;
										case O_LESS: v->push_back(new OpData(LESS)); break;
										case O_MORE: v->push_back(new OpData(MORE)); break;
										case O_LESS_EQ: v->push_back(new OpData(LEQ)); break;
										case O_MORE_EQ: v->push_back(new OpData(MEQ)); break;
										case O_BIT_OR: v->push_back(new OpData(BOR)); break;
										case O_BIT_AND: v->push_back(new OpData(BAND)); break;
										case O_BIT_XOR: v->push_back(new OpData(BXOR)); break;
										}
										Add(1);
							 } break;
							 }

							 if (bop->isStatement)
							 {
								 v->push_back(new OpData(POP));
								 Add(1);
							 }

	} break;
	case IF_STATEMENT:
	{
						 IfStatement* ifs = (IfStatement*)st;
						 CompileStatement(ifs->test, v);
						 OpData* jmp = new OpData(JZERO);
						 v->push_back(jmp);
						 Add(3);
						 forward_map.insert(make_pair(jmp->cnt, (unsigned short)0));
						 CompileStatement(ifs->exec, v);
						 if (ifs->fail == NULL)
						 {
							 jmp->sarg = (signed short)forward_map[jmp->cnt];
							 forward_map.erase(jmp->cnt);
							 v->push_back(new OpData(NOP));
							 Add(1);
						 }
						 else 
						 {
							 OpData* sjmp = new OpData(JMP);
							 v->push_back(sjmp);
							 Add(3);
							 forward_map.insert(make_pair(sjmp->cnt, (unsigned short)0));

							 CompileStatement(ifs->fail, v);
							 jmp->sarg = (signed short)forward_map[jmp->cnt];
							 sjmp->sarg = (signed short)forward_map[sjmp->cnt];
							 forward_map.erase(jmp->cnt);
							 forward_map.erase(sjmp->cnt);
							 v->push_back(new OpData(NOP));
							 Add(1);
						 }
	} break;
	case SCAN_STATEMENT:
	{
						   ScanStatement* s = (ScanStatement*)st;
						   v->push_back(new OpData(SCAN, output->AddLiterar(s->ident->ident)));
						   Add(1);
	} break;
	case PRINT_EXPR_STATEMENT:
	{
								 PrintExpressionStatement* ps = (PrintExpressionStatement*)st;
								 CompileStatement(ps->expression, v);
								 v->push_back(new OpData(PRINT));
								 Add(1);
	} break;
	case PRINT_STRING_STATEMENT:
	{
								 PrintStringStatement* ps = (PrintStringStatement*)st;
								 v->push_back(new OpData(PRINT_LIT, output->AddString(ps->print)));
								 Add(3);
	} break;
	case PRINT_FORMAT_STATEMENT:
	{
								   PrintFormattedStatement* ps = (PrintFormattedStatement*)st;
								   CompileStatement(ps->expression, v);
								   v->push_back(new OpData(PRINT_LIT, output->AddString(ps->format)));
								   Add(3);
	} break;
	case CONTINUE_STATEMENT:
	{
							   for (int i = 0; i < frame->frameCount; i++)
							   {
								   v->push_back(new OpData(POP_FRAME));
								   Add(1);
							   }

							   v->push_back(new OpData(JMP, ((signed short)-((signed short)frame->startPos))));
							   Add(3);
	} break;
	case FOR_STATEMENT:
	{
						  PUSH_STATIC_FRAME();

						  ForStatement* fs = (ForStatement*)st;
						  CompileStatement(fs->init, v);
						  PushFrame();
						  frame->startPos = 0;

						  CompileStatement(fs->test, v);
						  OpData* jmpToEnd = new OpData(JZERO);
						  v->push_back(jmpToEnd);
						  Add(3);
						  forward_map.insert(make_pair(jmpToEnd->cnt, (unsigned short)0));

						  CompileStatement(fs->exec, v);
						  CompileStatement(fs->cycle, v);
						  
						  jmpToEnd->sarg = (signed short)forward_map[jmpToEnd->cnt];
						  forward_map.erase(jmpToEnd->cnt);

						  for (DList::iterator it = frame->endHooks.begin(); it != frame->endHooks.end(); it++)
						  {
							  OpData* hook = *it;
							  hook->sarg = (signed short)forward_map[hook->cnt];
							  forward_map.erase(hook->cnt);
						  }

						  PopFrame();
						  Add(1);
						  v->push_back(new OpData());
						  POP_STATIC_FRAME();
	}
	}
}

void Compiler::PushFrame()
{
	Frame* f = new Frame;
	f->parent = frame;
	frame = f;
}

void Compiler::PopFrame()
{
	if (frame != NULL)
	{
		Frame* f = frame;
		frame = f->parent;
		delete f;
	}
}