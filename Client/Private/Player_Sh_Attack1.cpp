#include "stdafx.h"
#include "Player_Sh_Attack1.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"

#include "Animation.h"

CPlayer_Sh_Attack1::CPlayer_Sh_Attack1(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::SH_ATTACK , pOwner }
{

}

HRESULT CPlayer_Sh_Attack1::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_Sh_Attack1::Start_State()
{


	return S_OK;
}

void CPlayer_Sh_Attack1::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();

	
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();


	CWeapon_Player* pShuiKen = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));

	if (0.1f <= (TrackPos / Duration))
	{
		_vector vPlayerDir = static_cast<CPlayer*>(m_pOwner)->Get_Transform()->Get_State(CTransform::STATE_LOOK);

		pShuiKen->Set_Attacking(true);
		
		_float4x4* pComBindWMatrix = pShuiKen->Get_PartObjectComBindWorldMatrixPtr();
		_matrix ComBindMatrix = XMLoadFloat4x4(pComBindWMatrix);


		_vector vCompute_Result = XMVector3Normalize(vPlayerDir) * 100 * fTimeDelta;

		ComBindMatrix.r[3] += vCompute_Result;
	
		XMStoreFloat4x4(pComBindWMatrix, ComBindMatrix);

	}



	_bool bLBtnTap = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP;
	
	if (bLBtnTap)
	{
		if (m_fLastKeyPressTimeLBtn >= 0.f && (m_fAccTime - m_fLastKeyPressTimeLBtn <= 0.3f))
		{
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_ATTACK, true);
	
			m_fLastKeyPressTimeLBtn = -1.f;
		}
		m_fLastKeyPressTimeLBtn = m_fAccTime;
	}
	

	if (CPlayer::PLAYER_ANIMATIONID::SH_ATTACK == pModel->Get_CurAnimationIndex())
	{
		if (0.8f <= (TrackPos / Duration))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();

			_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
			TrackPos = 0.0;

			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_IDLE, true, 0.1f);
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
		}
	}

	if (Check_Jump())
		return;

 }

void CPlayer_Sh_Attack1::End_State()
{
	CWeapon_Player* pShuiKen = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));

	_float4x4 OriginMatrix =  pShuiKen->Get_OriginMatrix();
	pShuiKen->Get_Transform()->Set_WorldMatrix(OriginMatrix);

	pShuiKen->Set_Attacking(false);
}


_bool CPlayer_Sh_Attack1::Check_Jump()
{
	if (true == static_cast<CBody_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY))->IsLandingWall())
		return false;

	if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
	{
		CTransform* pTransform = m_pOwner->Get_Transform();
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

		_float fLandY = pPlayer->Get_LandPosY();
		_float fOffSetY = pPlayer->Get_OffsetY();
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


		if (fLandY + fOffSetY == XMVectorGetY(pTransform->Get_State(CTransform::STATE_POSITION)))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();


			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_START);
		}

		return true;
	}

	return false;
}

CPlayer_Sh_Attack1* CPlayer_Sh_Attack1::Create(class CGameObject* pOwner)
{
	CPlayer_Sh_Attack1* pInstance = new CPlayer_Sh_Attack1(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_Sh_Attack1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Sh_Attack1::Free()
{
	__super::Free();
}
