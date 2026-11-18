// prvm_steam.h

#pragma once
#include "steam.h"

void VM_steam_achivment_unlock(prvm_prog_t *prog)
{
	const char *szString;
	int nCnt;
	VM_SAFEPARMCOUNT(1, VM_steam_achivment_unlock);
	szString = PRVM_G_STRING(OFS_PARM0);
	nCnt = Steam_Achivment_Unlock(szString);
	PRVM_G_FLOAT(OFS_RETURN) = nCnt;
}


void VM_steam_achivment_lock(prvm_prog_t *prog)
{
	const char *szString;
	int nCnt;
	VM_SAFEPARMCOUNT(1, VM_steam_achivment_lock);
	szString = PRVM_G_STRING(OFS_PARM0);
	nCnt = Steam_Achivment_Lock(szString);
	PRVM_G_FLOAT(OFS_RETURN) = nCnt;
}

//float steam_achievement_unlock(string s) = #645;
//float steam_achievement_lock(string s) = #646;
//void steam_open_url(string s) = #647;
void VM_steam_open_url(prvm_prog_t *prog)
{
	#ifdef CONFIG_MENU
		const char *szString;
		//int nCnt; Baker: unreferenced
		VM_SAFEPARMCOUNT(1, VM_steam_open_url);
		szString = PRVM_G_STRING(OFS_PARM0);
		Steam_OpenUrl(szString);
		//PRVM_G_FLOAT(OFS_RETURN) = nCnt;
	#endif // CONFIG_MENU
}