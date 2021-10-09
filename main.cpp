#include "AnnexBReader.h"

//void printStack(void)
//{
//	unsigned int   i;
//	void* stack[100];
//	unsigned short frames;
//	SYMBOL_INFO* symbol;
//	HANDLE         process;
//
//	process = GetCurrentProcess();
//
//	SymInitialize(process, NULL, TRUE);
//
//	frames = CaptureStackBackTrace(0, 100, stack, NULL);
//	symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
//	symbol->MaxNameLen = 255;
//	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
//
//	for (i = 0; i < frames; i++)
//	{
//		SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
//
//		printf("%i: %s - 0x%0X\n", frames - i - 1, symbol->Name, symbol->Address);
//	}
//
//	free(symbol);
//}








int main()
{



	AnnexBReader reader;
	bool isOpen = reader.open("./data/cabac.h264");

	return EXIT_SUCCESS;
}


