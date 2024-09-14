#include "stdafx.h"
#include "Sniper_Death.h"


#include "Sniper.h"
#include "GameInstance.h"
#include "Animation.h"

CSniper_Death::CSniper_Death(class CGameObject* pOwner)
	: CState{ CSniper::SNIPER_ANIMATION::DEATH_1 , pOwner }
{

}

HRESULT CSniper_Death::Initialize()
{
	return S_OK;
}

HRESULT CSniper_Death::Start_State()
{


	return S_OK;
}

void CSniper_Death::Update(_float fTimeDelta)
{
	static_cast<CSniper*>(m_pOwner)->SetDead();
}

void CSniper_Death::End_State()
{

}


CSniper_Death* CSniper_Death::Create(class CGameObject* pOwner)
{
	CSniper_Death* pInstance = new CSniper_Death(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CSniper_Death"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSniper_Death::Free()
{
	__super::Free();
}
