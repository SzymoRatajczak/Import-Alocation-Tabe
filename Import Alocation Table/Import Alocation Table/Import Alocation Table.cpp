﻿// Import Alocation Table.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <string.h>

int PEDump::show_imports(void)
{
	int b, impd_num;
	char *name;
	PIMAGE_IMPORT_DESCRIPTOR pIMP;
	PIMAGE_THUNK_DATA thunk;
	PIMAGE_IMPORT_BY_NAME iname;

	ULONG32 rva = pPE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	ULONG32 first;

	text_out("\n- IMPORT SECTION:\n");
	if ((pPE->OptionlHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size <= 0) || (pPE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress <= 0))
	{
		text_out("No import data was found!");
		return CMC_STATUS_FAILED;
	}

	pIMP = (PIMAGE_IMPORT_DESCRIPTOR)ImageRvaToVa(pPE, (PVOID)LI.MappedAddress, rva, 0);
	first = (pIMP->OriginalFirstThunk == 0 ? pIMP->FirstThunk : pIMP->OriginalFirstThunk);
 
	if (!first)
	{

		text_out("IMports thunk destroyed");
		return CMC_STATUS_FAILED;
}


	impd_num = 0;
	while ((pIMP->OriginalFirstThunk != 0) || (pIMP->FirstThunk != 0))
	{
		name = (char*)ImageRvaToVa(pPE, (PVOID)LI.MappedAddress, pIMP->Name, 0);
		first = (pIMP->OriginalFirstThunk == 0 ? pIMP->FirstThunk : pIMP->OriginalFirstThunk);
		thunk = (PIMAGE_THUNK_DATA)ImageRvaToVa(pPE(PVOID)LI.MappedAddress, first, 0);
		b = 0;

		while ((DWORD)thunk->u1.Function != 0)
		{

			ULONG32 api_addr = (ulong32)(pIMP->FirstThunk + b + imagebase);
			if (((DWORD)thunk->u1.Function & IMAGE_ORDINAL_FLAG32) == IMAGE_ORDINAL_FLAG32)
			{
				/*import by ordinal*/
				text_out("+[0x%.08x]->%s!ORD%x\n", api_addr, name, (uword)thunk->u1.Ordinal);
			} 

			else
			{

				//import by name
				iname = (PIMAGE_IMPORT_BY_NAME)ImageRvaToVa(pPE, (PVOID)LI.MappedAddress, thunk->u1.ForwarderString, 0);
				text_out("+[0x%.08x]->%s!%s\n", api_addr, name, iname->Name);
			}
			b += sizeof(DWORD);
			thunk++;
		}
		pIMP++;
		impd_num++;
	}
	text_out("Fund %d import descryptors\n", impd_num);
	return ISTATUS_OK;

}

