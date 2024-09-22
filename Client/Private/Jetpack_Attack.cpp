#include "stdafx.h"
#include "Jetpack_Attack.h"


#include "Jetpack.h"
#include "Weapon_Jetpack.h"

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

HRESULT CJetpack_Attack::Start_State(void* pArg)
{


	return S_OK;
}

void CJetpack_Attack::Update(_float fTimeDelta)
{
	CTransform* pPlayerTransform = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform();
	CTransform* pJetpackTransform = m_pOwner->Get_Transform();

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	pJetpackTransform->LookAt(vPlayerPos , nullptr);


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

		if (0.1f <= (TrackPos / Duration))
		{
			CWeapon_Jetpack* JetpackWeapon = static_cast<CWeapon_Jetpack*>(static_cast<CJetpack*>(m_pOwner)->Get_Part(CJetpack::PARTID::PART_WEAPON));

			JetpackWeapon->Set_Attacking(true);

			_float4x4* pWorldMatrix = JetpackWeapon->Get_PartObjectComBindWorldMatrixPtr();


			_matrix ComBindMatrix = XMLoadFloat4x4(pWorldMatrix);

			_vector vLookNor = XMVector3Normalize(pJetpackTransform->Get_State(CTransform::STATE_LOOK));
			vLookNor = XMVectorSetY(vLookNor, XMVectorGetY(vLookNor) - 0.2f);  // 0.2f 만큼 아래로 내림

			_vector vCompute_Result = (vLookNor) * 300.f * fTimeDelta;

			ComBindMatrix.r[3] += vCompute_Result;
			XMStoreFloat4x4(pWorldMatrix, ComBindMatrix);
		}

	}


	



}

void CJetpack_Attack::End_State()
{
	CWeapon_Jetpack* JetpackWeapon = static_cast<CWeapon_Jetpack*>(static_cast<CJetpack*>(m_pOwner)->Get_Part(CJetpack::PARTID::PART_WEAPON));

	_float4x4 OriginMatrix = JetpackWeapon->Get_OriginMatrix();
	JetpackWeapon->Get_Transform()->Set_WorldMatrix(OriginMatrix);

	JetpackWeapon->Set_Attacking(false);
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
