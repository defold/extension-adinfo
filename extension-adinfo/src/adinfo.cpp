#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID)

#define EXTENSION_NAME AdInfoExt
#define LIB_NAME "AdInfoExt"
#define MODULE_NAME "adinfo"

#define DLIB_LOG_DOMAIN LIB_NAME
#include <dmsdk/sdk.h>
#include "adinfo.h"




ADInfoData g_ADInfoData;

void ADInfo_QueueAdId(const char* adId) {
    g_ADInfoData.m_AdId = adId;
}

static int GetAdInfo(lua_State* L) {
    DM_LUA_STACK_CHECK(L, 1);

    if(g_ADInfoData.m_callback != 0)
    {
        dmLogError("Callback already in progress");
        lua_pushboolean(L, 0);
        return 1;
    }

    luaL_checktype(L, 1, LUA_TFUNCTION);
    if (g_ADInfoData.m_callback) {
        dmScript::DestroyCallback(g_ADInfoData.m_callback);
    }
    g_ADInfoData.m_callback = dmScript::CreateCallback(L, 1);

    if (!g_ADInfoData.m_AdId)
    {
        ADInfo_GetAdId();
    }

    lua_pushboolean(L, 1);
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
            lua_pushliteral(L, "ad_ident");
            lua_pushstring(L, g_ADInfoData.m_AdId);
            lua_rawset(L, -3);
            lua_pushliteral(L, "ad_tracking_enabled");
            lua_pushboolean(L, ADInfo_IsAdvertisingTrackingEnabled());
            lua_rawset(L, -3);

            if (lua_pcall(L, 2, 0, 0) != 0)
            {
                dmLogError("Error running AdId callback: %s", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
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
    ADInfo_InitAdInfoExt(params);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeAdInfo(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, 0, 0, InitializeAdInfo, UpdateAdInfo, 0, FinalizeAdInfo)

#endif // platforms
