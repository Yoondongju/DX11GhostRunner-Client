#include "../Public/SoundMager.h"
#include "../Public/Timer_Manager.h"

CSound_Manager::CSound_Manager()
	:CBase{}
{
}

HRESULT CSound_Manager::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	FMOD::System_Create(&m_pSystem);

	m_pSystem->init(MAXCHANNEL, FMOD_INIT_NORMAL, nullptr);

	if (m_pSystem == nullptr)
		return E_FAIL;

	LoadSoundFile();

	return S_OK;
}

void CSound_Manager::PlaySound(const TCHAR* pSoundKey, _uint eID, _float fVolume)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_Sounds.begin(), m_Sounds.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_Sounds.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[eID]);

	m_pChannelArr[eID]->setVolume(fVolume);

	m_pSystem->update();

	//if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	//{
	//	FMOD_System_PlaySound(m_pSystem, 0, iter->second, FALSE, &m_pChannelArr[eID]);
	//}

	//FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	//FMOD_System_Update(m_pSystem);
}

void CSound_Manager::PlaySound(const TCHAR* pSoundKey, _uint eID, _float* pVolume)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_Sounds.begin(), m_Sounds.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_Sounds.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[eID]);

	m_pChannelArr[eID]->setVolume(*pVolume);

	m_pSystem->update();
}

void CSound_Manager::Play_SoundRepeat(const TCHAR* pSoundKey, _uint eID, _float fVolume)
{
	if (!Check_IsPlaying(eID))
	{
		PlaySound(pSoundKey, eID, fVolume);
	}
}

void CSound_Manager::PlayBGM(const TCHAR* pSoundKey, _float fVolume)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_Sounds.begin(), m_Sounds.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_Sounds.end())
		return;

	//FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[SOUND_BGM]);
	m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[0]);

	//FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	m_pChannelArr[0]->setMode(FMOD_LOOP_NORMAL);

	//FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	m_pChannelArr[0]->setVolume(fVolume);

	//FMOD_System_Update(m_pSystem);
	m_pSystem->update();
}

void CSound_Manager::StopSound(_uint eID)
{
	//FMOD_Channel_Stop(m_pChannelArr[eID]);
	m_pChannelArr[eID]->stop();
}

void CSound_Manager::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		m_pChannelArr[i]->stop();
}

void CSound_Manager::SetChannelVolume(_uint eID, _float fVolume)
{
	//FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
	m_pChannelArr[eID]->setVolume(fVolume);

	//FMOD_System_Update(m_pSystem);
	m_pSystem->update();
}

void CSound_Manager::SetPlayeSpeed(_uint eID, _float fSpeedRatio)
{
	_float fCurrentFrequency = {};
	m_pChannelArr[eID]->getFrequency(&fCurrentFrequency);

	//44100.f
	m_pChannelArr[eID]->setFrequency(fCurrentFrequency * fSpeedRatio);

	m_pSystem->update();
}

void CSound_Manager::Pause(_uint eID)
{
	_bool bIsPause;
	m_pChannelArr[eID]->getPaused(&bIsPause);

	m_pChannelArr[eID]->setPaused(bIsPause);
}

_bool CSound_Manager::Check_IsPlaying(_uint eID)
{
	_bool isPlay;

	m_pChannelArr[eID]->isPlaying(&isPlay);

	return isPlay;
}

void CSound_Manager::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst("../Bin/Ghostrunner/Sound/*", &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "../Bin/Ghostrunner/Sound/";	 // 상대 경로
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);

		//FMOD_SOUND* pSound = nullptr;
		FMOD::Sound* pSound = nullptr;

		//FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, 0, &pSound);
		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			_int iLength = _int(strlen(fd.name) + 1);

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_Sounds.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	m_pSystem->update();

	_findclose(handle);
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CSound_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	for (auto& Pair : m_Sounds)
	{
		delete[] Pair.first;

		//FMOD_Sound_Release(Mypair.second);
		Pair.second->release();
	}
	m_Sounds.clear();

	m_pSystem->release();
	m_pSystem->close();
}
