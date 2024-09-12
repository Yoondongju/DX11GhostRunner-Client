#include "stdafx.h"
#include "Sniper_Attack.h"


#include "Sniper.h"
#include "GameInstance.h"


CSniper_Attack::CSniper_Attack(class CGameObject* pOwner)
	: CState{ CSniper::SNIPER_ANIMATION::ATTACK , pOwner }
{

}

HRESULT CSniper_Attack::Initialize()
{
	return S_OK;
}

HRESULT CSniper_Attack::Start_State()
{
	CTransform* pOwnerTransform = m_pOwner->Get_Transform();

	XMStoreFloat3(&m_OriginRight, pOwnerTransform->Get_State(CTransform::STATE_RIGHT));
	XMStoreFloat3(&m_OriginUp, pOwnerTransform->Get_State(CTransform::STATE_UP));
	XMStoreFloat3(&m_OriginLook, pOwnerTransform->Get_State(CTransform::STATE_LOOK));


	return S_OK;
}

void CSniper_Attack::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;


	CTransform* pOwnerTransform = m_pOwner->Get_Transform();
	CTransform* pCamTransform = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform();


	_vector vCamPos = pCamTransform->Get_State(CTransform::STATE_POSITION);
	pOwnerTransform->LookAt_XZ(vCamPos);



	if (Check_DetectOut())
	{
		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pOwnerTransform->Set_State(CTransform::STATE_RIGHT, XMLoadFloat3(&m_OriginRight));
		pOwnerTransform->Set_State(CTransform::STATE_UP, XMLoadFloat3(&m_OriginUp));
		pOwnerTransform->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_OriginLook));


		pModel->SetUp_Animation(CSniper::SNIPER_ANIMATION::IDLE, true);
		pFsm->Change_State(CSniper::SNIPER_ANIMATION::IDLE);
		return;
	}
}

void CSniper_Attack::End_State()
{

}

_bool CSniper_Attack::Check_DetectOut()
{
	_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vSniperPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_float  fPlayerX = XMVectorGetX(vPlayerPos);
	_float  fPlayerZ = XMVectorGetZ(vPlayerPos);

	_float  fSniperX = XMVectorGetX(vSniperPos);
	_float  fSniperZ = XMVectorGetZ(vSniperPos);


	_float fDistanceSquar = (fPlayerX - fSniperX) * (fPlayerX - fSniperX) + (fPlayerZ - fSniperZ) * (fPlayerZ - fSniperZ);


	return (fDistanceSquar > (m_fDetectRadius * m_fDetectRadius));
}

_bool CSniper_Attack::Check_Death()
{
	return _bool();
}


CSniper_Attack* CSniper_Attack::Create(class CGameObject* pOwner)
{
	CSniper_Attack* pInstance = new CSniper_Attack(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CSniper_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSniper_Attack::Free()
{
	__super::Free();
}
