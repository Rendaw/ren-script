#ifndef script_h
#define script_h

// Lua scripting tools

/*
Lua Structure
-- All Lua instances, when loaded, also get loaded with the standard libraries
-- All Lua instances, when loaded, are provided logging/error functions
-- When a file is loaded (appended), it is run immediately

EXT functions - non standard library standard functions
All registered functions go under table "ext", so "ext.SomeFunction()" would be
an appropriate call.
*/

// Other Rendaw libraries
#include <ren-general/string.h>
#include <ren-general/auxinclude.h>
#include <ren-general/vector.h>
#include <ren-general/color.h>

// Lua headers/library
extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

class Script
{
	public:
		Script(void);
		Script(lua_State *FromInstance);
		~Script(void);

		// State setup
		bool Do(const String &ScriptName,
			const std::vector<std::pair<String, String> > &Varibles = std::vector<std::pair<String, String> >());

		void RegisterFunction(const String &InLuaName, lua_CFunction Function);
		void RegisterFunction(const String &InLuaName, lua_CFunction Function, void *AssociatedData);
		void EraseFunction(const String &InLuaName);

		// Index tool - creates an index from an address
		static String Index(void *Address, const String &Suffix);

		// Insert function tools
		void Error(const String &Message);
		void *GetAssociatedData(void);

		// Lua data persistence
		void SaveValue(const String &ValueName);
		void RetrieveValue(const String &ValueName);

		// Hook methods
		void CallHook(const String &HookName, int Arguments = 0);

		// Stack manipulation
		void GrabValue(int Position);
		void Pop(void);
		void ClearStack(void);

		// Table methods
		void CreateTable(void);
		void PullElement(const String &Index);
		void PullElement(int Index);
		bool TryElement(const String &Index);

		bool PullNext(void);

		// State query
		int Height(void);

		// Information create functions
		void CreateNil(void);

		// Information get functions
		bool IsTable(void);
		bool IsNil(void);
		String GetType(void); // For debug, mostly

		String GetString(void);
		int GetInteger(void);
		float GetFloat(void);
		bool GetBoolean(void);
		int GetIndex(void); // Like integer, but less one

		Vector GetVector(void);
		FlatVector GetFlatVector(void);
		Color GetColor(void);

		// Information put functions
		void PutString(const String &Puttee);
		void PutFloat(const float &Puttee);
		void PutInteger(const int &Puttee);
		void PutIndex(const int &Puttee);

		// Hacky stuffs
		void PushPointer(void *Pointer);
		void *GetPointer(void);

		// ?
		lua_State *GetState(void);

	private:
		lua_State *Instance;
		bool Owner;
};

#endif
