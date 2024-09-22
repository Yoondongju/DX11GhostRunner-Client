#include "stdafx.h"
#include "Elite_Walk.h"


#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"


CElite_Walk::CElite_Walk(class CGameObject* pOwner)
	: CState{ CElite::ELITE_ANIMATION::WALK_F , pOwner }
{

}

HRESULT CElite_Walk::Initialize()
{
	return S_OK;
}

HRESULT CElite_Walk::Start_State(void* pArg)
{
	switch (m_eDir)
	{
	case Engine::CState::FRONT:
		break;
	case Engine::CState::BACK:
		break;
	case Engine::CState::LEFT:	
		break;
	case Engine::CState::RIGHT:		
		break;
	case Engine::CState::STATE_DIR_END:
		break;
	default:
		break;
	}


	m_fRetreatTime = m_pGameInstance->Get_Random(1.f, 2.f);


	return S_OK;
}

void CElite_Walk::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;

	CModel* pModel = static_cast<CElite*>(m_pOwner)->Get_Model();

	CTransform* pEliteTransform = m_pOwner->Get_Transform();

	
	if (m_fRetreatTime > 0.f)
	{
		m_fRetreatTime -= fTimeDelta;
		pEliteTransform->Go_Backward(fTimeDelta);
	}
	else
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CElite::ELITE_ANIMATION::IDLE_TO_ALERT, true);
		pFsm->Change_State(CElite::ELITE_ANIMATION::IDLE_TO_ALERT);
	}
		

}

void CElite_Walk::End_State()
{

}


_bool CElite_Walk::Check_Death()
{
	return _bool();
}




CElite_Walk* CElite_Walk::Create(class CGameObject* pOwner)
{
	CElite_Walk* pInstance = new CElite_Walk(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CElite_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElite_Walk::Free()
{
	__super::Free();
}
