#ifndef _PLUGIN_H_INCLUDED
#define _PLUGIN_H_INCLUDED
#pragma once

#include "HookManager.h"

class Plugin : public HookManager
{
protected:
	// Message hook
#if MESSAGEHOOK_ENABLED
	PluginAPI::Bool32 MessageHook(PluginAPI::ConnectionInterface Connection, uint8_t MessageId, const void* MessageData, uint32_t MessageSize);
#endif

	// Connection hook
#if CONNECTIONHOOK_ENABLED
	PluginAPI::Bool32 ConnectionHook(PluginAPI::ConnectionInterface Connection, PluginAPI::ConnectionState EventCode, uint64_t PlatformId);
#endif

	// Plugin timer
#if PLUGINTIMER_ENABLED
	uint32_t PluginTimer(uint32_t TimeDelay);
#endif

	// Command hook
#if COMMANDHOOK_ENABLED
	PluginAPI::Bool32 CommandHook(const char* Message, const char* SendingUser, PluginAPI::Bool32 FromTelnet);
#endif

	// Event hook
#if EVENTHOOK_ENABLED
	PluginAPI::Bool32 EventHook();
#endif

	// Enum channel callback
	PluginAPI::Bool32 EnumChannelHook(const char* UniqueName, const char* Statstring, uint32_t Flags, uint32_t Ping, PluginAPI::Bool32 Invisible, uint32_t AccountNumber, uint32_t RegAuthority);

#if HIGHEST_REVISION >= 4
	// Enum filter callbacks and enum plugins callback
	PluginAPI::Bool32 EnumUserFiltersHook(const char* Filter);
	PluginAPI::Bool32 EnumMessageFiltersHook(const char* Filter);
	PluginAPI::Bool32 EnumLoadedPluginsHook(PluginAPI::ModuleHandle PluginInst);
#endif

public:
	// Instead of constructor/destructor, use these:
	PluginAPI::Bool32 Initialize(); // Return FALSE to abort (if this returns FALSE, Terminate isn't called)
	void Terminate(); // Do any termination here
};

#endif // _PLUGIN_H_INCLUDED