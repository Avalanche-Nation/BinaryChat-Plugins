#include "Plugin.h"

// Message hook
#if MESSAGEHOOK_ENABLED
PluginAPI::Bool32 Plugin::MessageHook(PluginAPI::ConnectionInterface Connection, uint8_t MessageId, const void* MessageData, uint32_t MessageSize)
{
	// Add code here
	return PluginAPI::False;
}
#endif

// Connection hook
#if CONNECTIONHOOK_ENABLED
PluginAPI::Bool32 Plugin::ConnectionHook(PluginAPI::ConnectionInterface Connection, PluginAPI::ConnectionState EventCode, uint64_t PlatformId)
{
	// Add code here
	return PluginAPI::False;
}
#endif

// Plugin timer
#if PLUGINTIMER_ENABLED
uint32_t Plugin::PluginTimer(uint32_t TimeDelay)
{
	// Add code here
	return TimeDelay;
}
#endif

// Command hook
#if COMMANDHOOK_ENABLED
PluginAPI::Bool32 Plugin::CommandHook(const char* Message, const char* SendingUser, PluginAPI::Bool32 FromTelnet)
{
	// Add code here
	return PluginAPI::False;
}
#endif

// Event hook
#if EVENTHOOK_ENABLED
PluginAPI::Bool32 Plugin::EventHook()
{
	// Add code here
	return PluginAPI::False;
}
#endif

// Enum channel callback
PluginAPI::Bool32 Plugin::EnumChannelHook(const char* UniqueName, const char* Statstring, uint32_t Flags, uint32_t Ping, PluginAPI::Bool32 Invisible, uint32_t AccountNumber, uint32_t RegAuthority)
{
	// Add code here
	return PluginAPI::True;
}

#if HIGHEST_REVISION >= 4
// Enum filter callbacks
PluginAPI::Bool32 Plugin::EnumUserFiltersHook(const char* Filter)
{
	// Add code here
	return PluginAPI::True;
}

PluginAPI::Bool32 Plugin::EnumMessageFiltersHook(const char* Filter)
{
	// Add code here
	return PluginAPI::True;
}

// Enum plugins callback
PluginAPI::Bool32 Plugin::EnumLoadedPluginsHook(PluginAPI::ModuleHandle PluginInst)
{
	// Add code here
	return PluginAPI::True;
}
#endif

PluginAPI::Bool32 Plugin::Initialize()
{
	// Initialization code here
	return PluginAPI::True;
}

void Plugin::Terminate()
{
	// Termination code here
}