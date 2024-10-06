#include "stdafx.h"
#include "Elite_Block.h"


#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"

#include "PartObject.h"

#include "Weapon_Elite.h"
#include "EliteSwordTrail.h"

CElite_Block::CElite_Block(class CGameObject* pOwner)
	: CState{ CElite::ELITE_ANIMATION::BLOCK_F , pOwner }
{

}

HRESULT CElite_Block::Initialize()
{
	return S_OK;
}

HRESULT CElite_Block::Start_State(void* pArg)
{
	if (nullptr != pArg)
		m_bDashBlock = *(_bool*)(pArg);
	

	CModel* pModel = m_pOwner->Get_Model();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	TrackPos = 0.f;



	
	CEliteSwordTrail* pSwordTrail = static_cast<CWeapon_Elite*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CElite::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);
	pSwordTrail->Set_Active(true);

	return S_OK;
}

void CElite_Block::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;

	CElite* pElite = static_cast<CElite*>(m_pOwner);
	CModel* pModel = pElite->Get_Model();

	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();


	if (true == m_bDashBlock)	// ������ ���ĳ����� ���⿴�ٸ�
	{		
		_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		CTransform* pEliteTransform = m_pOwner->Get_Transform();
		_vector vElitePos = pEliteTransform->Get_State(CTransform::STATE_POSITION);

		pEliteTransform->LookAt_XZ(vPlayerPos);

		_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(vPlayerPos, vElitePos)));

		_float fMinSpeed = 2.0f;    // �Ÿ��� ����� �� �ּ� �ӵ�
		_float fMaxSpeed = 20.0f;   // �Ÿ��� �� �� �ִ� �ӵ�
		_float fMaxDistance = 100.0f; // �ִ� �Ÿ�, �� �Ÿ� �̻��� �� fMaxSpeed ���

		// �Ÿ� ��� �ӵ� ���
		_float fSpeedFactor = fDistance / fMaxDistance;  // �Ÿ��� ���� ���� ��� (0 ~ 1 ������ ��)
		//fSpeedFactor = min(fSpeedFactor, 1.0f);          // 1.0 �̻����� �Ѿ�� �ʵ��� ����

		_float fAdjustedSpeed = fMinSpeed + (fMaxSpeed - fMinSpeed) * fSpeedFactor;

		static_cast<CWeapon_Elite*>(pElite->Get_Part(CElite::PART_WEAPON))->Check_Collision();

		if (fDistance > 50.f)
		{		
			pEliteTransform->Go_Straight(fAdjustedSpeed * fTimeDelta);
		}
		else
		{	
			pElite->Get_Part(CElite::PARTID::PART_PARTICLE_DASHBLOCK)->SetActiveMyParticle(true);	// �÷��̾� �Ÿ��� �־����� �翬 ��������
			m_bDashBlock = false;
		}			
	}


	if (0.9 <= TrackPos / (_float)Duration)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		_float fCurEnergy = pElite->Get_Energy();

		if (fCurEnergy > 0.f)
		{
			pModel->SetUp_Animation(CElite::ELITE_ANIMATION::IDLE_TO_ALERT, true);
			pFsm->Change_State(CElite::ELITE_ANIMATION::IDLE_TO_ALERT);
		}
		else
		{
			pModel->SetUp_Animation(CElite::ELITE_ANIMATION::HIT_STUN, true);
			pFsm->Change_State(CElite::ELITE_ANIMATION::HIT_STUN);
		}

	}

}

void CElite_Block::End_State()
{
	m_bDashBlock = false;
	m_fAccTime = 0.f;

	CElite* pElite = static_cast<CElite*>(m_pOwner);
	//pElite->Get_Part(CElite::PARTID::PART_PARTICLE_DASHBLOCK)->SetActiveMyParticle(false);
	pElite->Get_Part(CElite::PARTID::PART_PARTICLE_BLOCK)->SetActiveMyParticle(false);


	CEliteSwordTrail* pSwordTrail = static_cast<CWeapon_Elite*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CElite::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);
}


_bool CElite_Block::Check_Death()
{
	return _bool();
}




CElite_Block* CElite_Block::Create(class CGameObject* pOwner)
{
	CElite_Block* pInstance = new CElite_Block(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CElite_Block"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElite_Block::Free()
{
	__super::Free();
}
