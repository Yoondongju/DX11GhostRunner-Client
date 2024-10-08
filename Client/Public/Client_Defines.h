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

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_STAGE1 , LEVEL_STAGE1_BOSS , LEVEL_STAGE2_BOSS, LEVEL_END };

	enum MODEL_CHECK_LIST {
		KIT,
		DEBRIS,
		BARRICADE,
		BILLBOARD,
		BOSS_FIGHT1,
		BOSS_FIGHT3,
		CATWALK,
		CRANE,
		CYBERCITY,
		DECO,
		FAN,
		INDUSTRIAL_ELEVATOR,
		LAMP,
		PIPE,
		PLATFORM,
		ROTATIONFAN,
		TRAIN,

		SIGN,
		WALL,
		RUSSIAN_SIGN,
		CITY,
		BUILDING,
		WIRE,
		BODYBAG,
		CLIMBOBJECT,


		SPIDER,
		ELITE,
		JETPACK,
		MIRA,
		PISTOL,
		SNIPER,
		HEL,

		ETC,
		MODEL_CHECK_TYPE_END
	};

}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

extern int g_CurLevel;

using namespace Client;
using namespace std;
