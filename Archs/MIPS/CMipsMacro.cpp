#include "stdafx.h"
#include "CMipsMacro.h"
#include "MipsMacros.h"
#include "CMipsInstruction.h"
#include "Core/Common.h"
#include "Mips.h"
#include "MipsOpcodes.h"

CMipsMacro::CMipsMacro(int num, tMipsMacroVars& InputVars)
{
	CStringList List;

	InstructionAmount = MipsMacros[num].MaxOpcodes;
	Instructions = new CMipsInstruction[InstructionAmount];
	SpaceNeeded = InstructionAmount*4;
	MacroNum = num;

	Data.i[0] = InputVars.i[0];
	Data.i[1] = InputVars.i[1];
	if (Data.i[0] == true) Data.i1.Load(InputVars.List[0]);
	if (Data.i[1] == true) Data.i2.Load(InputVars.List[1]);
	Data.rd = InputVars.rd;
	Data.rs = InputVars.rs;
	Data.rt = InputVars.rt;
	IgnoreLoadDelay = Mips.GetIgnoreDelay();

	Global.RamPos += SpaceNeeded;
}

CMipsMacro::~CMipsMacro()
{
	delete[] Instructions;
}


bool CMipsMacro::Validate()
{
	tMipsMacroValues Values;
	CStringList List;

	Values.rd = Data.rd;
	Values.rs = Data.rs;
	Values.rt = Data.rt;

	if (Data.i[0] == true && ParsePostfix(Data.i1,&List,Values.i1) == false)
	{
		if (List.GetCount() == 0)
		{
			QueueError(ERROR_ERROR,"Invalid expression");
		} else {
			for (int l = 0; l < List.GetCount(); l++)
			{
				QueueError(ERROR_ERROR,List.GetEntry(l));
			}
		}
		return false;
	}
	if (Data.i[1] == true && ParsePostfix(Data.i2,&List,Values.i2) == false)
	{
		if (List.GetCount() == 0)
		{
			QueueError(ERROR_ERROR,"Invalid expression");
		} else {
			for (int l = 0; l < List.GetCount(); l++)
			{
				QueueError(ERROR_ERROR,List.GetEntry(l));
			}
		}
		return false;
	}

	int NewNum = MipsMacros[MacroNum].Function(Values,MipsMacros[MacroNum].flags,Instructions);

	if (IgnoreLoadDelay == false && Mips.GetDelaySlot() == true && NewNum > 1)
	{
		QueueError(ERROR_WARNING,"Macro with multiple opcodes used inside a delay slot");
	}

	for (int i = 0; i < NewNum; i++)
	{
		Instructions[i].Validate();
	}

	if (NewNum != InstructionAmount)	// anzahl ge�ndert
	{
		InstructionAmount = NewNum;
		SpaceNeeded = InstructionAmount*4;
		return true;
	} else {
		return false;
	}
}

void CMipsMacro::Encode()
{
	for (int i = 0; i < InstructionAmount; i++)
	{
		Instructions[i].Encode();
	}
}

void CMipsMacro::WriteTempData(FILE*& Output)
{
	for (int i = 0; i < InstructionAmount; i++)
	{
		Instructions[i].WriteTempData(Output);
	}
}