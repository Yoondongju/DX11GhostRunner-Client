#include "stdafx.h"
#include "Jetpack_Attack.h"


#include "Jetpack.h"
#include "GameInstance.h"
#include "Animation.h"


CJetpack_Attack::CJetpack_Attack(class CGameObject* pOwner)
	: CState{ CJetpack::JETPACK_ANIMATION::ATTACK_1 , pOwner }
{

}

HRESULT CJetpack_Attack::Initialize()
{
	return S_OK;
}

HRESULT CJetpack_Attack::Start_State()
{


	return S_OK;
}

void CJetpack_Attack::Update(_float fTimeDelta)
{
	CTransform* pPlayerTransform = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform();
	CTransform* pJetpackTransform = m_pOwner->Get_Transform();

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	pJetpackTransform->LookAt(vPlayerPos);


	CModel* pModel = m_pOwner->Get_Model();

	if (CJetpack::JETPACK_ANIMATION::ATTACK_1 == pModel->Get_CurAnimationIndex() ||
		CJetpack::JETPACK_ANIMATION::ATTACK_2 == pModel->Get_CurAnimationIndex())
	{
		_double Duration = pModel->Get_CurAnimation()->Get_Duration();
		const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

		if (0.8f <= (TrackPos / Duration))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();

			_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
			TrackPos = 0.0;

			pModel->SetUp_Animation(CJetpack::JETPACK_ANIMATION::IDLE, true);
			pFsm->Change_State(CJetpack::JETPACK_ANIMATION::IDLE);
		}
	}
}

void CJetpack_Attack::End_State()
{

}


CJetpack_Attack* CJetpack_Attack::Create(class CGameObject* pOwner)
{
	CJetpack_Attack* pInstance = new CJetpack_Attack(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CJetpack_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJetpack_Attack::Free()
{
	__super::Free();
}
