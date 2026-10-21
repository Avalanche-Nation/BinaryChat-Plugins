#ifndef _BINARYCHATAPIMANAGER_H_INCLUDED
#define _BINARYCHATAPIMANAGER_H_INCLUDED
#pragma once

#include "BCPlugin.h"
#include "PluginSettings.h"

class BinaryChatAPIManager
{
	friend PluginAPI::Bool32 __stdcall PluginMain(PluginAPI::PluginInformation* Information);
	// PluginMain may set the private variables (proc addresses)

private:
	uint32_t m_Revision; // Revision number

	// REVISION 1

	PluginAPI::ClearMessageProc						m_ClearMessage;
	PluginAPI::IsMessageSetProc						m_IsMessageSet;
	PluginAPI::SetMessageProc						m_SetMessage;
	PluginAPI::QueueMessageProc						m_QueueMessage;
	PluginAPI::QueueChatMessageProc					m_QueueChatMessage;
	PluginAPI::CloseConnectionProc					m_CloseConnection;
	PluginAPI::SendConnectionMessageProc			m_SendConnectionMessage;
	PluginAPI::QueryStateInformationProc			m_QueryStateInformation;
	PluginAPI::QueryChannelInformationProc			m_QueryChannelInformation;
	PluginAPI::SetOutputColorProc					m_SetOutputColor;
	PluginAPI::WriteOutputTimestampProc				m_WriteOutputTimestamp;
	PluginAPI::WriteOutputStringProc				m_WriteOutputString;

	static PluginAPI::Bool32 __stdcall EnumChannelHook(const char* UniqueName, const char* Statstring, uint32_t Flags, uint32_t Ping, PluginAPI::Bool32 Invisible, uint32_t AccountNumber, uint32_t RegAuthority, intptr_t lParam);

	// REVISION 2

#if HIGHEST_REVISION >= 2
	PluginAPI::ProcessBotCommandProc				m_ProcessBotCommand;
#endif

	// REVISION 3

#if HIGHEST_REVISION >= 3
	PluginAPI::GetRegistryPathProc					m_GetRegistryPath;
	PluginAPI::ResetAwayIdleProc					m_ResetAwayIdle;
	PluginAPI::IsServiceProc						m_IsService;
	PluginAPI::GetWindowProc						m_GetWindow;
	PluginAPI::GetMenuProc							m_GetMenu;
#endif

	// REVISION 4

#if HIGHEST_REVISION >= 4
	PluginAPI::IsUserFilteredProc					m_IsUserFiltered;
	PluginAPI::IsMessageFilteredProc				m_IsMessageFiltered;
	PluginAPI::EnumUserFiltersProc					m_EnumUserFilters;
	PluginAPI::EnumMessageFiltersProc				m_EnumMessageFilters;
	PluginAPI::AddUserFilterProc					m_AddUserFilter;
	PluginAPI::AddMessageFilterProc					m_AddMessageFilter;
	PluginAPI::RemoveUserFilterProc					m_RemoveUserFilter;
	PluginAPI::RemoveMessageFilterProc				m_RemoveMessageFilter;
	// PluginAPI::NotifyMsgProc						m_NotifyMsg; // Public because of ellipsis
	PluginAPI::EnumLoadedPluginsProc				m_EnumLoadedPlugins;

	static PluginAPI::Bool32 __stdcall EnumUserFiltersHook(const char* Filter, intptr_t lParam);
	static PluginAPI::Bool32 __stdcall EnumMessageFiltersHook(const char* Filter, intptr_t lParam);
	static PluginAPI::Bool32 __stdcall EnumLoadedPluginsHook(PluginAPI::ModuleHandle PluginInst, intptr_t lParam);
#endif

	// BinaryChat Version API

#if BINARYCHATAPI_ENABLED
	BinaryChatAPI::GetBotVersionProc				m_GetBotVersion;
	BinaryChatAPI::GetBotVersionExProc				m_GetBotVersionEx;
	BinaryChatAPI::QueryConfigurationSettingProc	m_QueryConfigurationSetting;
#endif

public:
	uint32_t Revision() { return m_Revision; }

	//////////////////////////////////////////////////////////////////////////////
	//
	// REVISION 1
	//

	PluginAPI::Bool32 ClearMessage(uint8_t MessageId)
	{
		return m_ClearMessage(MessageId);
	}

	PluginAPI::Bool32 IsMessageSet(uint8_t MessageId)
	{
		return m_IsMessageSet(MessageId);
	}

	PluginAPI::Bool32 SetMessage(uint8_t MessageId)
	{
		return m_SetMessage(MessageId);
	}

	PluginAPI::Bool32 QueueMessage(uint8_t MessageId, const void* MessageData, uint32_t MessageSize)
	{
		return m_QueueMessage(MessageId, MessageData, MessageSize);
	}

	PluginAPI::Bool32 QueueChatMessage(char* ChatMessage, PluginAPI::Bool32 FixD2Command)
	{
		return m_QueueChatMessage(ChatMessage, FixD2Command);
	}

	PluginAPI::Bool32 CloseConnection(PluginAPI::ConnectionInterface Connection)
	{
		return m_CloseConnection(static_cast<uint32_t>(Connection));
	}

	PluginAPI::Bool32 SendConnectionMessage(PluginAPI::ConnectionInterface Connection,
		uint8_t MessageId, const void* MessageData, uint32_t MessageSize)
	{
		return m_SendConnectionMessage(static_cast<uint32_t>(Connection), MessageId, MessageData, MessageSize);
	}

	PluginAPI::Bool32 QueryStateInformation(PluginAPI::StateInformation* StateInfo)
	{
		return m_QueryStateInformation(StateInfo);
	}

	PluginAPI::Bool32 QueryChannelInformation()
	{
		return m_QueryChannelInformation(&BinaryChatAPIManager::EnumChannelHook, reinterpret_cast<intptr_t>(&PluginInstance));
	}

	PluginAPI::Bool32 SetOutputColor(PluginAPI::OutputColor Color)
	{
		return m_SetOutputColor(Color);
	}

	PluginAPI::Bool32 WriteOutputTimestamp()
	{
		return m_WriteOutputTimestamp();
	}

	PluginAPI::Bool32 WriteOutputString(const char* String)
	{
		return m_WriteOutputString(String);
	}

	//////////////////////////////////////////////////////////////////////////////
	//
	// REVISION 2
	// (Make sure this revision is present before calling this, or it'll crash!)
	//

#if HIGHEST_REVISION >= 2
	PluginAPI::Bool32 ProcessBotCommand(const char* Command)
	{
		return m_ProcessBotCommand(Command);
	}
#endif

	//////////////////////////////////////////////////////////////////////////////
	//
	// REVISION 3
	// (Make sure this revision is present before calling this, or it'll crash!)
	//

#if HIGHEST_REVISION >= 3
	const char* GetRegistryPath()
	{
		return m_GetRegistryPath();
	}

	PluginAPI::Bool32 ResetAwayIdle()
	{
		return m_ResetAwayIdle();
	}

	PluginAPI::Bool32 IsService()
	{
		return m_IsService();
	}

	PluginAPI::WindowHandle GetWindow(PluginAPI::BinaryChatWindow WindowId)
	{
		return m_GetWindow(WindowId);
	}

	PluginAPI::MenuHandle GetMenu(PluginAPI::BinaryChatMenu MenuId)
	{
		return m_GetMenu(MenuId);
	}
#endif

	//////////////////////////////////////////////////////////////////////////////
	//
	// REVISION 4
	// (Make sure this revision is present before calling this, or it'll crash!)
	//

#if HIGHEST_REVISION >= 4
	PluginAPI::Bool32 IsUserFiltered(const char* User)
	{
		return m_IsUserFiltered(User);
	}

	PluginAPI::Bool32 IsMessageFiltered(const char* Message)
	{
		return m_IsMessageFiltered(Message);
	}

	PluginAPI::Bool32 EnumUserFilters()
	{
		return m_EnumUserFilters(&BinaryChatAPIManager::EnumUserFiltersHook, reinterpret_cast<intptr_t>(&PluginInstance));
	}

	PluginAPI::Bool32 EnumMessageFilters()
	{
		return m_EnumMessageFilters(&BinaryChatAPIManager::EnumMessageFiltersHook, reinterpret_cast<intptr_t>(&PluginInstance));
	}

	PluginAPI::Bool32 AddUserFilter(const char* Filter, PluginAPI::Bool32 SaveFilters)
	{
		return m_AddUserFilter(Filter, SaveFilters);
	}

	PluginAPI::Bool32 AddMessageFilter(const char* Filter, PluginAPI::Bool32 SaveFilters)
	{
		return m_AddMessageFilter(Filter, SaveFilters);
	}

	PluginAPI::Bool32 RemoveUserFilter(const char* Filter, PluginAPI::Bool32 SaveFilters)
	{
		return m_RemoveUserFilter(Filter, SaveFilters);
	}

	PluginAPI::Bool32 RemoveMessageFilter(const char* Filter, PluginAPI::Bool32 SaveFilters)
	{
		return m_RemoveMessageFilter(Filter, SaveFilters);
	}

	PluginAPI::NotifyMsgProc NotifyMsg; // Public because of ellipsis

	PluginAPI::Bool32 EnumLoadedPlugins()
	{
		return m_EnumLoadedPlugins(&BinaryChatAPIManager::EnumLoadedPluginsHook, reinterpret_cast<intptr_t>(&PluginInstance));
	}
#endif

	// BinaryChat Version functions
	// Return type for all of these is a "troolean":
	// TRUE on success, FALSE on failure, -1 indicating function not found
#if BINARYCHATAPI_ENABLED
	PluginAPI::Bool32 GetBotVersion(int32_t* Version, char* BuildTime, char* BuildDate, char* AuthBuildName)
	{
		return m_GetBotVersion
			? m_GetBotVersion(Version, BuildTime, BuildDate, AuthBuildName)
			: -1;
	}

	PluginAPI::Bool32 GetBotVersionEx(int32_t* Version, char* BuildTime, char* BuildDate, char* AuthBuildName, int32_t* BuildNumber, void* Reserved)
	{
		return m_GetBotVersionEx
			? m_GetBotVersionEx(Version, BuildTime, BuildDate, AuthBuildName, BuildNumber, NULL)
			: -1;
	}

	PluginAPI::Bool32 QueryConfigurationSetting(BinaryChatAPI::ConfigurationSetting Setting, char* Profile, void* Buffer, uint32_t* BufferLength)
	{
		return m_QueryConfigurationSetting
			? m_QueryConfigurationSetting(Setting, Profile, Buffer, BufferLength)
			: -1;
	}
#endif
};

#endif // _BINARYCHATAPIMANAGER_H_INCLUDED