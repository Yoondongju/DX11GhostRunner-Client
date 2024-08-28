#pragma once

#include <process.h>

#pragma comment(lib, "PhysXCooking_64.lib")
#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PVDRuntime_64.lib")

namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
	enum OBJECT_CHECK_TYPE { KIT, DEBRIS, SIGN, WALL, RUSSIAN_SIGN, CITY, BUILDING, MONSTER, ETC, OBJECT_CHECK_TYPE_END };

}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
using namespace std;
