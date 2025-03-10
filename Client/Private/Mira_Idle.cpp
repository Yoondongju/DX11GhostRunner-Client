#include "stdafx.h"
#include "Mira_Idle.h"


#include "Mira.h"
#include "GameInstance.h"
#include "Animation.h"


CMira_Idle::CMira_Idle(class CGameObject* pOwner)
	: CState{ CMira::MIRA_ANIMATION::IDLE , pOwner }
{

}

HRESULT CMira_Idle::Initialize()
{
	return S_OK;
}

HRESULT CMira_Idle::Start_State(void* pArg)
{


	return S_OK;
}

void CMira_Idle::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;

	if (Check_MindControling())
		return;


	if (true == static_cast<CEnemy*>(m_pOwner)->IsMindControlReady())
		return;



	if (Check_Detect())
	{
		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CMira::MIRA_ANIMATION::ATTACK, true);
		pFsm->Change_State(CMira::MIRA_ANIMATION::ATTACK);

		static_cast<CEnemy*>(m_pOwner)->Set_FindPlayer(true);
		return;
	}

}

void CMira_Idle::End_State()
{

}

_bool CMira_Idle::Check_Detect()
{
	_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vSniperPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_float  fPlayerX = XMVectorGetX(vPlayerPos);
	_float  fPlayerZ = XMVectorGetZ(vPlayerPos);

	_float  fSniperX = XMVectorGetX(vSniperPos);
	_float  fSniperZ = XMVectorGetZ(vSniperPos);


	_float fDistanceSquar = (fPlayerX - fSniperX) * (fPlayerX - fSniperX) + (fPlayerZ - fSniperZ) * (fPlayerZ - fSniperZ);


	return (fDistanceSquar <= (m_fDetectRadius * m_fDetectRadius));
}

_bool CMira_Idle::Check_Death()
{
	return _bool();
}


_bool CMira_Idle::Check_MindControling()
{
	CMira* pMira = static_cast<CMira*>(m_pOwner);
	if (false == pMira->IsMindControling())
		return false;

	
	CModel* pModel = m_pOwner->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();

	
	_bool	isMindContorling = true;

	pModel->SetUp_Animation(CMira::MIRA_ANIMATION::ATTACK, true);
	pFsm->Change_State(CMira::MIRA_ANIMATION::ATTACK, -1 , &isMindContorling);

	return true;
}


CMira_Idle* CMira_Idle::Create(class CGameObject* pOwner)
{
	CMira_Idle* pInstance = new CMira_Idle(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMira_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMira_Idle::Free()
{
	__super::Free();
}
