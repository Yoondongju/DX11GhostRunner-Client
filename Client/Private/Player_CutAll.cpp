#include "stdafx.h"
#include "Player_CutAll.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Animation.h"


#include "Sniper.h"
#include "Pistol.h"
#include "Mira.h"
#include "Jetpack.h"


#include "SwordTrail.h"

#include "EnemyMarkerUI.h"
#include "EventNotify.h"

CPlayer_CutAll::CPlayer_CutAll(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::FURR_AIM_LOOP , pOwner }
{

}

HRESULT CPlayer_CutAll::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_CutAll::Start_State(void* pArg)
{
	if (m_Targets.capacity() < 10)
		m_Targets.reserve(10);


	FindVisiableEnemy();

	sort(m_Targets.begin(), m_Targets.end());


	m_iNumHunt = 3;

	if (m_Targets.size() >= m_iNumHunt)	// ��� ����
	{
		static_cast<CPlayer*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_PARTICLE_CUTALL)->SetActiveMyParticle(true);

		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::FURR_AIM_LOOP, true);


		for (_uint i = 0; i < m_Targets.size(); i++)
		{
			static_cast<CEnemy*>(m_Targets[i].pSelf)->Set_Targeting(true);
		}
	}
	else							// ��� �Ұ���
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);

		static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_CUTALL);
	}

	return S_OK;
}


void CPlayer_CutAll::Update(_float fTimeDelta)
{
	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	CTransform* pPlayerTransform = m_pOwner->Get_Transform();
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::STATE_POSITION);


	// ��� �������� UI�� ����� 
	// UI ��m��


	m_fTargetDeleteTime += fTimeDelta;

	if (m_fTargetDeleteTime >= 0.3f && m_Targets.size() > m_iNumHunt)			// 4��   ���� ���Ͼ� 3����
	{
		m_fTargetDeleteTime = 0.f;

		static_cast<CEnemy*>(m_Targets.back().pSelf)->Set_Targeting(false);
		m_Targets.pop_back();
	}
	else if (m_Targets.size() == m_iNumHunt)
	{
		for (_uint i = 0; i < m_Targets.size(); i++)
		{
			static_cast<CEnemy*>(m_Targets[i].pSelf)->Set_FinalTargeting(true);
		}

		m_isHuntStart = true;
	}



	if (true == m_isHuntStart)
	{
		if (m_iNumKill >= m_iNumHunt)
		{
			m_isHuntStart = false;
			CFsm* pFsm = m_pOwner->Get_Fsm();
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
			return;
		}


		_vector vDistance = XMVectorSubtract(XMLoadFloat3(&m_Targets[m_iNumKill].fValue), vPlayerPos);
		_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

		_float4x4* pPlayerRotationMatrix = static_cast<CPlayer*>(m_pOwner)->Get_RotationMatrixPtr();


		m_fStartTime += fTimeDelta;
		pPlayerTransform->LookAt_Smooth(XMLoadFloat3(&m_Targets[m_iNumKill].fValue), fTimeDelta * 2, pPlayerRotationMatrix);


		
		if (fDistance <= 40.f)
		{
			Check_Collision();

			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE, true);

			CSwordTrail* pSwordTrail = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_SwordTrail();
			pSwordTrail->Set_Active(true);
		}
		else
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::FURR_AIM_LOOP, true);


		if (fDistance > 15.f)
		{
			if (m_fStartTime >= 0.5f)
			{
				_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

				_float fMaxSpeed = 6.f;  
				_float fSpeedRatio = fDistance / 100.0f;
				m_fAccSpeed = fSpeedRatio * fMaxSpeed * fTimeDelta;

				  

				pPlayerTransform->Go_Dir(vDistance, m_fAccSpeed);

				CSwordTrail* pSwordTrail = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_SwordTrail();
				pSwordTrail->Set_Active(false);
			}
		}

	}
}


void CPlayer_CutAll::End_State()
{
	m_Targets.clear();

	if (m_iNumKill != 0)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
		pPlayer->Set_StartCountCutAllTime(false);
		pPlayer->Set_CutAllRemainingTime(0.f);

	}

	m_isHuntStart = false;
	m_iNumKill = 0;

	

	CSwordTrail* pSwordTrail = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_SwordTrail();
	pSwordTrail->Set_Active(false);
}


void CPlayer_CutAll::FindVisiableEnemy()
{
	CTransform* pPlayerTransform = m_pOwner->Get_Transform();

	_vector vPlayerLookNor = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_LOOK));
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_float fVisiableAngle = m_fVisibleAngle * 0.5f;


	list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Sniper");
	for (auto Sniper : Snipers)
	{
		_vector vSniperPos = Sniper->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_vector vDifference = XMVectorSubtract(vSniperPos, vPlayerPos);		// ������ ����
		_float fDistacne = XMVectorGetX(XMVector3Length(vDifference));		// �� ���̳� ������ ����

		_vector vDir = XMVector3Normalize(vDifference);
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLookNor, vDir));		// ������ �����

		_float fAngle = XMConvertToDegrees(acos(fDot));

		if (fVisiableAngle >= fAngle &&
			m_fVisiableDistance >= fDistacne)	// �þ߰� �ȿ��ְ� �Ÿ��� �����ϴ�
		{
			_float3	Pos = {};
			vSniperPos = XMVectorSetY(vSniperPos, vSniperPos.m128_f32[1] + 20.f);
			XMStoreFloat3(&Pos, vSniperPos);

			Float3Wrapper  SniperPos;
			SniperPos.pSelf = Sniper;
			SniperPos.fValue = Pos;
			SniperPos.fDistanceToPlayer = fDistacne;

			m_Targets.emplace_back(SniperPos);
		}
	}

	list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Pistol");
	for (auto Pistol : Pistols)
	{
		_vector vPistolPos = Pistol->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_vector vDifference = XMVectorSubtract(vPistolPos, vPlayerPos);		// ������ ����
		_float fDistacne = XMVectorGetX(XMVector3Length(vDifference));		// �� ���̳� ������ ����

		_vector vDir = XMVector3Normalize(vDifference);
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLookNor, vDir));		// ������ �����

		_float fAngle = XMConvertToDegrees(acos(fDot));

		if (fVisiableAngle >= fAngle &&
			m_fVisiableDistance >= fDistacne)	// �þ߰� �ȿ� �մ�
		{
			_float3	Pos = {};
			vPistolPos = XMVectorSetY(vPistolPos, vPistolPos.m128_f32[1] + 20.f);
			XMStoreFloat3(&Pos, vPistolPos);

			Float3Wrapper  PistolPos;
			PistolPos.pSelf = Pistol;
			PistolPos.fValue = Pos;
			PistolPos.fDistanceToPlayer = fDistacne;

			m_Targets.emplace_back(PistolPos);
		}
	}

	list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Mira");
	for (auto Mira : Miras)
	{
		_vector vMiraPos = Mira->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_vector vDifference = XMVectorSubtract(vMiraPos, vPlayerPos);		// ������ ����
		_float fDistacne = XMVectorGetX(XMVector3Length(vDifference));		// �� ���̳� ������ ����

		_vector vDir = XMVector3Normalize(vDifference);
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLookNor, vDir));		// ������ �����

		_float fAngle = XMConvertToDegrees(acos(fDot));

		if (fVisiableAngle >= fAngle &&
			m_fVisiableDistance >= fDistacne)	// �þ߰� �ȿ� �մ�
		{
			_float3	Pos = {};
			vMiraPos = XMVectorSetY(vMiraPos, vMiraPos.m128_f32[1] + 20.f);
			XMStoreFloat3(&Pos, vMiraPos);
		
			Float3Wrapper  MiraPos;
			MiraPos.pSelf = Mira;
			MiraPos.fValue = Pos;
			MiraPos.fDistanceToPlayer = fDistacne;

			m_Targets.emplace_back(MiraPos);
		}
	}
}

void CPlayer_CutAll::Check_Collision()
{
	list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Sniper");
	for (auto& Sniper : Snipers)
	{
		if (true == static_cast<CSniper*>(Sniper)->Check_Collision())
		{
			
			++m_iNumKill;

			m_fStartTime = 0.f;
			m_fAccSpeed = 0.f;
		}
			
	}

	list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Pistol");
	for (auto& Pistol : Pistols)
	{
		if (true == static_cast<CPistol*>(Pistol)->Check_Collision())
		{
			
			++m_iNumKill;

			m_fStartTime = 0.f;
			m_fAccSpeed = 0.f;
		}
			
	}

	list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Mira");
	for (auto& Mira : Miras)
	{
		if (true == static_cast<CMira*>(Mira)->Check_Collision())
		{
			
			++m_iNumKill;

			m_fStartTime = 0.f;
			m_fAccSpeed = 0.f;
		}
			
	}

}

CPlayer_CutAll* CPlayer_CutAll::Create(class CGameObject* pOwner)
{
	CPlayer_CutAll* pInstance = new CPlayer_CutAll(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_CutAll"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_CutAll::Free()
{
	__super::Free();
}
