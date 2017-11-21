#pragma once

#include "..\LuaScript.h"
#include "..\LuaBaseClass.h"
#include "..\LuaClass.h"
#include "..\LuaCheck.h"

#define LS_NONE				0
#define LS_BOTTOM			1

#define SE_AFTERADD			0
#define SE_AFTERDEL			1


typedef unsigned short int PIndex;

LuaClass(OrderedArray)

	OrderedArray();
	~OrderedArray();
	void __init(lua_State *L);

	LuaIMethod(count);
	LuaIMethod(push);
	LuaIMethod(get);
	LuaIMethod(sort);
	LuaIMethod(setSort);
	LuaIMethod(del);
	
	LuaIMethod(draw);
	LuaIMethod(isPointOn);

private:

	unsigned short int _size, _count, _exists;
	unsigned short int _dcount, _sortType, _id;

	PLuaBaseClass *_data;
	PIndex *_deleted;
};