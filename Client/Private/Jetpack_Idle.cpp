#include "stdafx.h"
#include "Jetpack_Idle.h"


#include "Jetpack.h"
#include "GameInstance.h"
#include "Animation.h"


CJetpack_Idle::CJetpack_Idle(class CGameObject* pOwner)
	: CState{ CJetpack::JETPACK_ANIMATION::IDLE , pOwner }
{

}

HRESULT CJetpack_Idle::Initialize()
{
	return S_OK;
}

HRESULT CJetpack_Idle::Start_State(void* pArg)
{


	return S_OK;
}

void CJetpack_Idle::Update(_float fTimeDelta)
{
	_float fPosZ = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION).m128_f32[2];

	if (5715.f <= fPosZ)
	{
		m_fAccTime += fTimeDelta;

		if (m_fAccTime >= 2.f)
		{
			if (Check_Fly())
				return;
		
			m_fAccTime = 0.f;
		}
	}



	

}

void CJetpack_Idle::End_State()
{

}


_bool CJetpack_Idle::Check_Fly()
{
	CFsm* pFsm = m_pOwner->Get_Fsm();
	CModel* pModel = m_pOwner->Get_Model();


	pModel->SetUp_Animation(CJetpack::JETPACK_ANIMATION::FLY_F, true);
	pFsm->Change_State(CJetpack::JETPACK_ANIMATION::FLY_F);

	return  true;
}

CJetpack_Idle* CJetpack_Idle::Create(class CGameObject* pOwner)
{
	CJetpack_Idle* pInstance = new CJetpack_Idle(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CJetpack_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJetpack_Idle::Free()
{
	__super::Free();
}
