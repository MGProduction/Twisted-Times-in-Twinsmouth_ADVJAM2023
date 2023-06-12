// myextension.cpp
// Extension lib defines
#define LIB_NAME "TreePad"
#define MODULE_NAME "treepad"

// include the Defold SDK
#include <dmsdk/sdk.h>

int treepad2txt(void*v,const char*p);

static int Decode(lua_State* L)
{
    // The number of expected items to be on the Lua stack
    // once this struct goes out of scope
    const unsigned char *str;

    DM_LUA_STACK_CHECK(L, 1);

    // Check and get parameter string from stack

    /* requires a single string type argument */
    luaL_argcheck (L, lua_isstring (L, 1), 1, "must provide a string argument");    
    str = (const unsigned char *) lua_tostring (L, 1);

    lua_newtable( L );
   
    treepad2txt(L,(const char*)str);

    return 1;
}


// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"decode", Decode},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

dmExtension::Result AppInitializeTreePad(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeTreePad(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    printf("Registered %s Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeTreePad(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeTreePad(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}


// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

// TreePad is the C++ symbol that holds all relevant extension data.
// It must match the name field in the `ext.manifest`
DM_DECLARE_EXTENSION(TreePad, LIB_NAME, AppInitializeTreePad, AppFinalizeTreePad, InitializeTreePad, 0, 0, FinalizeTreePad)