#include <cassert>
#include <iostream>

#include <ren-script/databuilder.h>

void AssertEqual(String const &Got, String const &Expected)
{
	StringStream GotStream(Got), ExpectedStream(Expected);
	String GotLine, ExpectedLine;
	while (1)
	{
		if ((bool)getline(GotStream, GotLine) != (bool)getline(ExpectedStream, ExpectedLine))
			assert(false);
		if (!GotStream) break;
		std::cout << "Testing:\n\t" << GotLine << "\n\t" << ExpectedLine << std::endl;
		assert(GotLine == ExpectedLine);
	}
}

int main(int argc, char **argv)
{
	AssertEqual(ScriptDataBuilder::Escape("(\"n_ '!%"), "(\\\"n_ '!%");
	String const Unicode1(u8"\xE5\xAD\x90\xE4\xBE\x9B"), Unicode2(u8"\xE5\xA4\xA7\x20\xE4\xBA\xBA"), Unicode3(u8"\xE3\x83\x95\xE3\x82\xA6\xE3\x83\x81\xE3\x83\xA7\xE3\x82\xA6\xE7\xA7\x91");

	StringStream Output;
	Output << "x = function() return {\n";
	ScriptDataBuilder Builder(Output, 1);
	Builder.Key("a").Value(String("a"));
	Builder.Key(Unicode1).Value(Unicode1);
	Builder.Key(Unicode3).Value(Unicode3);
	Builder.Key(" \"a\"").Value(String("\" 'n%d"));
	Builder.Key(0).Value(0);
	Builder.Key(100).Value(100);
	Builder.Key(-100).Value(-100);
	Builder.Key("long").Value(100U);
	Builder.Key("float").Value(1.1f);
	Builder.Key("bool").Value(false);
	Builder.Key("vector").Value(Vector(1, 2, 3));
	Builder.Key("flatvector").Value(FlatVector(1, 2));
	Builder.Key("color").Value(Color(0.4f, 0.5f, 0.6f));
	Builder.Key("customvalue").CustomValue("\"red\"");
	Builder.Key("table").Table().Key("b").Value(String("b")).EndTable();
	Builder.Key("function").Function({"inputa", "inputb"}, "-- Do nothing\nprint \"Hello\"");
	Builder.Key("function2").Function({}, "print \"Ugug\"");
	Output << "\n} end\n";
	
	AssertEqual(Output.str(),
		"x = function() return {\n"
		"	[\"a\"] = \"a\",\n"
		"	[\"\xE5\xAD\x90\xE4\xBE\x9B\"] = \"\xE5\xAD\x90\xE4\xBE\x9B\",\n"
		"	[\"\xE3\x83\x95\xE3\x82\xA6\xE3\x83\x81\xE3\x83\xA7\xE3\x82\xA6\xE7\xA7\x91\"] = \"\xE3\x83\x95\xE3\x82\xA6\xE3\x83\x81\xE3\x83\xA7\xE3\x82\xA6\xE7\xA7\x91\",\n"
		"	[\" \\\"a\\\"\"] = \"\\\" 'n%d\",\n"
		"	[0] = 0,\n"
		"	[100] = 100,\n"
		"	[-100] = -100,\n"
		"	[\"long\"] = 100,\n"
		"	[\"float\"] = 1.1,\n"
		"	[\"bool\"] = false,\n"
		"	[\"vector\"] = {1, 2, 3},\n"
		"	[\"flatvector\"] = {1, 2},\n"
		"	[\"color\"] = {0.4, 0.5, 0.6, 1},\n"
		"	[\"customvalue\"] = \"red\",\n"
		"	[\"table\"] = \n"
		"	{\n"
		"		[\"b\"] = \"b\"\n"
		"	},\n"
		"	[\"function\"] = function(inputa, inputb)\n"
		"		-- Do nothing\n"
		"		print \"Hello\"\n"
		"	end,\n"
		"	[\"function2\"] = function()\n"
		"		print \"Ugug\"\n"
		"	end\n"
		"} end\n");
}

