#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID)

#define EXTENSION_NAME AdInfoExt
#define LIB_NAME "AdInfoExt"
#define MODULE_NAME "adinfo"

#define DLIB_LOG_DOMAIN LIB_NAME
#include <dmsdk/sdk.h>
#include "private_adinfo.h"

static int GetAdInfo(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);
    const char* id = ADInfo_GetAdId();
    if (id) 
    {
        lua_newtable(L);
        lua_pushliteral(L, "ad_ident");
        lua_pushstring(L, id);
        lua_rawset(L, -3);
        lua_pushliteral(L, "ad_tracking_enabled");
        lua_pushboolean(L, ADInfo_IsAdvertisingTrackingEnabled());
        lua_rawset(L, -3);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}
static const luaL_reg Module_methods[] =
{
    {"get", GetAdInfo},
    {0,0}
};

static void LuaInit(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    ADInfo_InitAdInfoExt();
    int top = lua_gettop(L);
    luaL_register(L, MODULE_NAME, Module_methods);
    lua_pop(L, 1);
}

static dmExtension::Result InitializeAdInfo(dmExtension::Params* params)
{
    LuaInit(params->m_L);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeAdInfo(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, 0, 0, InitializeAdInfo, 0, 0, FinalizeAdInfo)

#endif // platforms
