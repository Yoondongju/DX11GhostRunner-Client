#include "stdafx.h"
#include "Elite_Idle.h"


#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"


CElite_Idle::CElite_Idle(class CGameObject* pOwner)
	: CState{ CElite::ELITE_ANIMATION::IDLE , pOwner }
{

}

HRESULT CElite_Idle::Initialize()
{
	return S_OK;
}

HRESULT CElite_Idle::Start_State(void* pArg)
{


	return S_OK;
}

void CElite_Idle::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;





	if (Check_Detect())
	{
		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CElite::ELITE_ANIMATION::IDLE_TO_ALERT, true);
		pFsm->Change_State(CElite::ELITE_ANIMATION::IDLE_TO_ALERT);
		return;
	}

}

void CElite_Idle::End_State()
{

}

_bool CElite_Idle::Check_Detect()
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

_bool CElite_Idle::Check_Death()
{
	return _bool();
}




CElite_Idle* CElite_Idle::Create(class CGameObject* pOwner)
{
	CElite_Idle* pInstance = new CElite_Idle(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CElite_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElite_Idle::Free()
{
	__super::Free();
}
