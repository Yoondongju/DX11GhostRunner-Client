#pragma once
#include "Base.h"
#include "../Public/fmod.h"
#include "../Public/fmod.hpp"
#include <io.h>

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)

class ENGINE_DLL CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();

public:
	void PlaySound(const TCHAR * pSoundKey, _uint eID, _float fVolume);				// 그냥 소리재생인듯   (반복 X )
	void Play_SoundRepeat(const TCHAR * pSoundKey, _uint eID, _float fVolume);		// 그냥 소리재생인듯   (반복 O )
	void PlayBGM(const TCHAR * pSoundKey, _float fVolume);							// 배경음악
	void StopSound(_uint eID);
	void StopAll();
	void SetChannelVolume(_uint eID, _float fVolume);
	void SetPlayeSpeed(_uint eID, _float fSpeedRatio);
	void  Pause(_uint eID);
	_bool Check_IsPlaying(_uint eID);

private:
	void LoadSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD::Sound*> m_Sounds;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	enum { MAXCHANNEL = 32 }; //임시로 잡아둠 나중에 고치기
	FMOD::Channel* m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	//FMOD::System* m_pSystem;
	FMOD::System* m_pSystem = { nullptr };

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END