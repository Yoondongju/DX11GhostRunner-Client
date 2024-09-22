#include "stdafx.h"
#include "Mira_Suiside.h"


#include "Mira.h"
#include "GameInstance.h"
#include "Animation.h"


CMira_Suiside::CMira_Suiside(class CGameObject* pOwner)
	: CState{ CMira::MIRA_ANIMATION::SUISIDE , pOwner }
{

}

HRESULT CMira_Suiside::Initialize()
{
	return S_OK;
}

HRESULT CMira_Suiside::Start_State(void* pArg)
{


	return S_OK;
}

void CMira_Suiside::Update(_float fTimeDelta)
{




}

void CMira_Suiside::End_State()
{

}




CMira_Suiside* CMira_Suiside::Create(class CGameObject* pOwner)
{
	CMira_Suiside* pInstance = new CMira_Suiside(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMira_Suiside"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMira_Suiside::Free()
{
	__super::Free();
}
