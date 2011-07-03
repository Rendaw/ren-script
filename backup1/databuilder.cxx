#include "databuilder.h"

#include <cassert>
#include <cstdlib>

ScriptDataBuilder::ScriptDataBuilder(OutputStream &Output, unsigned int InitialIndentation) :
	Output(Output), InitialIndentation(InitialIndentation), Indentation(InitialIndentation), AfterKey(false)
	{}

ScriptDataBuilder &ScriptDataBuilder::Table(void)
{
	if (AfterKey)
	{
		Output << "\n";
		AfterKey = false;
	}
	Indent();
	Output << "{\n";
	Indentation++;
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::EndTable(void)
{
	assert(Indentation > InitialIndentation);
	if (Indentation == 0) exit(1); // Just in case, for release mode
	Indentation--;
	Indent();
	Output << "},\n";
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Key(String const &Name)
{
	assert(!AfterKey);
	Indent();
	Output << "[\"" << Name << "\"] = ";
	AfterKey = true;
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(String const &Data)
{
	if (AfterKey)
		AfterKey = false;
	else Indent();
	Output << "\"" << Data << "\",\n";
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(int const &Data)
{
	if (AfterKey)
		AfterKey = false;
	else Indent();
	Output << Data << ",\n";
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(float const &Data)
{
	if (AfterKey)
		AfterKey = false;
	else Indent();
	Output << Data << ",\n";
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(bool const &Data)
{
	if (AfterKey)
		AfterKey = false;
	else Indent();
	Output << (Data ? "true" : "false") << ",\n";
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(Vector const &Data)
{
	if (AfterKey)
		AfterKey = false;
	else Indent();
	Output << "{" << Data[0] << ", " << Data[1] << ", " << Data[2] << "},\n";
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(FlatVector const &Data)
{
	if (AfterKey)
		AfterKey = false;
	else Indent();
	Output << "{" << Data[0] << ", " << Data[1] << "},\n";
	return *this;
}

ScriptDataBuilder &ScriptDataBuilder::Value(Color const &Data)
{
	if (AfterKey)
		AfterKey = false;
	else Indent();
	Output << "{" << Data.Red << ", " << Data.Green << ", " << Data.Blue << ", " << Data.Alpha << "},\n";
	return *this;
}

void ScriptDataBuilder::Indent(void)
{
	for (unsigned int CurrentLevel = 0; CurrentLevel < Indentation; CurrentLevel++)
		Output << "\t";
}
