[![Build Status](https://travis-ci.com/Toliak/mtasa-module-sdk.svg?branch=master)](https://travis-ci.com/Toliak/mtasa-module-sdk)
[![pipeline status](https://gitlab.toliak.ru/mtasa/module-sdk/badges/master/pipeline.svg)](https://gitlab.toliak.ru/mtasa/module-sdk)
![Code size](https://img.shields.io/github/languages/code-size/Toliak/mtasa-module-sdk.svg)
![GitHub Repo size](https://img.shields.io/github/repo-size/Toliak/mtasa-module-sdk.svg)

# MTASA Module SDK

Extended OOP functionality for interaction between Lua and C++

## Guide

### Get and push arguments (arguments are lua-objects)

```cpp
lua_State *luaVm;                     // original lua vm pointer

LuaVmExtended lua{LuaVmExtended};     // create lua extended wrapper

std::vector<LuaArgument> args =
                         lua.getArguments();  // get all passed arguments

std::vector<LuaArguments> returnArgs{
    {-1},
    {"Error"},
};                                         // create argument vector

int amount = 
    lua.pushArguments(returnArgs.cbegin(), 
                      returnArgs.cend());  // accepts any iterators

// amount contains number of pushed arguments
```

### Call function

```cpp
std::vector<LuaArgument> returnArgs = lua.call(
    "functionName",         // Function name
    {{"arg1"}, {"arg2"}},   // Pass args
    3                       // Return args amount
);
```

## Tests

Tests require docker-compose

```bash
./compose-test.sh
```