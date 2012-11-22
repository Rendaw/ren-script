#include "databuilder.h"

#include <cassert>
#include <cstdlib>

String ScriptDataBuilder::Escape(String const &Input)
{
	MemoryStream Out;
	for (auto &Character : Input)
	{
		if (Character == '\\')
			Out << "\\\\";
		else if (Character == '"')
			Out << "\\\"";
		else Out << Character;
	}
	return Out;
}

ScriptDataBuilder::ScriptDataBuilder(OutputStream &Output, unsigned int InitialIndentation) :
	Output(Output), InitialIndentation(InitialIndentation), Indentation(InitialIndentation), 
	FirstAtLevel(true), AfterKey(false)
	{}

ScriptDataBuilder &ScriptDataBuilder::Key(String const &Name)
{
	assert(!AfterKey);
	Prepare();
	Output << "[\"" << Escape(Name) << "\"] = ";
	AfterKey = true;
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Key(int const &Index)
{
	assert(!AfterKey);
	Prepare();
	Output << "[" << Index << "] = ";
	AfterKey = true;
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Table(void)
{
	if (AfterKey)
	{
		AfterKey = false;
		Output << "\n";
		Indent();
	}
	else Prepare();
	Output << "{\n";
	Indentation++;
	FirstAtLevel = true;
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::EndTable(void)
{
	assert(Indentation > InitialIndentation);
	if (Indentation == 0) exit(1); // Just in case, for release mode
	assert(!AfterKey);
	FirstAtLevel = false;
	Indentation--;
	Output << "\n";
	Indent();
	Output << "}";
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(String const &Data)
{
	Prepare();
	Output << "\"" << Escape(Data) << "\"";
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(int const &Data)
{
	Prepare();
	Output << Data;
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(unsigned int const &Data)
{
	Prepare();
	Output << Data;
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(float const &Data)
{
	Prepare();
	Output << Data;
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(bool const &Data)
{
	Prepare();
	Output << (Data ? "true" : "false");
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(Vector const &Data)
{
	Prepare();
	Output << "{" << Data[0] << ", " << Data[1] << ", " << Data[2] << "}";
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(FlatVector const &Data)
{
	Prepare();
	Output << "{" << Data[0] << ", " << Data[1] << "}";
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(Color const &Data)
{
	Prepare();
	Output << "{" << Data.Red << ", " << Data.Green << ", " << Data.Blue << ", " << Data.Alpha << "}";
	return *this;
}
		
ScriptDataBuilder &ScriptDataBuilder::Function(std::list<String> const &Arguments, String const &Body)
{
	Prepare();
	Output << "function(";
	bool First = true;
	for (auto &Argument : Arguments) 
	{
		if (First) First = false;
		else Output << ", ";
		Output << Argument;
	}
	Output << ")\n";
	MemoryStream BodyStream(Body);
	String NextLine;
	while (BodyStream)
	{
		BodyStream >> NextLine;
		Indent();
		Output << "\t" << NextLine << "\n";
	}
	Indent();
	Output << "end";
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::CustomValue(String const &Data)
{
	Prepare();
	Output << Data;
	return *this;
}

OutputStream &ScriptDataBuilder::GetOutput(void)
{
	return Output;
}

void ScriptDataBuilder::Prepare(void)
{
	if (!AfterKey)
	{
		if (!FirstAtLevel) 
			Output << ",\n";
		
		Indent();
	}
	FirstAtLevel = false;
	AfterKey = false;
}

void ScriptDataBuilder::Indent(void)
{
	for (unsigned int CurrentLevel = 0; CurrentLevel < Indentation; CurrentLevel++)
		Output << "\t";
}
