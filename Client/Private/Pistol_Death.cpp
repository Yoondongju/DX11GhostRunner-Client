#include "stdafx.h"
#include "Pistol_Death.h"


#include "Pistol.h"
#include "GameInstance.h"

#include "Animation.h"

CPistol_Death::CPistol_Death(class CGameObject* pOwner)
	: CState{ CPistol::PISTOL_ANIMATION::DEATH_1 , pOwner }
{

}

HRESULT CPistol_Death::Initialize()
{
	return S_OK;
}

HRESULT CPistol_Death::Start_State()
{


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
