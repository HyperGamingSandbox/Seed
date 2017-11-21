#pragma once

#include "..\LuaScript.h"
#include "..\LuaBaseClass.h"
#include "..\LuaClass.h"
#include "..\LuaCheck.h"


LuaClass(Random)

	Random(unsigned short int classId);
	void __init(lua_State *L);
	~Random();

	LuaIMethod(get);
	LuaIMethod(seed);

private:
	long long _seed;
};
