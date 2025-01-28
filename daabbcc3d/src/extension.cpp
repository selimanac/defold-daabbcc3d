// Extension lib defines
#include "dmsdk/dlib/configfile_gen.hpp"
#include "dmsdk/dlib/log.h"
#include "dmsdk/extension/extension_gen.hpp"
#include "dmsdk/gameobject/gameobject.h"
#include "dmsdk/lua/lauxlib.h"

#define LIB_NAME "DAABBCC3D"
#define MODULE_NAME "daabbcc3d"

// include the Defold SDK
#include <daabbcc3d/daabbcc.h>
#include <dmsdk/sdk.h>

////////////////////////////////////////
// Group Operations
////////////////////////////////////////

static int AddGroup(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);

    uint8_t treeBuildType = daabbcc3d::UPDATE_INCREMENTAL;

    if (lua_isnumber(L, 1))
    {
        treeBuildType = luaL_checkint(L, 1);
    }

    uint8_t groupID = daabbcc3d::AddGroup(treeBuildType);
    lua_pushinteger(L, groupID);
    return 1;
}

static int RemoveGroup(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);

    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        daabbcc3d::ErrorAssert("RemoveGroup", groupID);
        return 0;
    }

    daabbcc3d::RemoveGroup(groupID);

    return 0;
}

////////////////////////////////////////
// Query/Ray Results
////////////////////////////////////////

static inline void Result(lua_State* L, uint32_t queryResultSize, dmArray<uint16_t>& queryResult)
{
    lua_createtable(L, queryResultSize, 0);

    int newTable = lua_gettop(L);

    for (int i = 0; i < queryResultSize; i++)
    {
        lua_pushnumber(L, queryResult[i]);
        lua_rawseti(L, newTable, i + 1);
    }

    lua_pushinteger(L, queryResultSize);
}

static inline void SortResult(lua_State* L, uint32_t queryResultSize, dmArray<daabbcc3d::ManifoldResult>& queryResult)
{
    lua_createtable(L, queryResultSize, 0);

    int newTable = lua_gettop(L);

    for (int i = 0; i < queryResultSize; i++)
    {
        lua_createtable(L, 2, 0);
        lua_pushstring(L, "id");
        lua_pushinteger(L, queryResult[i].m_proxyID);
        lua_settable(L, -3);
        lua_pushstring(L, "distance");
        lua_pushnumber(L, queryResult[i].m_distance);
        lua_settable(L, -3);

        lua_rawseti(L, newTable, i + 1);
    }

    lua_pushinteger(L, queryResultSize);
}

static inline void ManifoldResult(lua_State* L, uint32_t queryResultSize, dmArray<daabbcc3d::ManifoldResult>& queryResult)
{
    lua_createtable(L, queryResultSize, 0);

    int newTable = lua_gettop(L);

    for (int i = 0; i < queryResultSize; i++)
    {
        lua_createtable(L, 2, 0);
        lua_pushstring(L, "id");
        lua_pushinteger(L, queryResult[i].m_proxyID);
        lua_settable(L, -3);
        lua_pushstring(L, "distance");
        lua_pushnumber(L, queryResult[i].m_distance);
        lua_settable(L, -3);
        lua_pushstring(L, "depth");
        lua_pushnumber(L, queryResult[i].m_manifold.depth);
        lua_settable(L, -3);
        lua_pushstring(L, "contact_point_x");
        lua_pushnumber(L, queryResult[i].m_manifold.contact_point.x);
        lua_settable(L, -3);
        lua_pushstring(L, "contact_point_y");
        lua_pushnumber(L, queryResult[i].m_manifold.contact_point.y);
        lua_settable(L, -3);
        lua_pushstring(L, "contact_point_z");
        lua_pushnumber(L, queryResult[i].m_manifold.contact_point.z);
        lua_settable(L, -3);
        lua_pushstring(L, "normal_x");
        lua_pushnumber(L, queryResult[i].m_manifold.n.x);
        lua_settable(L, -3);
        lua_pushstring(L, "normal_y");
        lua_pushnumber(L, queryResult[i].m_manifold.n.y);
        lua_settable(L, -3);
        lua_pushstring(L, "normal_z");
        lua_pushnumber(L, queryResult[i].m_manifold.n.z);
        lua_settable(L, -3);

        lua_rawseti(L, newTable, i + 1);
    }

    lua_pushinteger(L, queryResultSize);
}

////////////////////////////////////////
// Query Operations
////////////////////////////////////////

static inline int QueryIDSort(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 2);
    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        daabbcc3d::ErrorAssert("QueryIDSort", groupID);
        return 0;
    }

    int32_t  proxyID = luaL_checkint(L, 2);
    uint64_t maskBits = B2_DEFAULT_MASK_BITS;
    bool     isManifold = false;

    if (lua_isnumber(L, 3))
    {
        maskBits = luaL_checkinteger(L, 3);
    }

    if (lua_isboolean(L, 4))
    {
        isManifold = lua_toboolean(L, 4);
    }

    daabbcc3d::QueryIDSort(proxyID, maskBits, isManifold);

    // Return Result
    uint32_t queryResultSize = 0;

    queryResultSize = daabbcc3d::GetQueryManifoldResultSize();

    if (queryResultSize > 0)
    {
        dmArray<daabbcc3d::ManifoldResult>& queryResult = daabbcc3d::GetQueryManifoldResults();
        if (!isManifold)
        {
            SortResult(L, queryResultSize, queryResult);
        }
        else
        {
            ManifoldResult(L, queryResultSize, queryResult);
        }
    }
    else
    {
        lua_pushnil(L);
        lua_pushinteger(L, queryResultSize);
    }

    return 2;
}

static int QueryAABBSort(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 2);
    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        daabbcc3d::ErrorAssert("QueryAABBSort", groupID);
        return 0;
    }

    dmVMath::Vector3* position = dmScript::CheckVector3(L, 2);
    float             width = luaL_checknumber(L, 3);
    float             height = luaL_checknumber(L, 4);
    float             depth = luaL_checknumber(L, 5);
    uint64_t          maskBits = B2_DEFAULT_MASK_BITS;
    bool              isManifold = false;

    if (lua_isnumber(L, 6))
    {
        maskBits = luaL_checkinteger(L, 6);
    }

    if (lua_isboolean(L, 7))
    {
        isManifold = lua_toboolean(L, 7);
    }

    daabbcc3d::QueryAABBSort(position->getX(), position->getY(), position->getZ(), width, height, depth, maskBits, isManifold);

    // Return Result

    uint32_t queryResultSize = 0;

    queryResultSize = daabbcc3d::GetQueryManifoldResultSize();

    if (queryResultSize > 0)
    {
        dmArray<daabbcc3d::ManifoldResult>& queryResult = daabbcc3d::GetQueryManifoldResults();

        if (!isManifold)
        {
            SortResult(L, queryResultSize, queryResult);
        }
        else
        {
            ManifoldResult(L, queryResultSize, queryResult);
        }
    }
    else
    {
        lua_pushnil(L);
        lua_pushinteger(L, queryResultSize);
    }

    return 2;
}

static int QueryAABB(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 2);
    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        daabbcc3d::ErrorAssert("QueryAABB", groupID);
        return 0;
    }

    dmVMath::Vector3* position = dmScript::CheckVector3(L, 2);
    float             width = luaL_checknumber(L, 3);
    float             height = luaL_checknumber(L, 4);
    float             depth = luaL_checknumber(L, 5);
    uint64_t          maskBits = B2_DEFAULT_MASK_BITS;
    bool              isManifold = false;

    if (lua_isnumber(L, 6))
    {
        maskBits = luaL_checkinteger(L, 6);
    }

    if (lua_isboolean(L, 7))
    {
        isManifold = lua_toboolean(L, 7);
    }

    daabbcc3d::QueryAABB(position->getX(), position->getY(), position->getZ(), width, height, depth, maskBits, isManifold);

    // Return Result

    uint32_t queryResultSize = 0;

    if (!isManifold)
    {
        queryResultSize = daabbcc3d::GetQueryResultSize();
    }
    else
    {
        queryResultSize = daabbcc3d::GetQueryManifoldResultSize();
    }

    if (queryResultSize > 0)
    {
        if (!isManifold)
        {
            dmArray<uint16_t>& queryResult = daabbcc3d::GetQueryResults();
            Result(L, queryResultSize, queryResult);
        }
        else
        {
            dmArray<daabbcc3d::ManifoldResult>& queryResult = daabbcc3d::GetQueryManifoldResults();
            ManifoldResult(L, queryResultSize, queryResult);
        }
    }
    else
    {
        lua_pushnil(L);
        lua_pushinteger(L, queryResultSize);
    }

    return 2;
}

static int QueryID(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 2);

    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        daabbcc3d::ErrorAssert("QueryID", groupID);
        return 0;
    }

    int32_t  proxyID = luaL_checkint(L, 2);
    uint64_t maskBits = B2_DEFAULT_MASK_BITS;
    bool     isManifold = false;

    if (lua_isnumber(L, 3))
    {
        maskBits = luaL_checkinteger(L, 3);
    }

    if (lua_isboolean(L, 4))
    {
        isManifold = lua_toboolean(L, 4);
    }

    daabbcc3d::QueryID(proxyID, maskBits, isManifold);

    // Return Result
    uint32_t queryResultSize = 0;

    if (!isManifold)
    {
        queryResultSize = daabbcc3d::GetQueryResultSize();
    }
    else
    {
        queryResultSize = daabbcc3d::GetQueryManifoldResultSize();
    }

    if (queryResultSize > 0)
    {
        if (!isManifold)
        {
            dmArray<uint16_t>& queryResult = daabbcc3d::GetQueryResults();
            Result(L, queryResultSize, queryResult);
        }
        else
        {
            dmArray<daabbcc3d::ManifoldResult>& queryResult = daabbcc3d::GetQueryManifoldResults();
            ManifoldResult(L, queryResultSize, queryResult);
        }
    }
    else
    {
        lua_pushnil(L);
        lua_pushinteger(L, queryResultSize);
    }

    return 2;
}

////////////////////////////////////////
// Raycast Operations
////////////////////////////////////////

static int RayCast(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 2);

    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        daabbcc3d::ErrorAssert("RayCast", groupID);
        return 0;
    }

    dmVMath::Vector3* start_position = dmScript::CheckVector3(L, 2);
    dmVMath::Vector3* end_position = dmScript::CheckVector3(L, 3);
    uint64_t          maskBits = B2_DEFAULT_MASK_BITS;
    bool              isManifold = false;

    if (lua_isnumber(L, 4))
    {
        maskBits = luaL_checkinteger(L, 4);
    }

    if (lua_isboolean(L, 5))
    {
        isManifold = lua_toboolean(L, 5);
    }

    daabbcc3d::RayCast(start_position->getX(), start_position->getY(), start_position->getZ(), end_position->getX(), end_position->getY(), end_position->getZ(), maskBits, isManifold);

    uint32_t queryResultSize = 0;

    if (!isManifold)
    {
        queryResultSize = daabbcc3d::GetQueryResultSize();
    }
    else
    {
        queryResultSize = daabbcc3d::GetQueryManifoldResultSize();
    }

    if (queryResultSize > 0)
    {
        if (!isManifold)
        {
            dmArray<uint16_t>& queryResult = daabbcc3d::GetQueryResults();
            Result(L, queryResultSize, queryResult);
        }
        else
        {
            dmArray<daabbcc3d::ManifoldResult>& queryResult = daabbcc3d::GetQueryManifoldResults();
            ManifoldResult(L, queryResultSize, queryResult);
        }
    }
    else
    {
        lua_pushnil(L);
        lua_pushinteger(L, queryResultSize);
    }

    return 2;
}

static int RayCastSort(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 2);

    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        daabbcc3d::ErrorAssert("RayCast", groupID);
        return 0;
    }

    dmVMath::Vector3* start_position = dmScript::CheckVector3(L, 2);
    dmVMath::Vector3* end_position = dmScript::CheckVector3(L, 3);

    uint64_t          maskBits = B2_DEFAULT_MASK_BITS;
    bool              isManifold = false;

    if (lua_isnumber(L, 4))
    {
        maskBits = luaL_checkinteger(L, 4);
    }

    if (lua_isboolean(L, 5))
    {
        isManifold = lua_toboolean(L, 5);
    }

    daabbcc3d::RayCastSort(start_position->getX(), start_position->getY(), start_position->getZ(), end_position->getX(), end_position->getY(), end_position->getZ(), maskBits, isManifold);

    uint32_t queryResultSize = 0;

    queryResultSize = daabbcc3d::GetQueryManifoldResultSize();

    if (queryResultSize > 0)
    {
        dmArray<daabbcc3d::ManifoldResult>& queryResult = daabbcc3d::GetQueryManifoldResults();
        if (!isManifold)
        {
            SortResult(L, queryResultSize, queryResult);
        }
        else
        {
            ManifoldResult(L, queryResultSize, queryResult);
        }
    }
    else
    {
        lua_pushnil(L);
        lua_pushinteger(L, queryResultSize);
    }

    return 2;
}

////////////////////////////////////////
// Proxy Operations
////////////////////////////////////////

static int AddProxy(lua_State* L)
{
    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        daabbcc3d::ErrorAssert("AddProxy", groupID);
        return 0;
    }

    dmVMath::Vector3* position = dmScript::CheckVector3(L, 2);
    float             width = luaL_checknumber(L, 3);
    float             height = luaL_checknumber(L, 4);
    float             depth = luaL_checknumber(L, 5);
    uint64_t          categoryBits = B2_DEFAULT_CATEGORY_BITS;

    if (lua_isnumber(L, 6))
    {
        categoryBits = luaL_checkinteger(L, 6);
    }

    int32_t proxyID = daabbcc3d::AddProxy(groupID, position->getX(), position->getY(), position->getZ(), width, height, depth, categoryBits);

    lua_pushinteger(L, proxyID);

    return 1;
}

static int AddGameObject(lua_State* L)
{
    // DM_LUA_STACK_CHECK(L, 1);

    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        dmLogError("AddGameObject: Group ID %i is invalid or already removed!", groupID);
        return 0;
    }

    dmGameObject::HInstance gameobjectInstance = dmScript::CheckGOInstance(L, 2);

    dmVMath::Point3         gameobjectPosition = dmGameObject::GetPosition(gameobjectInstance);
    float                   width = luaL_checknumber(L, 3);
    float                   height = luaL_checknumber(L, 4);
    float                   depth = luaL_checknumber(L, 5);
    uint64_t                categoryBits = B2_DEFAULT_CATEGORY_BITS;
    bool                    getWorldPosition = false;

    if (lua_isnumber(L, 6))
    {
        categoryBits = luaL_checkinteger(L, 6);
    }

    if (lua_isboolean(L, 7))
    {
        getWorldPosition = lua_toboolean(L, 7);
    }

    if (getWorldPosition)
    {
        gameobjectPosition = dmGameObject::GetWorldPosition(gameobjectInstance);
    }

    int32_t proxyID = daabbcc3d::AddProxy(groupID, gameobjectPosition.getX(), gameobjectPosition.getY(), gameobjectPosition.getZ(), width, height, depth, categoryBits);

    daabbcc3d::AddGameObject(groupID, proxyID, gameobjectPosition, width, height, depth, gameobjectInstance, getWorldPosition);

    lua_pushinteger(L, proxyID);

    return 1;
}

static int MoveProxy(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);

    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        daabbcc3d::ErrorAssert("MoveProxy", groupID);
        return 0;
    }

    int32_t           proxyID = luaL_checkint(L, 2);
    dmVMath::Vector3* position = dmScript::CheckVector3(L, 3);
    float             width = luaL_checknumber(L, 4);
    float             height = luaL_checknumber(L, 5);
    float             depth = luaL_checknumber(L, 6);

    daabbcc3d::MoveProxy(proxyID, position->getX(), position->getY(), position->getZ(), width, height, depth);

    return 0;
}

static int UpdateGameobjectSize(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);

    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        daabbcc3d::ErrorAssert("UpdateGameobjectSize", groupID);
        return 0;
    }

    int32_t proxyID = luaL_checkint(L, 2);
    float   width = luaL_checknumber(L, 3);
    float   height = luaL_checknumber(L, 4);
    float   depth = luaL_checknumber(L, 5);

    daabbcc3d::UpdateGameobjectSize(groupID, proxyID, width, height, depth);

    return 0;
}

static int RemoveProxy(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);

    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        daabbcc3d::ErrorAssert("RemoveProxy", groupID);
        return 0;
    }

    int32_t proxyID = luaL_checkint(L, 2);

    daabbcc3d::RemoveProxy(groupID, proxyID);

    return 0;
}

////////////////////////////////////////
// Gameobject Update Operations
////////////////////////////////////////

static int Run(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);

    if (lua_isboolean(L, 1))
    {
        bool state = lua_toboolean(L, 1);
        daabbcc3d::Run(state);
    }
    return 0;
}

static int UpdateFrequency(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);

    int32_t updateFrequency = luaL_checkint(L, 1);
    daabbcc3d::SetUpdateFrequency(updateFrequency);
    return 0;
}

static int Reset(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    daabbcc3d::Reset();
    return 0;
}

static int Rebuild(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    uint8_t groupID = luaL_checkint(L, 1);

    bool    isSet = daabbcc3d::SetTreeGroup(groupID);
    if (!isSet)
    {
        daabbcc3d::ErrorAssert("Rebuild", groupID);
        return 0;
    }

    bool fullBuild = lua_toboolean(L, 2);

    daabbcc3d::Rebuild(groupID, fullBuild);

    return 0;
}

static int RebuildAll(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    bool fullBuild = lua_toboolean(L, 2);
    daabbcc3d::RebuildAll(fullBuild);
    return 0;
}

// clang-format off
// Functions exposed to Lua
static const luaL_reg Module_methods[] = {
 // {"init", Init}, 

  // Group Operations
  {"new_group", AddGroup}, 
  {"remove_group", RemoveGroup}, 

  // Proxy Operations
  {"insert_aabb", AddProxy},
  {"insert_gameobject", AddGameObject},
  {"update_aabb", MoveProxy},
  {"update_gameobject_size", UpdateGameobjectSize},
  {"remove", RemoveProxy},

  // Query Operations
  {"query_aabb", QueryAABB},
  {"query_id", QueryID},
  {"query_aabb_sort", QueryAABBSort},
  {"query_id_sort", QueryIDSort},

    // Raycast Operations
  {"raycast", RayCast},
  {"raycast_sort", RayCastSort},

  // Gameobject Update Operations
  {"run", Run},
  {"update_frequency", UpdateFrequency},

  // Tree Operations
{"rebuild", Rebuild},
{"rebuild_all", RebuildAll},

  // Helpers
{"reset", Reset},

  {0, 0}
};
// clang-format on

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

#define SETCONSTANT(name) \
    lua_pushnumber(L, (lua_Number)daabbcc3d::name); \
    lua_setfield(L, -2, #name);

    SETCONSTANT(UPDATE_INCREMENTAL);
    SETCONSTANT(UPDATE_FULLREBUILD);
    SETCONSTANT(UPDATE_PARTIALREBUILD);
#undef SETCONSTANT

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

static dmExtension::Result AppInitializeDAABBCC3D(dmExtension::AppParams* params)
{
    dmLogInfo("AppInitializeDAABBCC3D");

    uint8_t  max_group_count = dmConfigFile::GetInt(params->m_ConfigFile, "daabbcc.max_group_count", 3);

    uint16_t max_gameobject_count = dmConfigFile::GetInt(params->m_ConfigFile, "daabbcc.max_gameobject_count", 128);

    uint16_t max_query_count = dmConfigFile::GetInt(params->m_ConfigFile, "daabbcc.max_query_result_count", 32);

    int32_t  updateFrequency = dmConfigFile::GetInt(params->m_ConfigFile, "display.update_frequency", 0);

    daabbcc3d::Setup(max_group_count, max_gameobject_count, max_query_count);
    daabbcc3d::SetUpdateFrequency(updateFrequency);

    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeDAABBCC3D(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    dmLogInfo("Registered %s Extension", MODULE_NAME);

    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeDAABBCC3D(dmExtension::Params* params)
{
    daabbcc3d::Reset();
    return dmExtension::RESULT_OK;
}

static dmExtension::Result OnUpdateDAABBCC3D(dmExtension::Params* params)
{
    daabbcc3d::GameObjectUpdate();
    return dmExtension::RESULT_OK;
}

// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update,
// on_event, final)

// DAABBCC is the C++ symbol that holds all relevant extension data.
// It must match the name field in the `ext.manifest`
DM_DECLARE_EXTENSION(DAABBCC3D, LIB_NAME, AppInitializeDAABBCC3D, 0, InitializeDAABBCC3D, OnUpdateDAABBCC3D, 0, FinalizeDAABBCC3D)
