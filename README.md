## BinaryChat Plugin Wiki

[Overview](#overview)[Source files](#files)[Lifecycle](#lifecycle)[PluginSettings.h](#settings)

Callbacks

[Hooks](#hooks)[Enumeration callbacks](#enums)

Calling the host

[Revision 1](#rev1)[Revision 2](#rev2)[Revision 3](#rev3)[Revision 4](#rev4)[Version API](#version)

Reference

[Types & enums](#types)[ConfigurationSetting](#config)[PluginInformation](#structs)[ABI guards](#abi)[Gotchas](#gotchas)[Recipes](#recipes)

# BinaryChat Plugin API

A C++ plugin template for **BinaryChat.exe**, a Battle.net chat bot. A plugin is a DLL that exports one function (`PluginMain`, exported as `SetInterfaceInformation`). The host passes a table of function pointers in, and the plugin hands back callbacks (hooks) the host calls on events. This page summarizes everything in the supplied headers and template.

## Overview

| Direction | Mechanism | Where |
| --- | --- | --- |
| Host → plugin | Five **hooks** (message, connection, timer, command, event) plus three **enumeration callbacks** (channel, filters, loaded plugins) | `HookManager` / `Plugin` |
| Plugin → host | Function pointers in `PluginInformation` (rev 1–4), wrapped by the global `API` object | `BinaryChatAPIManager` |
| Plugin → host (extra) | Three procs fetched with `GetProcAddress` from BinaryChat.exe (version/config) | `BinaryChatAPI::*Proc` |

Two globals exist: `Plugin PluginInstance` and `BinaryChatAPIManager API`. Hooks are static `__stdcall` trampolines that cast `lParam` back to the instance and call a virtual method you override in `Plugin`.

## Source files

| File | Purpose |
| --- | --- |
| `BCPlugin.h` | The ABI contract: enums, callback typedefs, `PluginInformation` 1–4, ABI `static_assert`s, `PluginMain` declaration |
| `PluginSettings.h` | Compile-time switches for hooks, Version API, and revision range; declares `PluginInstance` and `API` |
| `BinaryChatAPIManager.h/.cpp` | Class `API`: stores proc pointers, exposes wrapper methods, static enum trampolines |
| `HookManager.h/.cpp` | Abstract base: static hook trampolines, pure virtuals, `m_PluginInstance`, `m_Event` |
| `Plugin.h/.cpp` | **Your code goes here.** Overrides of every virtual, plus `Initialize()`/`Terminate()` |
| `PluginMain.cpp` | `DllMain`, `PluginMain` (revision detection, filling `API`, installing hooks) |
| `PluginExports.def` | Export definition (see the header note: export as `SetInterfaceInformation`) |
| `Template.slnx / .vcxproj / .filters / .user` | Visual Studio solution and project |

## Lifecycle

1. **DLL load**: `DllMain(DLL_PROCESS_ATTACH)` calls `DisableThreadLibraryCalls`. Nothing else; the default case is `__assume(0)`, which is valid only because thread notifications are disabled.
2. **Host calls `PluginMain(Information)`**:
   1. `ZeroMemory(&API)` so absent revisions are null.
   2. Revision is derived from `Information->Size` (≥ sizeof Info4 → 4, Info3 → 3, Info2 → 2, else 1), limited by `HIGHEST_REVISION`. If below `LOWEST_REVISION`, returns `FALSE` and the plugin isn't loaded.
   3. Rev 1 pointers always copied; rev 2–4 pointers only when that revision is present.
   4. Version API procs looked up in `BinaryChat.exe` (if `BINARYCHATAPI_ENABLED`).
   5. `PluginInstance.m_PluginInstance` = the plugin's HINSTANCE from the host.
   6. Each enabled hook and its `lParam` (`&PluginInstance`) is written back into `Information`. With `EVENTHOOK_ENABLED`, the host's `PluginEvent` handle is saved to `m_Event`.
   7. Returns `Plugin::Initialize()`.
3. **Running**: host invokes your hooks; you call `API.*`.
4. **Unload**: `DLL_PROCESS_DETACH` calls `Terminate()` only if `Initialize()` returned true.

`Initialize()`/`Terminate()` replace the constructor/destructor. Returning `False` from `Initialize` aborts loading and `Terminate` is not called.

```
// Plugin.cpp: typical Initialize / Terminate
Bool32 Plugin::Initialize()
{
	if (API.Revision() < 3)               // e.g. we rely on GetWindow
		return PluginAPI::False;           // abort load; Terminate() won't run
	API.SetOutputColor(PluginAPI::Green);
	API.WriteOutputString("MyPlugin loaded");
	return PluginAPI::True;
}

void Plugin::Terminate() { /* release resources; runs inside DLL_PROCESS_DETACH */ }
```

## PluginSettings.h

| Macro | Effect |
| --- | --- |
| `MESSAGEHOOK_ENABLED` | Compiles in and installs `MessageHook` |
| `CONNECTIONHOOK_ENABLED` | …`ConnectionHook` |
| `PLUGINTIMER_ENABLED` | …`PluginTimer` |
| `COMMANDHOOK_ENABLED` | …`CommandHook` |
| `EVENTHOOK_ENABLED` | …`EventHook` and the `m_Event` handle |
| `BINARYCHATAPI_ENABLED` | Version/config API (`GetBotVersion`, `GetBotVersionEx`, `QueryConfigurationSetting`) |
| `LOWEST_REVISION` (1) | Host older than this → plugin refuses to load |
| `HIGHEST_REVISION` (4) | Highest revision the plugin uses; strips rev 2–4 code below it. `API.Revision()` is guaranteed in \[LOWEST, HIGHEST\] |

Set a hook switch to false when unused: the host then never calls into you for it.

## Hooks (host calls you)

Override these in `Plugin`. The template bodies return `False` (timer returns `TimeDelay`). The meaning of each return value is defined by the host, which these files don't spell out, so check BinaryChat's behavior before relying on one.

### MessageHook rev 1

```
Bool32 MessageHook(ConnectionInterface Connection, uint8_t MessageId,
                   const void* MessageData, uint32_t MessageSize);
```

Called for protocol messages on a connection: `Connection_BNCS` (Battle.net chat server), `Connection_MCP` (realm), `Connection_GS` (game server). `MessageId` is the packet ID, `MessageData/Size` the raw payload. Pair with `SetMessage`/`ClearMessage`/`IsMessageSet` to subscribe to or suppress specific IDs.

```
// Initialize(): flag the IDs we care about (0x0F = SID_CHATEVENT)
API.SetMessage(0x0F);

Bool32 Plugin::MessageHook(PluginAPI::ConnectionInterface c, uint8_t id,
                           const void* data, uint32_t size)
{
	if (c == PluginAPI::Connection_BNCS && id == 0x0F && size >= 4) {
		const auto* p = static_cast<const uint8_t*>(data);
		// parse p[0 .. size) - data is only valid for the duration of this call
	}
	return PluginAPI::False;
}
```

### ConnectionHook rev 1

```
Bool32 ConnectionHook(ConnectionInterface Connection, ConnectionState EventCode, uint64_t PlatformId);
```

Connection opened/closed (`Connection_Opened`/`Connection_Closed`) for one of the three interfaces. `PlatformId` is 64-bit.

```
Bool32 Plugin::ConnectionHook(PluginAPI::ConnectionInterface c,
                              PluginAPI::ConnectionState s, uint64_t)
{
	if (c == PluginAPI::Connection_BNCS)
		API.WriteOutputString(s == PluginAPI::Connection_Opened
			? "Chat server connected" : "Chat server disconnected");
	return PluginAPI::False;
}
```

### PluginTimer rev 1

```
uint32_t PluginTimer(uint32_t TimeDelay);
```

Periodic tick. The return value is a delay (`uint32_t`); the template returns the incoming `TimeDelay` unchanged. Presumably the host uses it to schedule the next call, so return a different value to change the interval.

```
// Plugin.h: uint32_t m_Ticks = 0;
uint32_t Plugin::PluginTimer(uint32_t TimeDelay)
{
	if (++m_Ticks % 10 == 0)
		API.QueryChannelInformation();   // results arrive in EnumChannelHook
	return TimeDelay;                    // keep the host's interval
}
```

### CommandHook rev 1

```
Bool32 CommandHook(const char* Message, const char* SendingUser, Bool32 FromTelnet);
```

A bot command received from a chat user, or from the Telnet interface (`FromTelnet`). Use it to implement custom commands.

```
#include <string>
#include <string_view>

Bool32 Plugin::CommandHook(const char* Message, const char* SendingUser, Bool32 FromTelnet)
{
	if (std::string_view{ Message }.starts_with("!hello")) {
		std::string reply = std::string("Hello, ") + SendingUser;
		API.QueueChatMessage(reply.data(), PluginAPI::False);  // takes char*, not const
		return PluginAPI::True;   // convention assumed: True = handled
	}
	return PluginAPI::False;
}
```

### EventHook rev 1

```
Bool32 EventHook();   // m_Event is the HANDLE (PluginEvent) the host signals
```

Invoked when the plugin's event object is signaled. Use `m_Event` to wake the plugin from another thread.

**Example: worker thread → host thread via `PluginEvent`**. The worker queues work and signals `m_Event`; the host then calls `EventHook()` on its own thread, where it is safe to use `API.*`.

```
// Plugin.h additions
#include <mutex>
#include <queue>
#include <string>
#include <thread>

std::jthread m_Worker;
std::mutex m_Lock;
std::queue<std::string> m_Pending;

// Plugin.cpp
Bool32 Plugin::Initialize()
{
	m_Worker = std::jthread([this](std::stop_token st) {
		while (!st.stop_requested()) {
			std::string line = WaitForSomethingInteresting();   // your blocking source, with a timeout
			{
				std::lock_guard g{ m_Lock };
				m_Pending.push(std::move(line));
			}
			SetEvent(m_Event);          // host will now call EventHook()
		}
	});
	return PluginAPI::True;
}

Bool32 Plugin::EventHook()
{
	std::queue<std::string> local;
	{
		std::lock_guard g{ m_Lock };
		local.swap(m_Pending);          // grab everything, release the lock fast
	}
	while (!local.empty()) {
		API.WriteOutputString(local.front().c_str());
		local.pop();
	}
	return PluginAPI::False;
}
```

**Shutdown caution.** `Terminate()` runs inside `DLL_PROCESS_DETACH` under the loader lock. Calling `join()` there can deadlock, so prefer to request stop and let the worker exit on its own (or stop it earlier, for example from a command). Keep the worker from touching `API.*` or the DLL after unload.

## Enumeration callbacks

These fire only as a result of you calling the matching `API.Query/Enum*` function. The template passes `&PluginInstance` as `lParam` and the static trampoline forwards to your virtual. Template bodies return `True`; presumably `True` continues the enumeration and `False` stops it.

| Trigger | Override | Arguments |
| --- | --- | --- |
| `API.QueryChannelInformation()` | `EnumChannelHook` | `UniqueName, Statstring, Flags, Ping, Invisible, AccountNumber, RegAuthority` |
| `API.EnumUserFilters()` rev 4 | `EnumUserFiltersHook` | `Filter` |
| `API.EnumMessageFilters()` rev 4 | `EnumMessageFiltersHook` | `Filter` |
| `API.EnumLoadedPlugins()` rev 4 | `EnumLoadedPluginsHook` | `ModuleHandle PluginInst` |

```
// Plugin.h: std::vector<std::string> m_Users;
m_Users.clear();
API.QueryChannelInformation();       // synchronously calls EnumChannelHook once per user

Bool32 Plugin::EnumChannelHook(const char* UniqueName, const char*, uint32_t Flags, uint32_t Ping,
                               Bool32 Invisible, uint32_t, uint32_t)
{
	if (!Invisible) m_Users.emplace_back(UniqueName);
	return PluginAPI::True;           // keep enumerating
}
```

## API (you call the host)

All calls go through the global `API`. Methods for revisions 2–4 must only be called when `API.Revision()` is high enough, otherwise the pointer is null and it will crash.

### Revision 1: messaging, state, output

| Method | What it does |
| --- | --- |
| `SetMessage / ClearMessage / IsMessageSet(id)` | Mark, unmark, or test whether a message ID is flagged (controls what reaches your MessageHook) |
| `QueueMessage(id, data, size)` | Queue a raw message for sending |
| `QueueChatMessage(char* msg, FixD2Command)` | Send chat text; `FixD2Command` adjusts Diablo II-style commands. Note: non-const `char*` |
| `SendConnectionMessage(conn, id, data, size)` | Send a message directly on BNCS/MCP/GS |
| `CloseConnection(conn)` | Close one of the connections |
| `QueryStateInformation(StateInformation*)` | Fills channel name/flags, unique name, account name, statstring, and per-connection state. Set `Size` first |
| `QueryChannelInformation()` | Enumerate channel users into `EnumChannelHook` |
| `SetOutputColor(color)` | Set color for subsequent output |
| `WriteOutputTimestamp()` | Write a timestamp to the chat window |
| `WriteOutputString(str)` | Write text to the chat window |

```
PluginAPI::StateInformation si{};
si.Size = sizeof(si);                          // required before the call
if (API.QueryStateInformation(&si)) {
	API.SetOutputColor(PluginAPI::Yellow);
	API.WriteOutputTimestamp();
	API.WriteOutputString(si.ChannelName);
	if (si.State[PluginAPI::Connection_BNCS] == PluginAPI::Connection_Closed)
		API.CloseConnection(PluginAPI::Connection_MCP);
}
```

### Revision 2

`ProcessBotCommand(const char* Command)`: run a bot command as if typed locally.

```
if (API.Revision() >= 2)
	API.ProcessBotCommand("/help");   // goes through the bot's normal command handling
```

### Revision 3: registry, window and menu access

| Method | What it does |
| --- | --- |
| `GetRegistryPath()` | Returns the bot's registry path (`const char*`) |
| `ResetAwayIdle()` | Reset away/idle timers |
| `IsService()` | True if the bot is running as a service |
| `GetWindow(BinaryChatWindow)` | HWND of a bot window (see enums) |
| `GetMenu(BinaryChatMenu)` | HMENU of the main menu or the channel user popup submenu |

```
if (API.Revision() >= 3) {
	const char* regPath = API.GetRegistryPath();               // per-bot registry location
	auto hOutput = static_cast<HWND>(API.GetWindow(PluginAPI::ChatOutput));
	auto hMenu   = static_cast<HMENU>(API.GetMenu(PluginAPI::ChannelUserPopupMenu));
	AppendMenuW(hMenu, MF_STRING, 0xA000, L"My action");      // choose an ID that won't collide
	if (!API.IsService()) { /* safe to show UI */ }
	API.ResetAwayIdle();
}
```

### Revision 4: filters, notifications, plugins

| Method | What it does |
| --- | --- |
| `IsUserFiltered(user)` / `IsMessageFiltered(msg)` | Test against the bot's filter lists |
| `EnumUserFilters()` / `EnumMessageFilters()` | List filters via callback |
| `AddUserFilter / AddMessageFilter(filter, Save)` | Add a filter; `Save` persists the list |
| `RemoveUserFilter / RemoveMessageFilter(filter, Save)` | Remove a filter |
| `API.NotifyMsg(color, user, fmt, ...)` | printf-style notification in the UI. **Public member function pointer, `__cdecl`**, because of the ellipsis |
| `EnumLoadedPlugins()` | List other loaded plugin modules via callback |

```
if (API.Revision() >= 4) {
	if (!API.IsUserFiltered("spammer"))
		API.AddUserFilter("spammer", PluginAPI::True);   // True = persist the list
	API.NotifyMsg(PluginAPI::Red, "spammer", "filtered (%d hits)", 3);  // printf-style, __cdecl
	API.EnumLoadedPlugins();                              // results in EnumLoadedPluginsHook
}
```

### BinaryChat Version / config API

Resolved with `GetProcAddress` from `BinaryChat.exe`, so it isn't tied to the plugin revision. Each wrapper returns a “troolean”: `TRUE` success, `FALSE` failure, `-1` export not found.

| Method | Notes |
| --- | --- |
| `GetBotVersion(int32_t* Version, char* BuildTime, char* BuildDate, char* AuthBuildName)` | Version and build strings |
| `GetBotVersionEx(…, int32_t* BuildNumber, void* Reserved)` | Adds build number. The wrapper always passes `NULL` for `Reserved` and ignores your argument |
| `QueryConfigurationSetting(Setting, char* Profile, void* Buffer, uint32_t* BufferLength)` | Read a setting for a profile (list below). `BufferLength` is in/out |

```
int32_t ver = 0;
char buildTime[64]{}, buildDate[64]{}, authName[64]{};
const auto r = API.GetBotVersion(&ver, buildTime, buildDate, authName);
if (r == -1)      { /* host doesn't export it */ }
else if (r)       { /* ver and build strings are valid */ }

char profile[] = "Default";           // profile name (assumed)
char home[256]{};
uint32_t len = sizeof(home);          // in: buffer size, out: bytes written
if (API.QueryConfigurationSetting(BinaryChatAPI::Setup_Homechannel, profile, home, &len) == PluginAPI::True)
	API.WriteOutputString(home);
```

## ConfigurationSetting values

| # | Name | # | Name | # | Name |
| --- | --- | --- | --- | --- | --- |
| 0 | Username | 13 | LOD_CDkey | 26 | WindowOptions |
| 1 | Password | 14 | CDkeyName | 27 | ShowAdbanners |
| 2 | LastPing | 15 | Spawn | 28 | WindowSize |
| 3 | Master | 16 | Product | 29 | WindowXPos |
| 4 | Operator | 17 | ShowEncryptErrors | 30 | WindowYPos |
| 5 | IdleMessage | 18 | TelnetPort | 31 | Blank_Character |
| 6 | Homechannel | 19 | TelnetPassword | 32 | BotNetServer |
| 7 | Countidle | 20 | EnableRealm | 33 | BotNetServer_Pass |
| 8 | Log | 21 | RealmCharacter | 34 | BotNetDatabaseId |
| 9 | Timestamp | 22 | CharacterClass | 35 | ShowBotNetCmds |
| 10 | UserNotifications | 23 | InteractiveRealm | 36 | ConnectBattleNet |
| 11 | BattleNetServer | 24 | CharacterFlags |  |  |
| 12 | CDkey | 25 | PingSpoof |  |  |

All are in the `BinaryChatAPI` namespace with a `Setup_` prefix (e.g. `Setup_Homechannel`). Which type each returns (string vs integer) isn't stated in the headers.

## Types & enums

| Type | Values |
| --- | --- |
| `Bool32` | `int32_t`; `False = 0`, `True = 1` |
| `ModuleHandle / WindowHandle / MenuHandle / EventHandle` | `void*` stand-ins for HINSTANCE / HWND / HMENU / HANDLE |
| `ConnectionInterface` | `Connection_BNCS, Connection_MCP, Connection_GS, Connection_Count` |
| `ConnectionState` | `Connection_Closed, Connection_Opened` |
| `OutputColor` | White, Gray, Blue, Green, Red, Yellow, Purple, BlueGreen, MidBlue, DarkBlue, DarkGreen, DarkYellow, DarkRed, DarkPurple, LightBlue, `NumColors` |
| `BinaryChatWindow` | MainWindow, ChannelList, ChannelNameStatic, UserInput, ChatOutput, AccountManager, RealmManager, DownloadManager, NewsWindow, FiltersManager, ChannelsManager |
| `BinaryChatMenu` | `MainWindowMenu`, `ChannelUserPopupMenu` (actually the first submenu, not the parent) |
| `StateInformation` | `Size`, `ChannelName[256]`, `ChannelFlags`, `UniqueName[256]`, `AccountName[256]`, `Statstring[256]`, `State[Connection_Count]` (1044 bytes) |

## PluginInformation layout

Each revision derives from the previous, so a newer host's struct is a superset.

| Struct | Adds |
| --- | --- |
| `PluginInformation` (rev 1) | 12 API procs (`Net*`, `Sys*`, `UI*`), `BotNetClientInstance`, `BotNetConnection` (`BotNetConnectionState*`), the five hook/param pairs (writable), `PluginInstance`, `PluginEvent` |
| `PluginInformation2` | `SysProcessBotCommand` |
| `PluginInformation3` | `RegGetRegistryPath, NetResetAwayIdle, UIGetWindow, SysIsService, UIGetMenu` |
| `PluginInformation4` | `Flt*` filter procs, `UINotifyMsg`, `SysEnumLoadedPlugins` |

`BotNetClientInstance` and `BotNetConnection` are exposed by the struct, but the template doesn't use them.

## ABI guards

`BCPlugin.h` uses `static_assert`s to keep layouts identical to the original Win32 header on both architectures.

| Struct | x86 | x64 |
| --- | --- | --- |
| `PluginInformation` | 108 | 216 |
| `PluginInformation2` | 112 | 224 |
| `PluginInformation3` | 132 | 264 |
| `PluginInformation4` | 172 | 344 |

Also asserted: `Bool32` is 4 bytes, `intptr_t` is pointer-sized, all enums are 4 bytes, `StateInformation` is 1044 bytes.

## Gotchas

**Revision gating.** Calling a rev 2–4 method on an older host dereferences null. Always check `API.Revision()` (or `m_X` where applicable).

**Version API returns -1** when the export is missing; treat it as three states, not a bool.

**Calling conventions.** Hooks and API procs are `__stdcall`; `NotifyMsg` is `__cdecl` variadic. On x86 a mismatch corrupts the stack.

**Enum callbacks need the instance in lParam.** The wrappers pass `&PluginInstance` reinterpreted as `HookManager*`; this works because `Plugin` derives singly from `HookManager`.

**Disabled hooks aren't compiled or installed.** If you turn a hook on later, flip its macro in `PluginSettings.h`.

**Observed quirks in the template:** `GetBotVersionEx` ignores its `Reserved` argument; `QueueChatMessage` takes a mutable `char*`; the EnumChannelHook/Version proc fallbacks aren't null-guarded for rev 2–4 wrappers.

## Ideas for what plugins can do

- **Custom chat commands**: `CommandHook` + `QueueChatMessage` / `WriteOutputString`.
- **Packet sniffing and injection**: `SetMessage` + `MessageHook`, `SendConnectionMessage`.
- **Periodic jobs**: `PluginTimer` (e.g. polling channel users with `QueryChannelInformation`).
- **Cross-thread wakeups**: signal the `PluginEvent` handle to trigger `EventHook`.
- **Connection supervision**: `ConnectionHook` to react to disconnects or reconnect logic.
- **UI integration**: `GetWindow`, `GetMenu` to add menu items or subclass windows; `NotifyMsg` for colored notices.
- **Moderation**: user/message filter add/remove/enumerate, with `IsUserFiltered` checks.
- **Bot control**: `ProcessBotCommand`, `ResetAwayIdle`, `CloseConnection`.
- **Config-aware plugins**: `QueryConfigurationSetting` plus `GetRegistryPath` for per-profile storage.
- **Plugin cooperation**: `EnumLoadedPlugins`.
