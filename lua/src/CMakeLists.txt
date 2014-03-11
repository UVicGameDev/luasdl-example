CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(LUA_CORE_SRC
    lapi.c lapi.h
    lcode.c lcode.h
    lctype.c lctype.h
    ldebug.c ldebug.h
    ldo.c ldo.h
    ldump.c
    lfunc.c lfunc.h
    lgc.c lgc.h
    llex.c llex.h
    lmem.c lmem.h
    lobject.c lobject.h
    lopcodes.c lopcodes.h
    lparser.c lparser.h
    lstate.c lstate.h
    lstring.c lstring.h
    ltable.c ltable.h
    ltm.c ltm.h
    lundump.c lundump.h
    lvm.c lvm.h
    lzio.c lzio.h
    llimits.h
    ../include/lua.h
    ../include/lua.hpp
    ../include/luaconf.h
    )
    
SET(LUA_LIB_SRC
    lauxlib.c
    lbaselib.c
    lbitlib.c
    lcorolib.c
    ldblib.c
    liolib.c
    lmathlib.c
    loslib.c
    lstrlib.c
    ltablib.c
    loadlib.c
    linit.c
    ../include/lualib.h
    ../include/lauxlib.h
    )

SET(LUA_BASE_SRC ${LUA_CORE_SRC} ${LUA_LIB_SRC})

SET(LUA_SOURCES ${LUA_BASE_SRC})

IF(MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)

ADD_LIBRARY(lua STATIC ${LUA_SOURCES})
