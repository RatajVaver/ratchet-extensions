#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <lauxlib.h>

#define RATCHET_API_VERSION 1

/*
 * Every extension DLL must export exactly this function:
 *   extern "C" __declspec(dllexport) void RatchetRegister(RatchetAPI* api);
 *
 * It will be called once at startup, before any plugins are loaded.
 * Call api->register_ext("MyExt", "1.0.0") before using api->L or any log functions.
 * DO NOT store api->L, it shall not be used outside of Lua callbacks.
 * Keep in mind that introducing any faulty code will crash the server.
 */

typedef struct RatchetAPI RatchetAPI;

struct RatchetAPI {
    int api_version;
    lua_State* L;

    void (*register_ext)(const char* name, const char* version);

    void (*log_info)(const char* msg);

    void (*log_warning)(const char* msg);

    void (*log_error)(const char* msg);
};

#ifdef __cplusplus
}
#endif
