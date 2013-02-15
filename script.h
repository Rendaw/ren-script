#ifndef script_h
#define script_h

// Lua scripting tools

/*
Lua Structure
-- When a file is loaded (appended), it is run immediately
-- Use asserts to make sure the stack is as expected.  Assert after pulling elements from tables.
*/

#ifndef INTREELUA
extern "C"
{
#endif
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
#ifndef INTREELUA
}
#endif

#include <functional>

#include "../ren-general/string.h"
#include "../ren-general/auxinclude.h"
#include "../ren-general/vector.h"
#include "../ren-general/color.h"
#include "../ren-general/lifetime.h"

class Script
{
	public:
		// Unique index utility - creates an index from an address
		static String UniqueIndex(void *Address, const String &Suffix);

		Script(void);
		Script(lua_State *FromInstance);
		~Script(void);
		
		lua_State *GetState(void);

		String DumpStack(unsigned int Depth = 0);
		
		// Code loading and execution
		bool Do(const String &ScriptName, bool ShowErrors);
		
		// Stack information and manipulation
		unsigned int Height(void);
		void Pop(void);
		void ClearStack(void);
		void Lift(int Position);
		void Duplicate(int Position);
		
		// Information get functions
		bool IsNil(void);
		bool IsString(void);
		bool IsBoolean(void);
		bool IsNumber(void);
		bool IsTable(void);
		bool IsFunction(void);
		String GetType(void); // For debug, mostly

		void AssertNil(String const &Message);
		void AssertString(String const &Message);
		void AssertBoolean(String const &Message);
		void AssertNumber(String const &Message);
		void AssertTable(String const &Message);
		void AssertFunction(String const &Message);
		void AssertVector(String const &Message); // TODO Implement an interface for getting generic information about custom simple types (e.g. Vector::GetDataStructure() returns tree of floats/ints/strings/etc that can be used to assert properly)
		void AssertFlatVector(String const &Message);
		void AssertColor(String const &Message);

		String GetString(void);
		int GetInteger(void);
		unsigned int GetUnsignedInteger(void);
		int GetIndex(void); // Like integer, but less one
		float GetFloat(void);
		bool GetBoolean(void);

		Vector GetVector(void);
		FlatVector GetFlatVector(void);
		Color GetColor(void);
		
		void SaveGlobal(const String &GlobalName);
		void SaveInternal(const String &InternalName); // Stores a value in Lua registry

		// Information put functions
		void PushNil(void);
		void PushString(const String &Data);
		void PushInteger(const int &Data);
		void PushIndex(const int &Data);
		void PushFloat(const float &Data);
		void PushBoolean(const bool &Data);

		typedef std::function<int(Script &State)> Function;
		void PushFunction(Function NewFunction);
		void Error(const String &Message);

		void PushTable(void);

		void PushGlobal(const String &GlobalName);
		void PushInternal(const String &ValueName); // Pushes a value from Lua registry

		// Table methods
		bool IsEmpty(void);

		void PullElement(const String &Index);
		void PullElement(int Index);
		bool TryElement(const String &Index);

		bool PullNext(bool PopTableWhenDone = true);

		void Iterate(std::function<bool(Script &State)> Processor);

		void PutElement(const String &Index);
		void PutElement(int Index);

		// Lua function methods
		void CallHook(const String &HookName, int Arguments = 0);

		// Hacky stuffs
		void PushPointer(void *Pointer);
		void *GetPointer(void);

	private:
		static int HandleRegisteredFunction(lua_State *State);

		lua_State *Instance;
		bool Owner;
		std::list<std::function<int(Script State)> > FunctionStorage;
};

#endif
