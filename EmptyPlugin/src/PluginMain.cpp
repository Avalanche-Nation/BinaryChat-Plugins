#include "Plugin.h"

// Global objects
BinaryChatAPIManager API;
Plugin PluginInstance;

static PluginAPI::Bool32 Initialized;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD Reason, LPVOID)
{
	switch (Reason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hInstance);
		return TRUE;

	case DLL_PROCESS_DETACH:
		if (Initialized)
			PluginInstance.Terminate();

		return TRUE;

	default:
		__assume(0); // No other cases exist thanks to DisableThreadLibraryCalls
	}
}

PluginAPI::Bool32 __stdcall PluginMain(PluginAPI::PluginInformation* Information)
{
	// PluginInformation pointers for newer revisions (even if they don't exist)
	PluginAPI::PluginInformation2*& Information2 = (PluginAPI::PluginInformation2*&)Information;
	PluginAPI::PluginInformation3*& Information3 = (PluginAPI::PluginInformation3*&)Information;
	PluginAPI::PluginInformation4*& Information4 = (PluginAPI::PluginInformation4*&)Information;

	ZeroMemory(&API, sizeof(API));

	// Check which revision number this is
	uint32_t Revision;

#if HIGHEST_REVISION >= 4
	if (Information->Size >= sizeof(PluginAPI::PluginInformation4))
		Revision = 4;
	else
#endif
#if HIGHEST_REVISION >= 3
		if (Information->Size >= sizeof(PluginAPI::PluginInformation3))
			Revision = 3;
		else
#endif
#if HIGHEST_REVISION >= 2
			if (Information->Size >= sizeof(PluginAPI::PluginInformation2))
				Revision = 2;
			else
#endif
				Revision = 1;

	// Make sure this is higher than the lowest allowed revision
	if (Revision < LOWEST_REVISION)
		return FALSE;

	API.m_Revision = Revision;

	// Can safely assume all PluginInformation (revision 1) members exist,
	// but must check the Size parameter before looking at newer revisions

	// REVISION 1
	API.m_ClearMessage = Information->NetClearMessage;
	API.m_IsMessageSet = Information->NetIsMessageSet;
	API.m_SetMessage = Information->NetSetMessage;
	API.m_QueueMessage = Information->NetQueueMessage;
	API.m_QueueChatMessage = Information->NetQueueChatMessage;
	API.m_CloseConnection = Information->NetCloseConnection;
	API.m_SendConnectionMessage = Information->NetSendConnectionMessage;
	API.m_QueryStateInformation = Information->SysQueryStateInformation;
	API.m_QueryChannelInformation = Information->SysQueryChannelInformation;
	API.m_SetOutputColor = Information->UISetColor;
	API.m_WriteOutputTimestamp = Information->UIWriteTimestamp;
	API.m_WriteOutputString = Information->UIWriteString;

	// REVISION 2
#if HIGHEST_REVISION >= 2
	if (Revision >= 2)
		API.m_ProcessBotCommand = Information2->SysProcessBotCommand;
#endif

	// REVISION 3
#if HIGHEST_REVISION >= 3
	if (Revision >= 3) {
		API.m_GetRegistryPath = Information3->RegGetRegistryPath;
		API.m_ResetAwayIdle = Information3->NetResetAwayIdle;
		API.m_IsService = Information3->SysIsService;
		API.m_GetWindow = Information3->UIGetWindow;
		API.m_GetMenu = Information3->UIGetMenu;
	}
#endif

	// REVISION 4
#if HIGHEST_REVISION >= 4
	if (Revision >= 4) {
		API.m_IsUserFiltered = Information4->FltIsUserFiltered;
		API.m_IsMessageFiltered = Information4->FltIsMessageFiltered;
		API.m_EnumUserFilters = Information4->FltEnumUserFilters;
		API.m_EnumMessageFilters = Information4->FltEnumMessageFilters;
		API.m_AddUserFilter = Information4->FltAddUserFilter;
		API.m_AddMessageFilter = Information4->FltAddMessageFilter;
		API.m_RemoveUserFilter = Information4->FltRemoveUserFilter;
		API.m_RemoveMessageFilter = Information4->FltRemoveMessageFilter;
		API.NotifyMsg = Information4->UINotifyMsg;
		API.m_EnumLoadedPlugins = Information4->SysEnumLoadedPlugins;
	}
#endif

	// Version functions
#if BINARYCHATAPI_ENABLED
	HINSTANCE BinaryChat;

	if (BinaryChat = GetModuleHandle(L"BinaryChat.exe")) {
		(FARPROC&)API.m_GetBotVersion = GetProcAddress(BinaryChat, "GetBotVersion");
		(FARPROC&)API.m_GetBotVersionEx = GetProcAddress(BinaryChat, "GetBotVersionEx");
		(FARPROC&)API.m_QueryConfigurationSetting = GetProcAddress(BinaryChat, "QueryConfigurationSetting");
	}
#endif

	// Read the hInstance
	PluginInstance.m_PluginInstance = Information->PluginInstance;

	// Set the hook addresses
#if MESSAGEHOOK_ENABLED
	Information->MessageHook = &HookManager::MessageHook;
	Information->MessageHookParam = (LPARAM)&PluginInstance;
#endif
#if CONNECTIONHOOK_ENABLED
	Information->ConnectionHook = &HookManager::ConnectionHook;
	Information->ConnectionHookParam = (LPARAM)&PluginInstance;
#endif
#if PLUGINTIMER_ENABLED
	Information->PluginTimer = &HookManager::PluginTimer;
	Information->PluginTimerParam = (LPARAM)&PluginInstance;
#endif
#if COMMANDHOOK_ENABLED
	Information->CommandHook = &HookManager::CommandHook;
	Information->CommandHookParam = (LPARAM)&PluginInstance;
#endif
#if EVENTHOOK_ENABLED
	Information->EventHook = &HookManager::EventHook;
	Information->EventHookParam = (LPARAM)&PluginInstance;
	PluginInstance.m_Event = Information->PluginEvent;
#endif

	return Initialized = PluginInstance.Initialize();
}
