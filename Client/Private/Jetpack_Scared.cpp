#include "stdafx.h"
#include "Jetpack_Scared.h"


#include "Jetpack.h"
#include "GameInstance.h"
#include "Animation.h"


CJetpack_Scared::CJetpack_Scared(class CGameObject* pOwner)
	: CState{ CJetpack::JETPACK_ANIMATION::SCARED , pOwner }
{

}

HRESULT CJetpack_Scared::Initialize()
{
	return S_OK;
}

HRESULT CJetpack_Scared::Start_State(void* pArg)
{


	return S_OK;
}

void CJetpack_Scared::Update(_float fTimeDelta)
{



}

void CJetpack_Scared::End_State()
{

}


CJetpack_Scared* CJetpack_Scared::Create(class CGameObject* pOwner)
{
	CJetpack_Scared* pInstance = new CJetpack_Scared(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CJetpack_Scared"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJetpack_Scared::Free()
{
	__super::Free();
}
