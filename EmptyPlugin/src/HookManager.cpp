#include "HookManager.h"

// Message hook
#if MESSAGEHOOK_ENABLED
	// Callback
PluginAPI::Bool32 __stdcall HookManager::MessageHook(uint32_t ConnectionId, uint8_t MessageId, const void* MessageData, uint32_t MessageSize, intptr_t lParam)
{
	return ((HookManager*)lParam)->MessageHook((PluginAPI::ConnectionInterface)ConnectionId,
		MessageId, MessageData, MessageSize);
}
#endif

// Connection hook
#if CONNECTIONHOOK_ENABLED
	// Callback
PluginAPI::Bool32 __stdcall HookManager::ConnectionHook(uint32_t ConnectionId, uint32_t EventCode, uint64_t PlatformId, intptr_t lParam)
{
	return ((HookManager*)lParam)->ConnectionHook((PluginAPI::ConnectionInterface)ConnectionId,
		(PluginAPI::ConnectionState)EventCode, PlatformId);
}
#endif

// Plugin timer
#if PLUGINTIMER_ENABLED
	// Callback
uint32_t __stdcall HookManager::PluginTimer(uint32_t TimeDelay, intptr_t lParam)
{
	return ((HookManager*)lParam)->PluginTimer(TimeDelay);
}
#endif

// Command hook
#if COMMANDHOOK_ENABLED
	// Callback
PluginAPI::Bool32 __stdcall HookManager::CommandHook(const char* Message, const char* SendingUser, PluginAPI::Bool32 FromTelnet, intptr_t lParam)
{
	return ((HookManager*)lParam)->CommandHook(Message, SendingUser, FromTelnet);
}
#endif

// Event hook
#if EVENTHOOK_ENABLED
	// Callback
PluginAPI::Bool32 __stdcall HookManager::EventHook(intptr_t lParam)
{
	return ((HookManager*)lParam)->EventHook();
}
#endif