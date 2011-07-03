#include "script.h"

#include <ren-general/string.h>

// System libraries/headers
#include <iostream>
#include <assert.h>

// Scripting core
Script::Script(void) : Instance(luaL_newstate()), Owner(true)
{
	luaL_openlibs(Instance);
}

Script::Script(lua_State *FromInstance) : Instance(FromInstance), Owner(false)
	{}

Script::~Script(void)
{
	if (Owner) lua_close(Instance);
}

bool Script::Do(const String &ScriptName, const std::vector<std::pair<String, String> > &Variables)
{
	for (std::vector<std::pair<String, String> >::const_iterator CurrentVariable = Variables.begin();
		CurrentVariable != Variables.end(); CurrentVariable++)
	{
		lua_pushstring(Instance, CurrentVariable->second.c_str());
		lua_setglobal(Instance, CurrentVariable->first.c_str());
	}

	lua_getglobal(Instance, "debug");
	lua_getfield(Instance, -1, "traceback");
	lua_remove(Instance, -2);

	int LoadError = luaL_loadfile(Instance, ScriptName.c_str());
	if (LoadError != 0)
	{
		std::cerr << String("Error loading Lua file ") << ScriptName << std::endl;
		std::cerr << String("Error was:\n") << lua_tostring(Instance, -1) << std::endl;
		return false;
	}

	int Result = lua_pcall(Instance, 0, LUA_MULTRET, 1);
	if (Result != 0)
	{
		std::cerr << String("Error running Lua file ") << ScriptName << std::endl;
		std::cerr << String("Error was:\n") << lua_tostring(Instance, -1) << std::endl;;
		return false;
	}

	return true;
}

void Script::RegisterFunction(const String &InLuaName, lua_CFunction Function)
{
	lua_getglobal(Instance, "ext");
	if (lua_isnil(Instance, 1))
	{
		lua_settop(Instance, 0);
		lua_newtable(Instance);
	}

	lua_pushcfunction(Instance, Function);
	lua_setfield(Instance, 1, InLuaName.c_str());
	lua_setglobal(Instance, "ext");
}

void Script::RegisterFunction(const String &InLuaName, lua_CFunction Function, void *AssociatedData)
{
	assert(lua_gettop(Instance) == 0);

	lua_getglobal(Instance, "ext");
	if (lua_isnil(Instance, 1))
	{
		lua_settop(Instance, 0);
		lua_newtable(Instance);
	}

	lua_pushlightuserdata(Instance, AssociatedData);
	lua_pushcclosure(Instance, Function, 1);
	lua_setfield(Instance, 1, InLuaName.c_str());
	lua_setglobal(Instance, "ext");

	assert(lua_gettop(Instance) == 0);
}

void Script::EraseFunction(const String &InLuaName)
{
	assert(lua_gettop(Instance) == 0);

	lua_getglobal(Instance, "ext");
	assert(!lua_isnil(Instance, 1));

	lua_pushnil(Instance);
	lua_setfield(Instance, 1, InLuaName.c_str());
	lua_setglobal(Instance, "ext");

	assert(lua_gettop(Instance) == 0);
}

String Script::Index(void *Address, const String &Suffix)
{
	StringStream Out;
	Out << Address << "_" << Suffix;
	return Out.str();
}

void Script::Error(const String &Message)
{
	lua_pushstring(Instance, Message.c_str());
	lua_error(Instance);
}

void *Script::GetAssociatedData(void)
{
	return lua_touserdata(Instance, lua_upvalueindex(1));
}

void Script::SaveValue(const String &ValueName)
{
	lua_pushstring(Instance, ValueName.c_str());
	lua_pushvalue(Instance, -2);
	lua_settable(Instance, LUA_REGISTRYINDEX);
	lua_remove(Instance, -1);
}

void Script::RetrieveValue(const String &ValueName)
{
	lua_getfield(Instance, LUA_REGISTRYINDEX, ValueName.c_str());

	if (lua_isnil(Instance, -1))
	{
		StringStream Message;
		Message << "The saved value \"" + ValueName + "\" could not be retrieved.";
		Error(Message.str());
	}
}

void Script::CallHook(const String &HookName, int Arguments)
{
	// Get the debug function, for failed calls
	lua_getglobal(Instance, "debug");
	lua_getfield(Instance, -1, "traceback");
	lua_remove(Instance, -2);

	// Grab the hook
	lua_pushstring(Instance, HookName.c_str());
	lua_gettable(Instance, LUA_REGISTRYINDEX);

	// Move all the arguments to the correct stack position
	for (int UnmovedArguments = Arguments; UnmovedArguments > 0; UnmovedArguments--)
	{
		lua_pushvalue(Instance, -2 - Arguments);
		lua_remove(Instance, -3 - Arguments);
	}

	// Get the dbug function position, to remove it afterwards
	int DebugPosition = lua_gettop(Instance) - Arguments - 1;

	// Call the hook
	int Result = lua_pcall(Instance, Arguments, LUA_MULTRET, -2 - Arguments);

	// Check for errors, remove any returned values
	if (Result != 0)
	{
		std::cerr << "Error running Lua hook " << HookName << std::endl;
		std::cerr << "Error was:\n" << lua_tostring(Instance, -1) << std::endl;
	}

	assert(lua_isfunction(Instance, DebugPosition));
	lua_remove(Instance, DebugPosition);
}

void Script::GrabValue(int Position)
{
	lua_pushvalue(Instance, Position);
}

void Script::Pop(void)
{
	lua_remove(Instance, -1);
}

void Script::ClearStack(void)
{
	lua_settop(Instance, 0);
}

void Script::CreateTable(void)
{
	lua_newtable(Instance);
}

void Script::PullElement(const String &Index)
{
	if (!lua_istable(Instance, -1))
		{ Error("Tried to pull an element \"" + Index + "\" from a non-table."); return; }

	lua_getfield(Instance, -1, Index.c_str());

	if (lua_isnil(Instance, -1))
	{
		StringStream Message;
		Message << "The element \"" + Index + "\" could not be pulled, for it did not exist.";
		Error(Message.str());
	}
}

void Script::PullElement(int Index)
{
	if (!lua_istable(Instance, -1))
		{ Error("Tried to pull an element from a non-table."); return; }

	lua_pushinteger(Instance, Index);

	lua_gettable(Instance, -2);

	if (lua_isnil(Instance, -1))
	{
		StringStream Message;
		Message << "The element at \"" << Index << "\" could not be pulled, for it did not exist.";
		Error(Message.str());
	}
}

bool Script::TryElement(const String &Index)
{
	if (!lua_istable(Instance, -1))
		{ Error("Tried to test and pull an element from a non-table."); return false; }

	lua_getfield(Instance, -1, Index.c_str());

	// Check to see if the element existed
	if (lua_isnil(Instance, -1))
	{
		// Clean up the nil
		lua_remove(Instance, -1);
		return false;
	}
	return true;
}

bool Script::PullNext(void)
{
	if (lua_istable(Instance, -1))
		lua_pushnil(Instance);
	else if (!lua_istable(Instance, -2)) return false;

	return lua_next(Instance, -2);
}

int Script::Height(void)
{
	return lua_gettop(Instance);
}

void Script::CreateNil(void)
{
	lua_pushnil(Instance);
}

bool Script::IsTable(void)
{
	return lua_istable(Instance, -1);
}

bool Script::IsNil(void)
{
	return lua_isnil(Instance, -1);
}

String Script::GetType(void)
{
	return lua_typename(Instance, lua_type(Instance, -1));
}

String Script::GetString(void)
{
	if (!lua_isstring(Instance, -1))
		{ Error("Tried to get a string that wasn't a string."); return String(); }

	String Out = lua_tostring(Instance, -1);
	lua_remove(Instance, -1);

	return Out;
}

int Script::GetInteger(void)
{
	if (!lua_isnumber(Instance, -1))
		{ Error("Tried to get an integer that wasn't an integer."); return 0; }

	int Out = lua_tointeger(Instance, -1);
	lua_remove(Instance, -1);

	return Out;
}

float Script::GetFloat(void)
{
	if (!lua_isnumber(Instance, -1))
		{ Error("Tried to get a float that wasn't a float."); return 0.0f; }

	float Out = lua_tonumber(Instance, -1);
	lua_remove(Instance, -1);

	return Out;
}

bool Script::GetBoolean(void)
{
	/*if (!lua_isboolean(Instance, -1))
		{ Error("Tried to get a boolean that wasn't a boolean."); return false; }*/

	bool Out = lua_toboolean(Instance, -1);
	lua_remove(Instance, -1);

	return Out;
}

int Script::GetIndex(void)
{
	if (!lua_isnumber(Instance, -1))
		{ Error("Tried to get an integer that wasn't an index."); return 0; }

	int Out = lua_tointeger(Instance, -1);
	lua_remove(Instance, -1);

	return Out - 1;
}

Vector Script::GetVector(void)
{
	if (!lua_istable(Instance, -1))
		{ Error("Tried to get a vector that wasn't in a table."); return Vector(); }

	Vector Out;

	PullElement(1); Out[0] = GetFloat();
	PullElement(2); Out[1] = GetFloat();
	PullElement(3); Out[2] = GetFloat();

	lua_remove(Instance, -1);

	return Out;
}

FlatVector Script::GetFlatVector(void)
{
	if (!lua_istable(Instance, -1))
		{ Error("Tried to get a flat vector that wasn't in a table."); return FlatVector(); }

	FlatVector Out;

	PullElement(1); Out[0] = GetFloat();
	PullElement(2); Out[1] = GetFloat();

	lua_remove(Instance, -1);

	return Out;
}

Color Script::GetColor(void)
{
	if (!lua_istable(Instance, -1))
		{ Error("Tried to get a color that wasn't in a table."); return Color(); }

	Color Out;

	PullElement(1); Out.Red = GetFloat();
	PullElement(2); Out.Green = GetFloat();
	PullElement(3); Out.Blue = GetFloat();
	PullElement(4); Out.Alpha = GetFloat();

	lua_remove(Instance, -1);

	return Out;
}

void Script::PutString(const String &Puttee)
	{ lua_pushstring(Instance, Puttee.c_str()); }

void Script::PutFloat(const float &Puttee)
	{ lua_pushnumber(Instance, Puttee); }

void Script::PutInteger(const int &Puttee)
	{ lua_pushinteger(Instance, Puttee); }

void Script::PutIndex(const int &Puttee)
	{ lua_pushinteger(Instance, Puttee + 1); }

void Script::PushPointer(void *Pointer)
{
	lua_pushlightuserdata(Instance, Pointer);
}

void *Script::GetPointer(void)
{
	if (!lua_islightuserdata(Instance, -1))
		{ Error("Tried to get a pointer that wasn't a pointer."); return NULL; }
	void *Out = lua_touserdata(Instance, -1);
	lua_remove(Instance, -1);
	return Out;
}

lua_State *Script::GetState(void)
{
	return Instance;
}
