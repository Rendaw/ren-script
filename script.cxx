#include "script.h"

#include <ren-general/string.h>

// System libraries/headers
#include <iostream>
#include <assert.h>

String Script::UniqueIndex(void *Address, const String &Suffix)
{
	StringStream Out;
	Out << Address << "_" << Suffix;
	return Out.str();
}

Script::Script(void) : Instance(luaL_newstate()), Owner(true) {}

Script::Script(lua_State *FromInstance) : Instance(FromInstance), Owner(false) {}

Script::~Script(void)
{
	if (Owner) lua_close(Instance);
}

lua_State *Script::GetState(void)
{
	return Instance;
}

bool Script::Do(const String &ScriptName, bool ShowErrors)
{
	if (ShowErrors)
	{
		lua_getglobal(Instance, "debug");
		lua_getfield(Instance, -1, "traceback");
		lua_remove(Instance, -2);
	}

	int LoadError = luaL_loadfile(Instance, ScriptName.c_str());
	if (LoadError != LUA_OK)
	{
		StandardErrorStream << String("Error loading Lua file ") << ScriptName << "\n" << OutputStream::Flush();
		StandardErrorStream << String("Error was:\n") << lua_tostring(Instance, -1) << "\n" << OutputStream::Flush();
		return false;
	}

	int Result = lua_pcall(Instance, 0, LUA_MULTRET, ShowErrors ? 1 : 0);
	if (Result != LUA_OK)
	{
		if (ShowErrors)
		{
			StandardErrorStream << String("Error running Lua file ") << ScriptName << "\n" << OutputStream::Flush();
			StandardErrorStream << String("Error was:\n") << lua_tostring(Instance, -1) << "\n" << OutputStream::Flush();
		}
		return false;
	}

	return true;
}

int Script::Height(void)
	{ return lua_gettop(Instance); } 

void Script::Pop(void)
	{ lua_remove(Instance, -1); }

void Script::ClearStack(void)
	{ lua_settop(Instance, 0); }

void Script::Lift(int Position)
{
#ifndef NDEBUG
	unsigned int InitialHeight = Height();
#endif	
	lua_pushvalue(Instance, Position); 
	lua_remove(Instance, Position < 0 ? Position - 1 : Position);
#ifndef NDEBUG
	assert(Height() == InitialHeight);
#endif	
}

bool Script::IsNil(void)
	{ return lua_isnil(Instance, -1); }
		
bool Script::IsString(void)
	{ return lua_isstring(Instance, -1); }

bool Script::IsBoolean(void)
	{ return lua_isboolean(Instance, -1); }

bool Script::IsNumber(void)
	{ return lua_isnumber(Instance, -1); }

bool Script::IsTable(void)
	{ return lua_istable(Instance, -1); }

String Script::GetType(void)
	{ return lua_typename(Instance, lua_type(Instance, -1)); }
		
void Script::AssertNil(String const &Message)
	{ if (!IsNil()) throw Error::User(Message); }

void Script::AssertString(String const &Message)
	{ if (!IsString()) throw Error::User(Message); }

void Script::AssertBoolean(String const &Message)
	{ if (!IsBoolean()) throw Error::User(Message); }

void Script::AssertNumber(String const &Message)
	{ if (!IsNumber()) throw Error::User(Message); }
	
void Script::AssertTable(String const &Message)
	{ if (!IsTable()) throw Error::User(Message); }

void Script::AssertFunction(String const &Message)
	{ if (!IsFunction()) throw Error::User(Message); }

void AssertVector(String const &Message)
	{ if (!IsTable()) throw Error::User(Message + "  (Vector was not a table.)"); }

void AssertFlatVector(String const &Message)
	{ if (!IsTable()) throw Error::User(Message + "  (FlatVector was not a table.)"); }

void AssertColor(String const &Message)
	{ if (!IsTable()) throw Error::User(Message + "  (Color was not a table.)"); }

String Script::GetString(void)
{
	assert(IsString());
	String Out = lua_tostring(Instance, -1);
	lua_remove(Instance, -1);
	return Out;
}

int Script::GetInteger(void)
{
	assert(IsNumber());
	int Out = lua_tointeger(Instance, -1);
	lua_remove(Instance, -1);
	return Out;
}

unsigned int Script::GetUnsignedInteger(void)
{
	assert(IsNumber());
	unsigned int Out = lua_tonumber(Instance, -1);
	lua_remove(Instance, -1);
	return Out;
}

int Script::GetIndex(void)
{
	assert(IsNumber());
	int Out = lua_tointeger(Instance, -1);
	lua_remove(Instance, -1);
	return Out - 1;
}

float Script::GetFloat(void)
{
	assert(IsNumber());
	float Out = lua_tonumber(Instance, -1);
	lua_remove(Instance, -1);
	return Out;
}

bool Script::GetBoolean(void)
{
	assert(IsBoolean());
	bool Out = lua_toboolean(Instance, -1);
	lua_remove(Instance, -1);
	return Out;
}

Vector Script::GetVector(void)
{
	assert(IsTable()); // TODO Generic interface
	Vector Out;
	PullElement(1); Out[0] = GetFloat();
	PullElement(2); Out[1] = GetFloat();
	PullElement(3); Out[2] = GetFloat();
	lua_remove(Instance, -1);
	return Out;
}

FlatVector Script::GetFlatVector(void)
{
	assert(IsTable()); // TODO Generic interface
	FlatVector Out;
	PullElement(1); Out[0] = GetFloat();
	PullElement(2); Out[1] = GetFloat();
	lua_remove(Instance, -1);
	return Out;
}

Color Script::GetColor(void)
{
	assert(IsTable()); // TODO Generic interface
	Color Out;
	PullElement(1); Out.Red = GetFloat();
	PullElement(2); Out.Green = GetFloat();
	PullElement(3); Out.Blue = GetFloat();
	PullElement(4); Out.Alpha = GetFloat();
	lua_remove(Instance, -1);
	return Out;
}

void Script::SaveGlobal(const String &GlobalName)
	{ lua_setglobal(Instance, GlobalName.c_str()); }

void Script::SaveInternal(const String &InternalName)
{
	lua_pushstring(Instance, InternalName.c_str());
	Duplicate(-2);
	lua_settable(Instance, LUA_REGISTRYINDEX);
	Pop();
}

void Script::PushNil(void)
	{ lua_pushnil(Instance); }

void Script::PushString(const String &Puttee)
	{ lua_pushstring(Instance, Pushtee.c_str()); }

void Script::PushInteger(const int &Puttee)
	{ lua_pushinteger(Instance, Pushtee); }

void Script::PushIndex(const int &Puttee)
	{ lua_pushinteger(Instance, Pushtee + 1); }

void Script::PushFloat(const float &Puttee)
	{ lua_pushnumber(Instance, Pushtee); }

void Script::PushBoolean(const bool &Data)
	{ lua_pushboolean(Instance, Data); }

void Script::PushFunction(Function NewFunction)
{
	// Store the lambda
	Function *UserData = new Function(NewFunction);
	FunctionStorage.push_back(UserData);

	// Create the lua hook
#ifndef NDEBUG
	unsigned int StartHeight = Height();
#endif

	lua_pushlightuserdata(Instance, UserData);
	lua_pushcclosure(Instance, HandleRegisteredFunction, 1);

#ifndef NDEBUG
	assert(Height() == StartHeight);
#endif
}

void Script::CreateTable(void)
	{ lua_newtable(Instance); }

void Script::PushGlobal(const String &GlobalName)
	{ lua_getglobal(Instance, GlobalName.c_str()); }
		
void Script::PushInternal(const String &InternalName)
	{ lua_getfield(Instance, LUA_REGISTRYINDEX, ValueName.c_str()); }
		
bool Script::IsEmpty(void)
{
	assert(IsTable());
	unsigned int InitialHeight = Height();
	lua_pushnil(Instance);
	if (lua_next(Instance, -2) == 0)
		return true;
	lua_pop(Instance, 2);
#ifndef NDEBUG
	assert(Height() == InitialHeight);
#endif
}

void Script::PullElement(const String &Index)
{
	assert(IsTable());
	lua_getfield(Instance, -1, Index.c_str());
}

void Script::PullElement(int Index)
{
	assert(IsTable());
	lua_pushinteger(Instance, Index);
	lua_gettable(Instance, -2);
}

bool Script::TryElement(const String &Index)
{
	assert(IsTable());
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

bool Script::PullNext(bool PopTableWhenDone)
{
	// If a table is on top, push a nill to start the iteration process
	if (lua_istable(Instance, -1))
		lua_pushnil(Instance);

	// The stack must always have a table underneath the iterator value for iteration
	assert(!lua_istable(Instance, -2));

	// Try to iterate - if the end is reached, pop the table
	bool Success = lua_next(Instance, -2);
	if (!Success && PopTableWhenDone)
		lua_pop(Instance, 1);
	return Success;
}

void Script::Iterate(std::function<bool(Script &State)> Processor)
{
	while (PullNext(false))
	{
#ifndef NDEBUG
		unsigned int InitialHeight = Height();
#endif
		bool Continue = Processor(*this);
#ifndef NDEBUG
		assert(Height() == InitialHeight);
#endif
		if (!Continue)
	       	{
			Pop();
			break;
		}
	}
}
		
void Script::PutElement(const String &Index)
{
	assert(IsTable());
#ifndef NDEBUG
	unsigned int InitialHeight = Height();
#endif
	PushString(Index);
	Lift(-2);
	lua_settable(Instance, -3);
#ifndef NDEBUG
	assert(Height() == InitialHeight - 1);
#endif
}

void Script::PutElement(int Index)
{
	assert(lua_istable(Instance, -2));
#ifndef NDEBUG
	unsigned int InitialHeight = Height();
#endif
	PushInt(Index);
	Lift(-2);
	lua_settable(Instance, -3);
#ifndef NDEBUG
	assert(Height() == InitialHeight - 1);
#endif
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

void Script::PushPointer(void *Pointer)
{
	lua_pushlightuserdata(Instance, Pointer);
}

void *Script::GetPointer(void)
{
	assert(lua_islightuserdata(Instance, -1));
	void *Out = lua_touserdata(Instance, -1);
	lua_remove(Instance, -1);
	return Out;
}

int Script::HandleRegisteredFunction(lua_State *State)
{
	Function &UserFunction = *(Function *)lua_touserdata(State, lua_upvalueindex(1));
	try 
	{
		return UserFunction(Script(State));
	}
	catch (Error::System &Failure)
	{
		lua_pushstring(Instance, ("Encountered an error interacting with the system.  The error message was: " + Failure.Explanation).c_str());
		lua_error(Instance);
	}
	catch (Error::User &Failure)
	{
		lua_pushstring(Instance, Failure.Explanation.c_str());
		lua_error(Instance);
	}
}

