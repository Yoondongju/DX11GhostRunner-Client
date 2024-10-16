#include "stdafx.h"
#include "Hel_Sprint.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

#include "PartObject.h"

CHel_Sprint::CHel_Sprint(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::SPRINT , pOwner }
{

}

HRESULT CHel_Sprint::Initialize()
{
	m_fAttackRange = 40.f;

	return S_OK;
}

HRESULT CHel_Sprint::Start_State(void* pArg)
{
	

	return S_OK;
}


void CHel_Sprint::Update(_float fTimeDelta)
{
	CTransform* pHelTransform = m_pOwner->Get_Transform();

	_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vHelPos = pHelTransform->Get_State(CTransform::STATE_POSITION);
	
	vPlayerPos = XMVectorSetY(vPlayerPos, 0.f);
	vHelPos = XMVectorSetY(vHelPos, 0.f);

	_float fCurDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vHelPos));

	if (m_fAttackRange < fCurDistance)
	{
		pHelTransform->LookAt_XZ(vPlayerPos);
		pHelTransform->Go_Straight(fTimeDelta * 30.f);
	}
	else
	{
		// 그냥 기본공격, 뒤로 대쉬한번갈기고 대쉬공격, 
		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();


		_int iRandom = m_pGameInstance->Get_Random_Interger(0, 1);  
		switch (iRandom)
		{
		case 0:
		{
			pModel->SetUp_Animation(CHel::HEL_ANIMATION::ATTACK3, true);
			pFsm->Change_State(CHel::HEL_ANIMATION::ATTACK1);
		}
		break;
		case 1:
		{
			pModel->SetUp_Animation(CHel::HEL_ANIMATION::DASH_BACK ,true);
			pFsm->Change_State(CHel::HEL_ANIMATION::DASH_BACK);
		}
		break;
		default:
			break;
		}

		return;
	}
	
}



void CHel_Sprint::End_State()
{

}


CHel_Sprint* CHel_Sprint::Create(class CGameObject* pOwner)
{
	CHel_Sprint* pInstance = new CHel_Sprint(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Sprint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Sprint::Free()
{
	__super::Free();
}
