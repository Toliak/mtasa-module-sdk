# Argument types

- [x] Boolean
- [x] Number (double)
- [x] Integer
- [x] String
- [x] Userdata
- [x] Lightuserdata
- [x] Nil
- [ ] Table (only from C++ to Lua)

## How to add type from Lua

### [LuaArgument.h](LuaArgument.h)

* Specify constructor
* Add it to copy method
* Add it to destroy method
* Specify ``LUA_VM_ARGUMENT_GET_FUNCTION``

### [LuaVmExtended.h](LuaVmExtended.h)

* Add it to parseArgument methods
* Add it to pushArgument method

### [Exception.h](Exception.h)

* Add it to ``STRING_TYPE``