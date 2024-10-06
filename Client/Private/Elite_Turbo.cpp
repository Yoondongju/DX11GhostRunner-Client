#include "stdafx.h"
#include "Elite_Turbo.h"


#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"

#include "Weapon_Elite.h"
#include "EliteSwordTrail.h"

CElite_Turbo::CElite_Turbo(class CGameObject* pOwner)
	: CState{ CElite::ELITE_ANIMATION::ALERT_TO_TURBO , pOwner }
{

}

HRESULT CElite_Turbo::Initialize()
{
	return S_OK;
}

HRESULT CElite_Turbo::Start_State(void* pArg)
{
	CModel* pModel = m_pOwner->Get_Model();
	pModel->SetUp_Animation(CElite::ELITE_ANIMATION::ALERT_TO_TURBO, true);

	CEliteSwordTrail* pSwordTrail = static_cast<CWeapon_Elite*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CElite::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);
	pSwordTrail->Set_Active(true);

	// �÷��̾ �������� �ٷ� ������� ó������
	// �и��� ��������

	return S_OK;
}

void CElite_Turbo::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;

	_float fSpeed = 1.f;
	CElite* pElite = static_cast<CElite*>(m_pOwner);
	if (true == pElite->IsPage2())
		fSpeed = 1.6f;

	if (true == m_bStartTurboDash && Check_TurboDash(fTimeDelta * fSpeed))
		return;


	CModel* pModel = static_cast<CElite*>(m_pOwner)->Get_Model();

	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();


	if (CElite::ELITE_ANIMATION::ALERT_TO_TURBO == pModel->Get_CurAnimationIndex() &&
		0.9 <= TrackPos / (_float)Duration)
	{		
		pModel->SetUp_Animation(CElite::ELITE_ANIMATION::TURBO_LOOP, true);
	}
	else if (CElite::ELITE_ANIMATION::TURBO_LOOP == pModel->Get_CurAnimationIndex())
	{
		if (XMVector3Equal(XMVectorZero(), XMLoadFloat3(&m_RushStartPos)))
		{
			_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);

			CTransform* pEliteTransform = m_pOwner->Get_Transform();
			_vector vElitePos = pEliteTransform->Get_State(CTransform::STATE_POSITION);

			m_bStartTurboDash = true;
			XMStoreFloat3(&m_RushStartPos, vElitePos);
		}
	}

	// �ͺ� �����Ҷ� �������� �ϰ� �ͺ������϶�

	else if (CElite::ELITE_ANIMATION::TURBO_TO_DASH == pModel->Get_CurAnimationIndex() &&
		0.9 <= TrackPos / (_float)Duration)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		CElite* pElite = static_cast<CElite*>(m_pOwner);
		
		_float fCurEnergy = pElite->Get_Energy();

		if (fCurEnergy > 0.f)
		{
			pModel->SetUp_Animation(CElite::ELITE_ANIMATION::WALK_B, true);
			pFsm->Change_State(CElite::ELITE_ANIMATION::WALK_F, STATE_DIR::BACK);
		}
		else
		{
			pModel->SetUp_Animation(CElite::ELITE_ANIMATION::HIT_STUN, true);
			pFsm->Change_State(CElite::ELITE_ANIMATION::HIT_STUN);
		}		
	}

}

void CElite_Turbo::End_State()
{
	m_bStartTurboDash = false;
	m_fAccSpeed = 0.f;
	

	XMStoreFloat3(&m_GoDirNor, XMVectorZero());
	XMStoreFloat3(&m_RushStartPos, XMVectorZero());
}


_bool CElite_Turbo::Check_Death()
{
	return _bool();
}


_bool CElite_Turbo::Check_TurboDash(_float fTimeDelta)
{
	CTransform* pEliteTransform = m_pOwner->Get_Transform();
	_vector vElitePos = pEliteTransform->Get_State(CTransform::STATE_POSITION);

	// ó�������� ��������ġ - ���� ����ġ  == �Ÿ����̰�
	_float fAccRushedDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&m_RushStartPos), vElitePos)));

	_float fDistanceToMove = m_fAccSpeed;	// �̹��������� �Ÿ�

	_float fRemainingDistance = m_fMaxRushDistance - fAccRushedDistance;


	if (50.f >= fRemainingDistance) 
	{
		CElite* pElite = static_cast<CElite*>(m_pOwner);

		static_cast<CWeapon_Elite*>(pElite->Get_Part(CElite::PART_WEAPON))->Check_Collision();


		CModel* pModel = pElite->Get_Model();	

		if (CElite::ELITE_ANIMATION::TURBO_TO_DASH != pModel->Get_CurAnimationIndex())
		{
			_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
			TrackPos = 0.0;

			pModel->SetUp_Animation(CElite::ELITE_ANIMATION::TURBO_TO_DASH, true);
		}

		
		CEliteSwordTrail* pSwordTrail = static_cast<CWeapon_Elite*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CElite::PARTID::PART_WEAPON))->Get_SwordTrail();
		pSwordTrail->Set_Active(true);

		if (0.1f >= fRemainingDistance)
		{
			fDistanceToMove = 0.f;
			m_bStartTurboDash = false;
		}	
	}
	else
	{
		pEliteTransform->Go_Straight(fDistanceToMove);
		m_fAccSpeed += fTimeDelta;

		CEliteSwordTrail* pSwordTrail = static_cast<CWeapon_Elite*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CElite::PARTID::PART_WEAPON))->Get_SwordTrail();
		pSwordTrail->Set_Active(false);
	}

	

	return false;
}




CElite_Turbo* CElite_Turbo::Create(class CGameObject* pOwner)
{
	CElite_Turbo* pInstance = new CElite_Turbo(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CElite_Turbo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElite_Turbo::Free()
{
	__super::Free();
}
