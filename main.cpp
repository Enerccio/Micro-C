#include "Compiler.h"
#include "Interpret.h"
#include "CuCFile.h"
#include "Disassembler.h"
#include "MicroCVM.h"

#include <fstream>
#include <string>

#include "ast.h"
extern int yyparse();
extern Block* program;

void compile(char* from, char* to)
{
	FILE* input = fopen(from, "r");
	CuCFile* output = new CuCFile();
	output->SetName(std::string(from));

	Compiler cmp; 

	bool compiled = cmp.Compile(input, output, from, true, false);

	fclose(input);

	std::ofstream out;
	out.open((const char*)to, std::ios::binary);

	if (compiled)
		CuCFile::save(output, &out);
	out.close();
	delete output;
}

void disassemble(char* from, char* to)
{
	std::ifstream in;
	in.open((const char*)from, std::ios::binary);
	std::ofstream out;
	out.open((const char*)to);

	CuCFile* file = CuCFile::parse(&in);
	in.close();

	if (file != NULL)
	{
		Disassembler d;
		d.Dissassemble(out, file);
	}
	
	out.close();
}

void run(char* source)
{
	std::ifstream in;
	in.open((const char*)source, std::ios::binary);
	CuCFile* file = CuCFile::parse(&in);
	in.close();

	if (file != NULL)
	{
		MicroCVM vm;
		vm.Load(file);
		while (vm.HasNext())
			vm.Iterate();
	}

	delete file;
}


int main(int argc, char** argv)
{
	for (int i = 0; i<argc; i++)
		printf("%s\n", argv[i]);

	if (argc > 3){
		if (strcmp(argv[1], "-c") == 0)
			compile(argv[2], argv[3]);
		else if (strcmp(argv[1], "-d") == 0)
			disassemble(argv[2], argv[3]);
	}

	// debug
	if (argc == 3)
	{
		compile(argv[1], "tmp");
		disassemble("tmp", argv[2]);
		printf("\n\n");
		run("tmp");
	}

	return 0;
}