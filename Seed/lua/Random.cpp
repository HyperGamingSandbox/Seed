#include "Random.h"


Random::Random(unsigned short int classId) : LuaBaseClass(classId) {
	setmt();
}


void Random::__init(lua_State *L) {

	CHECK_ARG(1, integer);
	_seed = lua_tointeger(L, 1);
}

Random::~Random() {
}

LuaEMethod(Random, get) {
	CHECK_ARG(1, integer);
	auto r = lua_tointeger(L, 1);

	auto p = _seed;

	_seed = (3037 * _seed + 1877) / 7 * 3559 + 3571;
	// s ^= p >> 17;
	// s ^= p >> 23;
	// s ^= p >> 48;

	_seed ^= p >> 32;

	if (_seed < 0) _seed *= -1;

	lua_pushinteger(L, (_seed >> 25) % r);

	return 1;
}

LuaEMethod(Random, seed) {
	lua_pushinteger(L, _seed);
	return 1;
}


LuaMethods(Random) = {
	LuaMethodDesc(Random, get),
	LuaMethodDesc(Random, seed),
	{ 0,0 }
};

LuaMetaMethods(Random) = {
	{ 0,0 }
};

void module_random(lua_State *L) {
	LuaClass<Random>::Register(L);
}