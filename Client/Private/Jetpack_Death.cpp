#include "stdafx.h"
#include "Jetpack_Death.h"


#include "Jetpack.h"
#include "GameInstance.h"
#include "Animation.h"


CJetpack_Death::CJetpack_Death(class CGameObject* pOwner)
	: CState{ CJetpack::JETPACK_ANIMATION::FLY_FALL , pOwner }
{

}

HRESULT CJetpack_Death::Initialize()
{
	return S_OK;
}

HRESULT CJetpack_Death::Start_State()
{


	return S_OK;
}

void CJetpack_Death::Update(_float fTimeDelta)
{
	CModel* pModel = m_pOwner->Get_Model();

	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	if (0.9 <= TrackPos / Duration)
	{
		static_cast<CJetpack*>(m_pOwner)->SetDead();
	}
}

void CJetpack_Death::End_State()
{

}


CJetpack_Death* CJetpack_Death::Create(class CGameObject* pOwner)
{
	CJetpack_Death* pInstance = new CJetpack_Death(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CJetpack_Death"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJetpack_Death::Free()
{
	__super::Free();
}
