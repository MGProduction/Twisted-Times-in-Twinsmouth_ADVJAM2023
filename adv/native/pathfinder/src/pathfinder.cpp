// myextension.cpp
// Extension lib defines
#define LIB_NAME "PathFinder"
#define MODULE_NAME "pathfinder"

// include the Defold SDK
#include <dmsdk/sdk.h>

#include "corepathfinder.h"

int getVector2(lua_State* L,int index,Vector2*v)
{ 
 int kind=lua_type(L, index);
 if(kind==LUA_TUSERDATA)
  {
    luaL_checktype(L, index, LUA_TUSERDATA);
    Vector2*v2=(Vector2*)(lua_touserdata(L, index));
    v->X = v2->X;
    v->Y = v2->Y;
    //dmLogInfo("[vmath:%d] x=%.2f y=%.2f z=%.2f\n", index,v->X,v->Y,0);
   return 1;
  }
 else
  {   
    //dmLogInfo("[vmath:%d] not a table [%d]!",index,kind);
   return 0;
  }
}

static int FindPath(lua_State* L)
{
    // The number of expected items to be on the Lua stack
    // once this struct goes out of scope
    DM_LUA_STACK_CHECK(L, 1);

    // Check and get parameter string from stack
    Vector2 start,end;
    getVector2(L,1,&start);
    getVector2(L,2,&end);
    
    Vector2 p[2048];
    Vector2 outputpath[512];      
    int     pnt=0;
    int     num_results;

    if(lua_istable(L,3))
    {
     int index=3,idx=0;

     lua_pushnil(L);  // Push a nil value to start the iteration

    // Iterate through the table
     while (lua_next(L, index) != 0)
      {
          // Check the value type        
          int kind=lua_type(L, -2);
          idx++;
          if (kind == LUA_TNUMBER) {
              // Get the key as a string
              lua_getfield(L, -1, "x");
              float x = lua_tonumber(L, -1);
              lua_pop(L, 1);

              lua_getfield(L, -1, "y");
              float y = lua_tonumber(L, -1);
              lua_pop(L, 1);

              lua_getfield(L, -1, "z");
              float z = lua_tonumber(L, -1);
              lua_pop(L, 1);
              
              p[pnt].X=x;
              p[pnt].Y=y;
              pnt++;
          }

          // Pop the value, but keep the key for the next iteration
          lua_pop(L, 1);
      }
    }

     num_results=doFindPath(&p[0],pnt,start,end,&outputpath[0]);
     lua_createtable(L, num_results, 0);

     int newTable = lua_gettop(L),i=0;

     for(i=0;i<num_results;i++)
       {
       lua_createtable(L, 2, 0);
       
       lua_pushstring(L, "x");
       lua_pushnumber(L, outputpath[i].X);
       lua_settable(L, -3);

       lua_pushstring(L, "y");
       lua_pushnumber(L, outputpath[i].Y);
       lua_settable(L, -3);

       lua_rawseti(L, newTable, i + 1);
       }

    return 1;
}


// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"findpath", FindPath},
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

dmExtension::Result AppInitializePathFinder(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result InitializePathFinder(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    printf("Registered %s Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizePathFinder(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizePathFinder(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}


// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

// PathFinder is the C++ symbol that holds all relevant extension data.
// It must match the name field in the `ext.manifest`
DM_DECLARE_EXTENSION(PathFinder, LIB_NAME, AppInitializePathFinder, AppFinalizePathFinder, InitializePathFinder, 0, 0, FinalizePathFinder)