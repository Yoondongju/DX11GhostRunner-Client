#include "stdafx.h"
#include "Player_Block.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_Block::CPlayer_Block(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::BLOCK_1 , pOwner }
{

}

HRESULT CPlayer_Block::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Block::Start_State()
{


	return S_OK;
}

void CPlayer_Block::Update(_float fTimeDelta)
{
}

void CPlayer_Block::End_State()
{

}


CPlayer_Block* CPlayer_Block::Create(class CGameObject* pOwner)
{
	CPlayer_Block* pInstance = new CPlayer_Block(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Block"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Block::Free()
{
	__super::Free();
}
