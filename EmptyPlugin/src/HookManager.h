#ifndef _HOOKMANAGER_H_INCLUDED
#define _HOOKMANAGER_H_INCLUDED
#pragma once

#include "BinaryChatAPIManager.h"

class HookManager
{
	friend PluginAPI::Bool32 __stdcall PluginMain(PluginAPI::PluginInformation* Information);
	// PluginMain may read/set the private/protected variables/proc addresses

	friend class BinaryChatAPIManager;
	// BinaryChatAPIManager may call protected virtual functions

private:
	// Message hook
#if MESSAGEHOOK_ENABLED
	static PluginAPI::Bool32 __stdcall MessageHook(uint32_t ConnectionId, uint8_t MessageId, const void* MessageData, uint32_t MessageSize, intptr_t lParam);
#endif

	// Connection hook
#if CONNECTIONHOOK_ENABLED
	static PluginAPI::Bool32 __stdcall ConnectionHook(uint32_t ConnectionId, uint32_t EventCode, uint64_t PlatformId, intptr_t lParam);
#endif

	// Plugin timer
#if PLUGINTIMER_ENABLED
	static uint32_t __stdcall PluginTimer(uint32_t TimeDelay, intptr_t lParam);
#endif

	// Command hook
#if COMMANDHOOK_ENABLED
	static PluginAPI::Bool32 __stdcall CommandHook(const char* Message, const char* SendingUser, PluginAPI::Bool32 FromTelnet, intptr_t lParam);
#endif

	// Event hook
#if EVENTHOOK_ENABLED
	static PluginAPI::Bool32 __stdcall EventHook(intptr_t lParam);
#endif

protected:
	PluginAPI::ModuleHandle m_PluginInstance;

	// Message hook (virtual function)
#if MESSAGEHOOK_ENABLED
	virtual PluginAPI::Bool32 MessageHook(PluginAPI::ConnectionInterface Connection, uint8_t MessageId, const void* MessageData, uint32_t MessageSize) = 0;
#endif

	// Connection hook (virtual function)
#if CONNECTIONHOOK_ENABLED
	virtual PluginAPI::Bool32 ConnectionHook(PluginAPI::ConnectionInterface Connection, PluginAPI::ConnectionState EventCode, uint64_t PlatformId) = 0;
#endif

	// Plugin timer (virtual function)
#if PLUGINTIMER_ENABLED
	virtual uint32_t PluginTimer(uint32_t TimeDelay) = 0;
#endif

	// Command hook (virtual function)
#if COMMANDHOOK_ENABLED
	virtual PluginAPI::Bool32 CommandHook(const char* Message, const char* SendingUser, PluginAPI::Bool32 FromTelnet) = 0;
#endif

	// Event hook (virtual function) + event handle
#if EVENTHOOK_ENABLED
	HANDLE m_Event;

	virtual PluginAPI::Bool32 EventHook() = 0;
#endif

	// Enum channel callback
	virtual PluginAPI::Bool32 EnumChannelHook(const char* UniqueName, const char* Statstring, uint32_t Flags, uint32_t Ping, PluginAPI::Bool32 Invisible, uint32_t AccountNumber, uint32_t RegAuthority) = 0;

#if HIGHEST_REVISION >= 4
	// Enum filter callbacks and enum plugins callback
	virtual PluginAPI::Bool32 EnumUserFiltersHook(const char* Filter) = 0;
	virtual PluginAPI::Bool32 EnumMessageFiltersHook(const char* Filter) = 0;
	virtual PluginAPI::Bool32 EnumLoadedPluginsHook(PluginAPI::ModuleHandle PluginInst) = 0;
#endif
};

#endif // _HOOKMANAGER_H_INCLUDED