#include "Plugin.h"

PluginAPI::Bool32 __stdcall BinaryChatAPIManager::EnumChannelHook(const char* UniqueName, const char* Statstring, uint32_t Flags, uint32_t Ping, PluginAPI::Bool32 Invisible, uint32_t AccountNumber, uint32_t RegAuthority, intptr_t lParam)
{
	return reinterpret_cast<HookManager*>(lParam)->EnumChannelHook(UniqueName, Statstring, Flags, Ping, Invisible, AccountNumber, RegAuthority);
}

#if HIGHEST_REVISION >= 4
PluginAPI::Bool32 __stdcall BinaryChatAPIManager::EnumUserFiltersHook(const char* Filter, intptr_t lParam)
{
	return reinterpret_cast<HookManager*>(lParam)->EnumUserFiltersHook(Filter);
}

PluginAPI::Bool32 __stdcall BinaryChatAPIManager::EnumMessageFiltersHook(const char* Filter, intptr_t lParam)
{
	return reinterpret_cast<HookManager*>(lParam)->EnumMessageFiltersHook(Filter);
}

PluginAPI::Bool32 __stdcall BinaryChatAPIManager::EnumLoadedPluginsHook(PluginAPI::ModuleHandle PluginInst, intptr_t lParam)
{
	return reinterpret_cast<HookManager*>(lParam)->EnumLoadedPluginsHook(PluginInst);
}
#endif