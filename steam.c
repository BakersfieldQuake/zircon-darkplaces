// steam.c

#include "darkplaces.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "steam.h"

#ifdef _WIN32
	#include <Windows.h>
#else
	// Baker: ??
#endif //

qbool g_steamInitialized = false;
//HMODULE SteamDLL;
dllhandle_t SteamDLL;
PFN_SB_Init     pSB_Init;
PFN_SB_Exec     pSB_Exec;
PFN_SB_Frame    pSB_Frame;
PFN_SB_Shutdown pSB_Shutdown;
PFN_SB_GetSteamIDStr pSB_GetSteamIDStr;
PFN_SB_Achivement_Unlock pSB_Achivement_Unlock;
PFN_SB_Achivement_Lock pSB_Achivement_Lock;

PFN_SB_SteamNet_Init         pSB_SteamNet_Init;
PFN_SB_SteamNet_ServerStop   pSB_SteamNet_ServerStop;
PFN_SB_SteamNet_ServerStart  pSB_SteamNet_ServerStart;
PFN_SB_SteamNet_ServerGetFakeIP_String pSB_SteamNet_ServerGetFakeIP_String;
PFN_SB_SteamNet_ClientDisconnect       pSB_SteamNet_ClientDisconnect;
PFN_SB_SteamNet_ClientConnectFakeIP    pSB_SteamNet_ClientConnectFakeIP;
PFN_SB_SteamNet_Shutdown               pSB_SteamNet_Shutdown;
PFN_SB_SteamNet_Read   pSB_SteamNet_Read;
PFN_SB_SteamNet_Write  pSB_SteamNet_Write;
PFN_SB_SteamNet_IsFakeIPAddr pSB_SteamNet_IsFakeIPAddr;
PFN_SB_SteamNet_InviteOverlay pSB_SteamNet_InviteOverlay;
PFN_SB_SteamNet_InviteFriendToCurrentServer pSB_SteamNet_InviteFriendToCurrentServer;
PFN_SB_RegisterEngineCmdExec pSB_RegisterEngineCmdExec;

static void SteamBridge_ExecFromDLL(const char *text)
{
	if (!text || !text[0]) return;
	Cbuf_AddTextLine(cmd_local, text); // KLESKBY - November 11 2025
	//Cmd_ExecuteString(cmd_local, text, src_local, false);
}

#if 0
static qbool LoadSteamFunc(void **out, const char *name)
{
	//*out = (void*)GetProcAddress(SteamDLL, name);
	*out = (void*)Sys_GetProcAddress(SteamDLL, name);
	if (!*out) {
		Con_PrintLinef(CON_ERROR "Steam: missing required symbol '%s'", name);
		return false;
	}
	return true;
}
#endif

static dllfunction_t steam_funcs[] =
{
	{"SB_Init", (void **)&pSB_Init},
	{"SB_Frame", (void **)&pSB_Frame},
	{"SB_Shutdown", (void **)&pSB_Shutdown},
	{"SB_GetSteamIDStr", (void **)&pSB_GetSteamIDStr},
	{"SB_RegisterEngineCmdExec", (void **)&pSB_RegisterEngineCmdExec},
	{"SB_Achivement_Unlock", (void **)&pSB_Achivement_Unlock},
	{"SB_Achivement_Lock", (void **)&pSB_Achivement_Lock},

	// networking
	{"SB_SteamNet_Init", (void **)&pSB_SteamNet_Init},
	{"SB_SteamNet_ServerStop", (void **)&pSB_SteamNet_ServerStop},
	{"SB_SteamNet_ServerStart", (void **)&pSB_SteamNet_ServerStart},
	{"SB_SteamNet_ServerGetFakeIP_String", (void **)&pSB_SteamNet_ServerGetFakeIP_String},
	{"SB_SteamNet_ClientDisconnect", (void **)&pSB_SteamNet_ClientDisconnect},
	{"SB_SteamNet_ClientConnectFakeIP", (void **)&pSB_SteamNet_ClientConnectFakeIP},
	{"SB_SteamNet_Shutdown", (void **)&pSB_SteamNet_Shutdown},
	{"SB_SteamNet_Read", (void **)&pSB_SteamNet_Read},
	{"SB_SteamNet_Write", (void **)&pSB_SteamNet_Write},
	{"SB_SteamNet_IsFakeIPAddr", (void **)&pSB_SteamNet_IsFakeIPAddr},

	// overlay / invites – probably optional
	{"SB_SteamNet_InviteOverlay", (void **)&pSB_SteamNet_InviteOverlay},
	{"SB_SteamNet_InviteFriendToCurrentServer", (void **)&pSB_SteamNet_InviteFriendToCurrentServer},

	{NULL, NULL}
};

CALLERS_ (NetConn_InitOnce Steam_Init)


void Steam_Frame()
{
	if (!g_steamInitialized) return;
	pSB_Frame(); //SteamAPI_RunCallbacks();
}

void Steam_Shutdown()
{
	if (!g_steamInitialized) return;
	pSB_Shutdown(); // SteamAPI_Shutdown();
	g_steamInitialized = false;
}

int Steam_Achivment_Unlock(const char *name)
{
	if (!name || !name[0]) return 0;
	if (!pSB_Achivement_Unlock) return 0;
	return pSB_Achivement_Unlock(name);
}

int Steam_Achivment_Lock(const char *name)
{
	if (!name || !name[0]) return 0;
	if (!pSB_Achivement_Lock) return 0;
	return pSB_Achivement_Lock(name);
}

	#ifdef _WIN32
		#include <shellapi.h>
	#endif //

void Steam_OpenUrl(const char *url)
{
#ifdef CONFIG_MENU
	/*qbool ok =*/ Sys_Open_URL(url);
#endif // CONFIG_MENU
}

// Baker: NetConn_InitOnce will call SteamNet_Init => Steam_Init
// before Host_InitOnce calls.  So this gets called twice.
qbool Steam_Init()
{
	if (g_steamInitialized) return true;

	if (!FS_SysFileExists("steam_appid.txt")) {
		// Baker: This will be normal with Quake .. probably shouldn't print
		Con_DPrintLinef("Steam: missing steam_appid.txt, skipping steam.");
		return false;
	}

	const char* steam_dllnames[] = {
#if defined(_WIN64)
		"steambridge64.dll",
#elif defined(_WIN32)
		"steambridge32.dll",
#elif defined(MACOSX)
		"steambridge64.dylib",
#else
		"steambridge64.so",
#endif
		NULL
	};

	RELATED_(oggfuncs)
	qbool ret = Sys_LoadDependency(steam_dllnames, &SteamDLL, steam_funcs);

	if (!ret) {
		// Baker: Unable to load steambridge64.dll or equivalent
		return false;
	}

	if (!pSB_Init) {
		// Baker: Make sure this initialized before trying to call it.
		return false;
	}

	if (!pSB_Init()) {
		Con_PrintLinef("Steam: Error initializing steam");
		return false;
	}

	if (pSB_RegisterEngineCmdExec)
		pSB_RegisterEngineCmdExec(SteamBridge_ExecFromDLL);

	Con_PrintLinef("Steam: successfully initialized");

	extern cvar_t net_sourceaddresscheck;
	Cvar_SetValueQuick (&net_sourceaddresscheck, 0);
	Con_DPrintLinef("Set net_sourceaddresscheck 0.");
	g_steamInitialized = true;

	return true;
} // Steam_Init


