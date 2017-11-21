#include "OrderedArray.h"
#include "DD\DrawMachine.h"
#include "RenderObject.h"

OrderedArray::OrderedArray() : LuaBaseClass() {
	_count = _exists = _dcount = 0;
	_size = 8;
	_data = new PLuaBaseClass[_size];
	_deleted = new PIndex[_size];
	_sortType = 0;
	setmt();
}

OrderedArray::~OrderedArray() {

	auto p = _data;
	for (int i = 0; i < _count; i++, p++) {
		if (*p == NULL) {
			continue;
		}
		auto bc = *p;		
		auto share = bc->share(-1);
		if (share == 0) {
			delete bc;
		}
	}
	delete _data;
	delete _deleted;
}

void OrderedArray::__init(lua_State *L) {
	CHECK_ARG(1, integer);
	_id = (unsigned short int)lua_tointeger(L, 1);
}

LuaEMethod(OrderedArray, count) {
	lua_pushinteger(L, _count);
	return 1;
}

LuaEMethod(OrderedArray, get) {
	CHECK_ARG(1, integer);
	auto index = lua_tointeger(L, 1);

	_lock->lock_shared();

	if (index >= _count) {
		lua_pushnil(L);
		_lock->unlock_shared();
	}
	else {
		auto bc = _data[index];
		if (bc) {
			bc->share(1);
			_lock->unlock_shared();
			UserData *ud = (UserData *)lua_newuserdata(L, sizeof(UserData));
			ud->type = UDT_Class;
			ud->data = bc;
			bc->registerInNewState(L);
		}
		else {
			lua_pushnil(L);
			_lock->unlock_shared();
		}
	}

	
	return 1;
}

LuaEMethod(OrderedArray, push) {
	_lock->lock();

	auto last = lua_gettop(L);

	if (_dcount == 0) {
		auto _new_size = _count + last;
		if (_new_size > _size) {
			_new_size = _size << 1;

			auto _ndata = new PLuaBaseClass[_new_size];
			memcpy(_ndata, _data, _size * sizeof(PLuaBaseClass));
			delete _data;
			_data = _ndata;

			auto _ndeleted = new PIndex[_new_size];
			if (_deleted) {
				memcpy(_ndeleted, _deleted, _dcount * sizeof(PIndex));
				delete _deleted;
			}
			
			_deleted = _ndeleted;

			_size = _new_size;
		}
	}

	UserData *ud;
	LuaBaseClass *bc;
	RenderObject *ro;

	for (int index = 1; index <= last; index++) {
		auto tp = lua_type(L, index);
		switch (tp) {
		case LUA_TUSERDATA:
			ud = (UserData *)lua_touserdata(L, index);
			switch (ud->type) {
			case UDT_Class:
				bc = static_cast<LuaBaseClass *>(ud->data);
				bc->share(1);
				_data[_count] = bc;
				ro = dynamic_cast<RenderObject *>(bc);
				ro->_lid = _id;
				ro->_lindex = _count;
				_count++;
				_exists++;
				break;
			default:
				_lock->unlock();
				lua_pushfstring(L, "OrderedArray unknown UserData type %d", ud->type);
				lua_error(L);
			}
			break;
		default:
			_lock->unlock();
			lua_pushfstring(L, "OrderedArray unsupport type %s", lua_typename(L, tp));
			lua_error(L);
		}
	}			
	
	_lock->unlock();
	return 0;
}

LuaEMethod(OrderedArray, draw) {

	CHECK_ARGCLASS(1, DDDrawMachine, _drawMachine);

	_lock->lock_shared();
	// _lock->lock();


	auto p = _data;
	for (int i = 0; i < _count; i++, p++) {
		auto bc = *p;
		if (bc) {
			RenderObject *ro = dynamic_cast<RenderObject *>(bc);
			// RenderObject *ro = (RenderObject *)bc;
			ro->draw(_drawMachine);
		}
	}

	_lock->unlock_shared();
	// _lock->unlock();

	return 0;
}

#include <algorithm>
#include "Array.h"

LuaEMethod(OrderedArray, setSort) {
	CHECK_ARG(1, integer);
	_sortType = (int)lua_tointeger(L, 1);
	return 0;
}

typedef bool(*SortFunc)(Array *a, Array *b);

SortFunc sortFuncs[] = {
	[](Array *a, Array *b) {

		if (a == NULL) return false;
		if (b == NULL) return true;
		return false;

	},
	[](Array *a, Array *b) {

		if (a == NULL) return false;
		if (b == NULL) {
			return true;
		}

		bool r = b->getData()[ROF_Y].i > a->getData()[ROF_Y].i;

		return r;

	}
};

LuaEMethod(OrderedArray, sort) {

	CHECK_ARG(1, integer);
	auto sortEvent = lua_tointeger(L, 1);

	if (_sortType == LS_NONE && sortEvent == SE_AFTERADD) {
		return 0;
	}

	_lock->lock();

	auto p = _data;
	for (int i = 0; i < _count; i++, p++) {
		auto bc = *p;
		if (bc) {
			bc->_lock->lock_shared();
		}
	}

	std::sort((Array **)_data, (Array **)&_data[_count], sortFuncs[_sortType]);

	_count = _exists;

	p = _data;
	for (int i = 0; i < _count; i++, p++) {
		auto bc = *p;
		// if (bc) {
			bc->_lock->unlock_shared();
		// }
	}

	_lock->unlock();
	return 0;
}

LuaEMethod(OrderedArray, del) {

	_lock->lock();

	UserData *ud = (UserData *)lua_touserdata(L, 1);
	auto bc = static_cast<LuaBaseClass *>(ud->data);
	RenderObject *ro = dynamic_cast<RenderObject *>(bc);
	_data[ro->_lindex] = 0;
	_exists--;
	auto r = bc->share(-1);
	if (r == 0) {
		delete bc;
	}

	/*
	_deleted[_dcount] = ro->_lindex;
	_dcount++;
	*/
	_lock->unlock();

	return 0;
}

LuaEMethod(OrderedArray, isPointOn) {
	
	CHECK_ARG(1, integer);
	CHECK_ARG(2, integer);

	_lock->lock_shared();

	auto x = lua_tointeger(L, 1);
	auto y = lua_tointeger(L, 2);

	auto p = &_data[_count - 1];
	auto c = _count;
	while (c--) {
		auto bc = *p;
		if(bc) {
			RenderObject *ro = dynamic_cast<RenderObject *>(bc);

			auto prop = ro->getData();

			if (!prop[ROF_SKIPHOVER].b && ro->__isPointOn(x, y)) {

				
				bc->share(1);
				_lock->unlock_shared();

				// OutputDebugStringA("isPointOn 1\n");
				UserData *ud = (UserData *)lua_newuserdata(L, sizeof(UserData));
				ud->type = UDT_Class;
				ud->data = bc;
				
				// OutputDebugStringA("isPointOn 2\n");

				bc->registerInNewState(L);
				
				return 1;
			}
		}
		p--;
	}
	lua_pushnil(L);
	_lock->unlock_shared();
	return 1;
}

LuaMethods(OrderedArray) = {
	LuaMethodDesc(OrderedArray, count),
	LuaMethodDesc(OrderedArray, get),
	LuaMethodDesc(OrderedArray, push),
	LuaMethodDesc(OrderedArray, sort),
	LuaMethodDesc(OrderedArray, setSort),
	LuaMethodDesc(OrderedArray, draw),
	LuaMethodDesc(OrderedArray, isPointOn),
	LuaMethodDesc(OrderedArray, del),
	{ 0,0 }
};

LuaMetaMethods(OrderedArray) = {
	// LuaMethodDesc(Array, __tostring),
	// LuaMethodDesc(Font, __newindex),
	{ 0,0 }
};


void module_ordered_array(lua_State *L) {
	LuaClass<OrderedArray>::Register(L);
}
