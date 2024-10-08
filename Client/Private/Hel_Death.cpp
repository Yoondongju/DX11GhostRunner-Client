#include "stdafx.h"
#include "Hel_Death.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"


CHel_Death::CHel_Death(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::HEAD_CUT_DEAD , pOwner }
{

}


HRESULT CHel_Death::Initialize()
{
	return S_OK;
}


HRESULT CHel_Death::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_Death::Update(_float fTimeDelta)
{
	static_cast<CHel*>(m_pOwner)->SetDead();
}


void CHel_Death::End_State()
{

}




CHel_Death* CHel_Death::Create(class CGameObject* pOwner)
{
	CHel_Death* pInstance = new CHel_Death(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Death"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Death::Free()
{
	__super::Free();
}
