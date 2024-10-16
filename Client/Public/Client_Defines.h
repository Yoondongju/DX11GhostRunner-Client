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
	enum SOUND_ID 
	{	SOUND_BGM, 
		SOUND_PLAYER, 
		SOUND_PLAYER_HOOK,
		SOUND_PLAYER_DASH,
		SOUND_PLAYERVOICE,
		SOUND_PLAYEREFFECT,

		SOUND_PISTOL, 
		SOUND_PISTOLBLOOD,

		SOUND_SNIPER, 
		SOUND_SNIPERBLOOD,

		SOUND_MIRA, 
		SOUND_MIRABLOOD,

		SOUND_JETPACK, 
		SOUND_JETPACKBLOOD,

		SOUND_ENEMYEFFECT,

		SOUND_ELITE, 
		SOUND_HEL, 
		 

		SOUND_TARGET_UI,
		SOUND_FINALTARGET_UI,
		SOUND_END 
	};	//BGM은 항상 0번

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
extern float g_fBgmVolume;
extern float g_fEffectVolume;

using namespace Client;
using namespace std;
