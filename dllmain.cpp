#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "includes\injector\injector.hpp"
#include <cstdint>

float LeftGroupX, LeftGroupY, RightGroupX, RightGroupY;

void(__cdecl *FE_Object_SetCenter)(DWORD* FEObject, float _PositionX, float _PositionY) = (void(__cdecl*)(DWORD*, float, float))0x5B4980;
void(__cdecl *FE_Object_GetCenter)(DWORD* FEObject, float *PositionX, float *PositionY) = (void(__cdecl*)(DWORD*, float*, float*))0x5BDCF0;
void*(__cdecl *FEObject_FindObject)(const char *pkg_name, unsigned int obj_hash) = (void*(__cdecl*)(const char*, unsigned int))0x5BD520;

int __stdcall cFEng_QueuePackageMessage_Hook(unsigned int MessageHash, char const *FEPackageName, DWORD* FEObject)
{
	int ResX, ResY;
	float Difference;
	
	ResX = *(int*)0xBBAE8C;
	ResY = *(int*)0xBBAE90;
	
	if (ResX != 0 && ResY != 0) Difference = (320.0f * (((float)ResX / (float)ResY) * 0.75f)) - 320.f; // Calculate position difference for current aspect ratio
	else Difference = 0; // 4:3 if we can't get any values

	/*injector::WriteMemory<float*>(0x5D52FB, &Difference, true);
	injector::WriteMemory<float*>(0x5D5358, &Difference, true);*/

	DWORD* LeftGroup = (DWORD*)FEObject_FindObject(FEPackageName, 0x1603009E); // "HUD_SingleRace.fng", leftgrouphash

	if (LeftGroup) // Move left group
	{
		FE_Object_GetCenter(LeftGroup, &LeftGroupX, &LeftGroupY);
		FE_Object_SetCenter(LeftGroup, LeftGroupX - Difference, LeftGroupY);
	}

	DWORD* RightGroup = (DWORD*)FEObject_FindObject(FEPackageName, 0x5D0101F1); // "HUD_SingleRace.fng", rightgrouphash

	if (RightGroup) // Move right group
	{
		FE_Object_GetCenter(RightGroup, &RightGroupX, &RightGroupY);
		FE_Object_SetCenter(RightGroup, RightGroupX + Difference, RightGroupY);
	}

	return 1;
}

void Init()
{
	injector::MakeCALL(0x612ED4, cFEng_QueuePackageMessage_Hook, true);
	injector::MakeCALL(0x612EEE, cFEng_QueuePackageMessage_Hook, true);
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		Init();
	}
	return TRUE;

}