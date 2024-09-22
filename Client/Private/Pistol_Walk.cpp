#include "stdafx.h"
#include "Pistol_Walk.h"


#include "Pistol.h"
#include "GameInstance.h"


CPistol_Walk::CPistol_Walk(class CGameObject* pOwner)
	: CState{ CPistol::PISTOL_ANIMATION::WALK_F , pOwner }
{

}

HRESULT CPistol_Walk::Initialize()
{
	return S_OK;
}

HRESULT CPistol_Walk::Start_State(void* pArg)
{

	return S_OK;
}

void CPistol_Walk::Update(_float fTimeDelta)
{

	if (Check_Attack(fTimeDelta))
		return;

}

void CPistol_Walk::End_State()
{

}


_bool CPistol_Walk::Check_Attack(_float fTimeDelta)
{
	CTransform* pOwnerTransform = m_pOwner->Get_Transform();
	CTransform* pPlayerTransform = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform();


	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPistolPos = pOwnerTransform->Get_State(CTransform::STATE_POSITION);

	CFsm* pFsm = m_pOwner->Get_Fsm();
	CModel* pModel = m_pOwner->Get_Model();

	if (m_fCanAttackDistance >= fabs(XMVectorGetX(vPlayerPos) - XMVectorGetX(vPistolPos)) &&
		m_fCanAttackDistance >= fabs(XMVectorGetZ(vPlayerPos) - XMVectorGetZ(vPistolPos)))
	{
		pModel->SetUp_Animation(CPistol::PISTOL_ANIMATION::ATTACK, true);
		pFsm->Change_State(CPistol::PISTOL_ANIMATION::ATTACK);
		return true;
	}
	else
	{
		pOwnerTransform->LookAt_XZ(vPlayerPos);
		pOwnerTransform->Go_Straight(fTimeDelta * 1.5f);

		if (m_fCanAttackDistance * 2 < fabs(XMVectorGetX(vPlayerPos) - XMVectorGetX(vPistolPos)) &&
			m_fCanAttackDistance * 2 < fabs(XMVectorGetZ(vPlayerPos) - XMVectorGetZ(vPistolPos)))
		{
			pModel->SetUp_Animation(CPistol::PISTOL_ANIMATION::IDLE_2, true);
			pFsm->Change_State(CPistol::PISTOL_ANIMATION::IDLE_1);
			return true;
		}
	}

	return false;
}

CPistol_Walk* CPistol_Walk::Create(class CGameObject* pOwner)
{
	CPistol_Walk* pInstance = new CPistol_Walk(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPistol_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPistol_Walk::Free()
{
	__super::Free();
}
