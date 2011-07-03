#ifndef databuilder_h
#define databuilder_h

#include "../general/string.h"
#include "../general/vector.h"
#include "../general/color.h"

class ScriptDataBuilder
{
	public:
		ScriptDataBuilder(OutputStream &Output, unsigned int InitialIndentation);
		ScriptDataBuilder &Table(void);
		ScriptDataBuilder &EndTable(void);
		ScriptDataBuilder &Key(String const &Name);
		ScriptDataBuilder &Value(String const &Data);
		ScriptDataBuilder &Value(int const &Data);
		ScriptDataBuilder &Value(float const &Data);
		ScriptDataBuilder &Value(bool const &Data);
		ScriptDataBuilder &Value(Vector const &Data);
		ScriptDataBuilder &Value(FlatVector const &Data);
		ScriptDataBuilder &Value(Color const &Data);
		
	protected:
		void Indent(void);
		
		OutputStream &Output;
		unsigned int const InitialIndentation;
		unsigned int Indentation;
		
		bool AfterKey;
};

#endif
