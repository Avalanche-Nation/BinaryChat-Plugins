#ifndef _BCPLUGIN_H_INCLUDED
#define _BCPLUGIN_H_INCLUDED
#pragma once

#include <windows.h>

#include <cstddef>
#include <cstdint>

class BotNetConnectionState;

namespace PluginAPI
{
	using Bool32 = int32_t;
	inline constexpr Bool32 False = 0;
	inline constexpr Bool32 True = 1;

	using ModuleHandle = void*; // HINSTANCE
	using WindowHandle = void*; // HWND
	using MenuHandle   = void*; // HMENU
	using EventHandle  = void*; // HANDLE
}

namespace BinaryChatAPI // Procs are exported from BinaryChat.exe
{
	enum ConfigurationSetting {
		Setup_Username = 0,
		Setup_Password = 1,
		Setup_LastPing = 2,
		Setup_Master = 3,
		Setup_Operator = 4,
		Setup_IdleMessage = 5,
		Setup_Homechannel = 6,
		Setup_Countidle = 7,
		Setup_Log = 8,
		Setup_Timestamp = 9,
		Setup_UserNotifications = 10,
		Setup_BattleNetServer = 11,
		Setup_CDkey = 12,
		Setup_LOD_CDkey = 13,
		Setup_CDkeyName = 14,
		Setup_Spawn = 15,
		Setup_Product = 16,
		Setup_ShowEncryptErrors = 17,
		Setup_TelnetPort = 18,
		Setup_TelnetPassword = 19,
		Setup_EnableRealm = 20,
		Setup_RealmCharacter = 21,
		Setup_CharacterClass = 22,
		Setup_InteractiveRealm = 23,
		Setup_CharacterFlags = 24,
		Setup_PingSpoof = 25,
		Setup_WindowOptions = 26,
		Setup_ShowAdbanners = 27,
		Setup_WindowSize = 28,
		Setup_WindowXPos = 29,
		Setup_WindowYPos = 30,
		Setup_Blank_Character = 31,
		Setup_BotNetServer = 32,
		Setup_BotNetServer_Pass = 33,
		Setup_BotNetDatabaseId = 34,
		Setup_ShowBotNetCmds = 35,
		Setup_ConnectBattleNet = 36
	};

	using GetBotVersionProc             = PluginAPI::Bool32(__stdcall*)(int32_t* Version, char* BuildTime, char* BuildDate, char* AuthBuildName);
	using GetBotVersionExProc           = PluginAPI::Bool32(__stdcall*)(int32_t* Version, char* BuildTime, char* BuildDate, char* AuthBuildName, int32_t* BuildNumber, void* Reserved);
	using QueryConfigurationSettingProc = PluginAPI::Bool32(__stdcall*)(ConfigurationSetting Setting, char* Profile, void* Buffer, uint32_t* BufferLength);

}

namespace PluginAPI
{

	//////////////////////////////////////////////////////////////////////////////
	//
	// REVISION 1
	//

	enum ConnectionInterface : int32_t {
		Connection_BNCS,
		Connection_MCP,
		Connection_GS,
		Connection_Count // Last interface index +1 = amount of interfaces
	};

	enum ConnectionState : int32_t {
		Connection_Closed,
		Connection_Opened
	};

	enum OutputColor : int32_t {
		White, Gray, Blue, Green, Red, Yellow, Purple, BlueGreen,
		MidBlue, DarkBlue, DarkGreen, DarkYellow, DarkRed, DarkPurple,
		LightBlue,
		NumColors // Last color index +1 = amount of colors
	};

	struct StateInformation {
		uint32_t Size;
		char ChannelName[256];
		uint32_t ChannelFlags;
		char UniqueName[256];
		char AccountName[256];
		char Statstring[256];
		ConnectionState State[Connection_Count];
	};

	// Prototypes of callback functions

	// Hooks:
	using MessageHookProc             = Bool32(__stdcall*)(uint32_t ConnectionId, uint8_t MessageId, const void* MessageData, uint32_t MessageSize, intptr_t lParam);
	using ConnectionHookProc          = Bool32(__stdcall*)(uint32_t ConnectionId, uint32_t EventCode, uint64_t PlatformId, intptr_t lParam);
	using PluginTimerProc             = uint32_t(__stdcall*)(uint32_t TimeDelay, intptr_t lParam);
	using CommandHookProc             = Bool32(__stdcall*)(const char* Message, const char* SendingUser, Bool32 FromTelnet, intptr_t lParam);
	using EventHookProc               = Bool32(__stdcall*)(intptr_t lParam);

	// Channel enum:
	using EnumChannelProc             = Bool32(__stdcall*)(const char* UniqueName, const char* Statstring, uint32_t Flags, uint32_t Ping, Bool32 Invisible, uint32_t AccountNumber, uint32_t RegAuthority, intptr_t lParam);

	// Prototypes of BinaryChat API functions
	using ClearMessageProc            = Bool32(__stdcall*)(uint8_t MessageId);
	using IsMessageSetProc            = Bool32(__stdcall*)(uint8_t MessageId);
	using SetMessageProc              = Bool32(__stdcall*)(uint8_t MessageId);
	using QueueMessageProc            = Bool32(__stdcall*)(uint8_t MessageId, const void* MessageData, uint32_t MessageSize);
	using QueueChatMessageProc        = Bool32(__stdcall*)(char* ChatMessage, Bool32 FixD2Command);
	using QueryStateInformationProc   = Bool32(__stdcall*)(StateInformation* StateInfo);
	using QueryChannelInformationProc = Bool32(__stdcall*)(EnumChannelProc Callback, intptr_t lParam);
	using CloseConnectionProc         = Bool32(__stdcall*)(uint32_t ConnectionId);
	using SendConnectionMessageProc   = Bool32(__stdcall*)(uint32_t ConnectionId, uint8_t MessageId, const void* MessageData, uint32_t MessageSize);
	using SetOutputColorProc          = Bool32(__stdcall*)(OutputColor Color);
	using WriteOutputTimestampProc    = Bool32(__stdcall*)(void);
	using WriteOutputStringProc       = Bool32(__stdcall*)(const char* String);

	struct PluginInformation {
		uint32_t Size;
		ClearMessageProc            NetClearMessage;
		IsMessageSetProc            NetIsMessageSet;
		SetMessageProc              NetSetMessage;
		QueueMessageProc            NetQueueMessage;
		QueueChatMessageProc        NetQueueChatMessage;
		QueryStateInformationProc   SysQueryStateInformation;
		QueryChannelInformationProc SysQueryChannelInformation;
		CloseConnectionProc         NetCloseConnection;
		SendConnectionMessageProc   NetSendConnectionMessage;
		SetOutputColorProc          UISetColor;
		WriteOutputTimestampProc    UIWriteTimestamp;
		WriteOutputStringProc       UIWriteString;

		ModuleHandle BotNetClientInstance;
		BotNetConnectionState* BotNetConnection;

		MessageHookProc MessageHook;        // non-const
		intptr_t MessageHookParam;          // non-const

		ConnectionHookProc ConnectionHook;  // non-const
		intptr_t ConnectionHookParam;       // non-const

		PluginTimerProc PluginTimer;        // non-const
		intptr_t PluginTimerParam;          // non-const

		CommandHookProc CommandHook;        // non-const
		intptr_t CommandHookParam;          // non-const

		ModuleHandle PluginInstance;

		EventHookProc EventHook;            // non-const
		intptr_t EventHookParam;            // non-const
		EventHandle PluginEvent;
	};

	//////////////////////////////////////////////////////////////////////////////
	//
	// REVISION 2
	//

	using ProcessBotCommandProc = Bool32(__stdcall*)(const char* lpszCommand);

	struct PluginInformation2 : public PluginInformation
	{
		ProcessBotCommandProc SysProcessBotCommand;
	};

	//////////////////////////////////////////////////////////////////////////////
	//
	// REVISION 3
	//

	enum BinaryChatWindow : int32_t {
		MainWindow,
		ChannelList,
		ChannelNameStatic,
		UserInput,
		ChatOutput,
		AccountManager,
		RealmManager,
		DownloadManager,
		NewsWindow,
		FiltersManager,
		ChannelsManager
	};

	enum BinaryChatMenu : int32_t {
		MainWindowMenu,
		ChannelUserPopupMenu // NOTE: this is actually the first submenu, not the parent menu
	};

	using GetRegistryPathProc = const char* (__stdcall*)(void);
	using ResetAwayIdleProc   = Bool32(__stdcall*)(void);
	using GetWindowProc       = WindowHandle(__stdcall*)(BinaryChatWindow WindowId);
	using IsServiceProc       = Bool32(__stdcall*)(void);
	using GetMenuProc         = MenuHandle(__stdcall*)(BinaryChatMenu MenuId);

	struct PluginInformation3 : public PluginInformation2
	{
		GetRegistryPathProc RegGetRegistryPath;
		ResetAwayIdleProc NetResetAwayIdle;
		GetWindowProc UIGetWindow;
		IsServiceProc SysIsService;
		GetMenuProc UIGetMenu;
	};

	//////////////////////////////////////////////////////////////////////////////
	//
	// REVISION 4
	//

	using EnumFilterProc = Bool32(__stdcall*)(const char* Filter, intptr_t lParam);
	using EnumPluginsProc = Bool32(__stdcall*)(ModuleHandle PluginInst, intptr_t lParam);

	using IsUserFilteredProc      = Bool32(__stdcall*)(const char* User);
	using IsMessageFilteredProc   = Bool32(__stdcall*)(const char* Message);
	using EnumUserFiltersProc     = Bool32(__stdcall*)(EnumFilterProc Callback, intptr_t lParam);
	using EnumMessageFiltersProc  = Bool32(__stdcall*)(EnumFilterProc Callback, intptr_t lParam);
	using AddUserFilterProc       = Bool32(__stdcall*)(const char* Filter, Bool32 SaveFilters);
	using AddMessageFilterProc    = Bool32(__stdcall*)(const char* Filter, Bool32 SaveFilters);
	using RemoveUserFilterProc    = Bool32(__stdcall*)(const char* Filter, Bool32 SaveFilters);
	using RemoveMessageFilterProc = Bool32(__stdcall*)(const char* Filter, Bool32 SaveFilters);
	using NotifyMsgProc           = Bool32(__cdecl*)(OutputColor Color, const char* User, const char* MessageFormat, ...);
	using EnumLoadedPluginsProc   = Bool32(__stdcall*)(EnumPluginsProc Callback, intptr_t lParam);

	struct PluginInformation4 : public PluginInformation3 {
		IsUserFilteredProc FltIsUserFiltered;
		IsMessageFilteredProc FltIsMessageFiltered;
		EnumUserFiltersProc FltEnumUserFilters;
		EnumMessageFiltersProc FltEnumMessageFilters;
		AddUserFilterProc FltAddUserFilter;
		AddMessageFilterProc FltAddMessageFilter;
		RemoveUserFilterProc FltRemoveUserFilter;
		RemoveMessageFilterProc FltRemoveMessageFilter;
		NotifyMsgProc UINotifyMsg;
		EnumLoadedPluginsProc SysEnumLoadedPlugins;
	};

	//------------------------------------------------------------------------
	// ABI guards. Values are what the original Win32-typed header produces.
	//------------------------------------------------------------------------
	static_assert(sizeof(Bool32) == 4, "BOOL must stay 32-bit");
	static_assert(sizeof(intptr_t) == sizeof(void*), "LPARAM must stay pointer-sized");
	static_assert(sizeof(ConnectionState) == 4 && sizeof(OutputColor) == 4 &&
		sizeof(ConnectionInterface) == 4 && sizeof(BinaryChatWindow) == 4 &&
		sizeof(BinaryChatMenu) == 4, "enums must stay 32-bit");
	static_assert(sizeof(StateInformation) == 1044, "StateInformation layout changed");

	constexpr std::size_t PluginAbiSize(std::size_t x86, std::size_t x64) { return sizeof(void*) == 4 ? x86 : x64; }
	static_assert(sizeof(PluginInformation) == PluginAbiSize(108, 216), "PluginInformation (rev 1) layout changed");
	static_assert(sizeof(PluginInformation2) == PluginAbiSize(112, 224), "PluginInformation2 layout changed");
	static_assert(sizeof(PluginInformation3) == PluginAbiSize(132, 264), "PluginInformation3 layout changed");
	static_assert(sizeof(PluginInformation4) == PluginAbiSize(172, 344), "PluginInformation4 layout changed");
}

// If you are the BinaryChat
//		using PluginMainProc = PluginAPI::Bool32(__stdcall*)(PluginAPI::PluginInformation* Information);
// Else
PluginAPI::Bool32 __stdcall PluginMain(PluginAPI::PluginInformation* Information);
// Export as "SetInterfaceInformation"
// End If

#endif // _BCPLUGIN_H_INCLUDED