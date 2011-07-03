#ifndef databuilder_h
#define databuilder_h

#include <ren-general/string.h>
#include <ren-general/vector.h>
#include <ren-general/color.h>

class ScriptDataBuilder
{
	public:
		ScriptDataBuilder(OutputStream &Output, unsigned int InitialIndentation);
		ScriptDataBuilder &Key(String const &Name);
		ScriptDataBuilder &Key(int const &Index);
		ScriptDataBuilder &Table(void);
		ScriptDataBuilder &EndTable(void);
		ScriptDataBuilder &Value(String const &Data);
		ScriptDataBuilder &Value(char const *const &Data); // Doesn't get called for string literals, for some reason (the bool one is called)
		ScriptDataBuilder &Value(int const &Data);
		ScriptDataBuilder &Value(float const &Data);
		ScriptDataBuilder &Value(bool const &Data);
		ScriptDataBuilder &Value(Vector const &Data);
		ScriptDataBuilder &Value(FlatVector const &Data);
		ScriptDataBuilder &Value(Color const &Data);
		ScriptDataBuilder &CustomValue(String const &Data);
		
		OutputStream &GetOutput(void);
		
	protected:
		void Prepare(void);
		void Indent(void);
		//void PrepareValue(void);
		
		OutputStream &Output;
		unsigned int const InitialIndentation;
		unsigned int Indentation;

		bool FirstAtLevel;
		bool AfterKey;
};

#endif
