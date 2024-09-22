#include "stdafx.h"
#include "Elite_Death.h"


#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"


CElite_Death::CElite_Death(class CGameObject* pOwner)
	: CState{ CElite::ELITE_ANIMATION::DEATH_1 , pOwner }
{

}


HRESULT CElite_Death::Initialize()
{
	return S_OK;
}


HRESULT CElite_Death::Start_State(void* pArg)
{


	return S_OK;
}


void CElite_Death::Update(_float fTimeDelta)
{
	static_cast<CElite*>(m_pOwner)->SetDead();
}


void CElite_Death::End_State()
{

}




CElite_Death* CElite_Death::Create(class CGameObject* pOwner)
{
	CElite_Death* pInstance = new CElite_Death(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CElite_Death"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElite_Death::Free()
{
	__super::Free();
}
