#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID)

#define EXTENSION_NAME AdInfoExt
#define LIB_NAME "AdInfoExt"
#define MODULE_NAME "adinfo"

#define DLIB_LOG_DOMAIN LIB_NAME
#include <dmsdk/sdk.h>
#include <stdlib.h>
#include "adinfo.h"




ADInfoData g_ADInfoData;

void ADInfo_QueueAdId(const char* adId) {
    g_ADInfoData.m_AdId = strdup(adId);
}

static int AdInfo_UpdateConversionValue(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 0);

    lua_Number value = luaL_checknumber(L, 1);
    AdInfo_PlatformUpdateConversionValue(value);
    return 0;
}

static int AdInfo_Get(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);

    if(g_ADInfoData.m_callback != 0)
    {
        dmLogError("Callback already in progress");
        lua_pushboolean(L, 0);
        return 1;
    }

    luaL_checktype(L, 1, LUA_TFUNCTION);
    g_ADInfoData.m_callback = dmScript::CreateCallback(L, 1);

    if (!g_ADInfoData.m_AdId)
    {
        ADInfo_PlatformGetAdId();
    }

    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_reg Module_methods[] =
{
    {"get", AdInfo_Get},
    {"update_conversion_value", AdInfo_UpdateConversionValue},
    {0,0}
};

static void LuaInit(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    int top = lua_gettop(L);
    luaL_register(L, MODULE_NAME, Module_methods);
    lua_pop(L, 1);
}

static dmExtension::Result UpdateAdInfo(dmExtension::Params* params)
{
    if(g_ADInfoData.m_callback != 0 && g_ADInfoData.m_AdId != 0)
    {
        lua_State* L = dmScript::GetCallbackLuaContext(g_ADInfoData.m_callback);
        DM_LUA_STACK_CHECK(L, 0);

        if (dmScript::SetupCallback(g_ADInfoData.m_callback))
        {
            lua_newtable(L);
            lua_pushstring(L, g_ADInfoData.m_AdId);
            lua_setfield(L, -2, "ad_ident");
            lua_pushboolean(L, ADInfo_PlatformIsAdvertisingTrackingEnabled());
            lua_setfield(L, -2, "ad_tracking_enabled");

            dmScript::PCall(L, 2, 0);
            dmScript::TeardownCallback(g_ADInfoData.m_callback);
        }
        dmScript::DestroyCallback(g_ADInfoData.m_callback);
        g_ADInfoData.m_callback = 0;
    }
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeAdInfo(dmExtension::Params* params)
{
    LuaInit(params->m_L);
    ADInfo_PlatformInitAdInfoExt(params);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeAdInfo(dmExtension::Params* params)
{
    free(g_ADInfoData.m_AdId);
    g_ADInfoData.m_AdId = 0;
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, 0, 0, InitializeAdInfo, UpdateAdInfo, 0, FinalizeAdInfo)

#endif // platforms
