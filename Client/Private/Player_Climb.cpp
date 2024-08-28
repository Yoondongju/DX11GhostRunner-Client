#include "stdafx.h"
#include "Player_Climb.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_Climb::CPlayer_Climb(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::CLIMB , pOwner }
{

}

HRESULT CPlayer_Climb::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Climb::Start_State()
{

	return S_OK;
}

void CPlayer_Climb::Update(_float fTimeDelta)
{
}

void CPlayer_Climb::End_State()
{

}


CPlayer_Climb* CPlayer_Climb::Create(class CGameObject* pOwner)
{
	CPlayer_Climb* pInstance = new CPlayer_Climb(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Climb"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Climb::Free()
{
	__super::Free();
}
