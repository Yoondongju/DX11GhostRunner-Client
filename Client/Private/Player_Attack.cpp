#include "stdafx.h"
#include "Player_Attack.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_Attack::CPlayer_Attack(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::ATTACK_1 , pOwner }
{

}

HRESULT CPlayer_Attack::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Attack::Start_State()
{


	return S_OK;
}

void CPlayer_Attack::Update(_float fTimeDelta)
{
}

void CPlayer_Attack::End_State()
{

}


CPlayer_Attack* CPlayer_Attack::Create(class CGameObject* pOwner)
{
	CPlayer_Attack* pInstance = new CPlayer_Attack(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Attack::Free()
{
	__super::Free();
}
