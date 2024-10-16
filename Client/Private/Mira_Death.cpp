#include "stdafx.h"
#include "Mira_Death.h"


#include "Mira.h"
#include "GameInstance.h"
#include "Animation.h"


CMira_Death::CMira_Death(class CGameObject* pOwner)
	: CState{ CMira::MIRA_ANIMATION::DEATH , pOwner }
{

}


HRESULT CMira_Death::Initialize()
{
	return S_OK;
}


HRESULT CMira_Death::Start_State(void* pArg)
{
	_uint iRandom = m_pGameInstance->Get_Random_Interger(0, 1);
	switch (iRandom)
	{
	case 0:
		m_pGameInstance->Play_Sound(TEXT("MiraDeath1.ogg"), SOUND_MIRA, 3.f);
		m_pGameInstance->Play_Sound(TEXT("HitBlood2.ogg"), SOUND_MIRABLOOD, 3.f);
		break;
	case 1:
		m_pGameInstance->Play_Sound(TEXT("MiraDeath1.ogg"), SOUND_MIRA, 3.f);
		m_pGameInstance->Play_Sound(TEXT("HitBlood3.ogg"), SOUND_MIRABLOOD, 3.f);
		break;
	default:
		break;
	}

	return S_OK;
}


void CMira_Death::Update(_float fTimeDelta)
{
	static_cast<CMira*>(m_pOwner)->SetDead();
}


void CMira_Death::End_State()
{

}




CMira_Death* CMira_Death::Create(class CGameObject* pOwner)
{
	CMira_Death* pInstance = new CMira_Death(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMira_Death"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMira_Death::Free()
{
	__super::Free();
}
