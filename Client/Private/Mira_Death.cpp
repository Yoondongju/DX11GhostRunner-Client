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


HRESULT CMira_Death::Start_State()
{


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
