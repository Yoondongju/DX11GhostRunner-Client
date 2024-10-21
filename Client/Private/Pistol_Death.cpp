#include "stdafx.h"
#include "Pistol_Death.h"


#include "Pistol.h"
#include "GameInstance.h"

#include "Animation.h"

#include "KillCountUI.h"

CPistol_Death::CPistol_Death(class CGameObject* pOwner)
	: CState{ CPistol::PISTOL_ANIMATION::DEATH_1 , pOwner }
{

}

HRESULT CPistol_Death::Initialize()
{
	return S_OK;
}

HRESULT CPistol_Death::Start_State(void* pArg)
{
	_uint iRandom = m_pGameInstance->Get_Random_Interger(0, 1);
	switch (iRandom)
	{
	case 0:
		m_pGameInstance->Play_Sound(TEXT("PistolDeath1.ogg"), SOUND_PISTOL, 3.f);
		m_pGameInstance->Play_Sound(TEXT("HitBlood1.ogg"), SOUND_PISTOLBLOOD, 3.f);
		break;
	case 1:
		m_pGameInstance->Play_Sound(TEXT("PistolDeath1.ogg"), SOUND_PISTOL, 3.f);
		m_pGameInstance->Play_Sound(TEXT("HitBlood3.ogg"), SOUND_PISTOLBLOOD, 3.f);
		break;
	default:
		break;
	}

	static_cast<CKillCountUI*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, L"Layer_KillCount", 0))->Set_Active(true);

	return S_OK;
}

void CPistol_Death::Update(_float fTimeDelta)
{
	static_cast<CPistol*>(m_pOwner)->SetDead();
}

void CPistol_Death::End_State()
{

}


CPistol_Death* CPistol_Death::Create(class CGameObject* pOwner)
{
	CPistol_Death* pInstance = new CPistol_Death(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPistol_Death"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPistol_Death::Free()
{
	__super::Free();
}
