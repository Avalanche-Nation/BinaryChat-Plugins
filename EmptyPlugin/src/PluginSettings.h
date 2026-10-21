#ifndef _PLUGINSETTINGS_H_INCLUDED
#define _PLUGINSETTINGS_H_INCLUDED
#pragma once

// TRUE/FALSE settings for hooks being enabled/disabled
// Change each to false if it is not used, for optimization

#define MESSAGEHOOK_ENABLED		TRUE
#define CONNECTIONHOOK_ENABLED	TRUE
#define PLUGINTIMER_ENABLED		TRUE
#define COMMANDHOOK_ENABLED		TRUE
#define EVENTHOOK_ENABLED		TRUE

// TRUE/FALSE setting for BinaryChat Version API being enabled

#define BINARYCHATAPI_ENABLED	TRUE

// Highest and lowest revisions allowed
// Lowest revision: Set this value to the lowest revision number allowed by the plugin.
// If the BinaryChat revision is lower than this, the plugin won't be loaded.
// Highest revision: Set this value to the highest revision number used by the plugin (which
// isn't necessarily the same as the highest revision number supported by class BinaryChatAPI)
// for optimization.
// API.Revision() is guaranteed to return a number between LOWEST_REVISION and HIGHEST_REVISION.

#define LOWEST_REVISION			1
#define HIGHEST_REVISION		4

// Global object declarations
extern class Plugin PluginInstance;
extern class BinaryChatAPIManager API;

#endif // _PLUGINSETTINGS_H_INCLUDED