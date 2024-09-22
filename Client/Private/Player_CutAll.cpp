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

	static_cast<CPlayer*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_PARTICLE_CUTALL)->SetActiveMyParticle(true);

	return S_OK;
}

void CPlayer_CutAll::Update(_float fTimeDelta)
{
	Check_Collision();

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	CTransform* pPlayerTransform = m_pOwner->Get_Transform();
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::STATE_POSITION);

	
	if (CPlayer::PLAYER_ANIMATIONID::FURR_AIM_LOOP == pModel->Get_CurAnimationIndex())
	{
		_double Duration = pModel->Get_CurAnimation()->Get_Duration();
		_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();

		if (3 > m_TargetsPos.size())
		{
			list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Sniper");
			for (auto& Sniper : Snipers)
			{
				_vector vSniperPos = Sniper->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION);

				_vector vDistance = XMVectorSubtract(vPlayerPos, vSniperPos);
				_float fDistance = XMVectorGetX(XMVector3Length(vDistance));



				_float3	Pos = {};
				_float3	Look = {};

				XMStoreFloat3(&Pos, vSniperPos);
				XMStoreFloat3(&Look, Sniper->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));

				Float3Wrapper  SniperPos;
				SniperPos.fValue = Pos;
				SniperPos.fDistanceToPlayer = fDistance;

				Float3Wrapper  SniperLook;
				SniperLook.fValue = Look;
				SniperLook.fDistanceToPlayer = fDistance;

				m_TargetsPos.emplace(SniperPos);
				m_TargetsLook.emplace(SniperLook);
			}

			list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Pistol");
			for (auto& Pistol : Pistols)
			{
				_vector vPistolPos = Pistol->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION);

				_vector vDistance = XMVectorSubtract(vPlayerPos, vPistolPos);
				_float fDistance = XMVectorGetX(XMVector3Length(vDistance));


				_float3	Pos = {};
				_float3	Look = {};

				XMStoreFloat3(&Pos, vPistolPos);
				XMStoreFloat3(&Look, Pistol->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));

				Float3Wrapper  PistolPos;
				PistolPos.fValue = Pos;
				PistolPos.fDistanceToPlayer = fDistance;

				Float3Wrapper  PistolLook;
				PistolLook.fValue = Look;
				PistolLook.fDistanceToPlayer = fDistance;

				m_TargetsPos.emplace(PistolPos);
				m_TargetsLook.emplace(PistolLook);
			}

			list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Mira");
			for (auto& Mira : Miras)
			{
				_vector vMiraPos = Mira->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION);

				_vector vDistance = XMVectorSubtract(vPlayerPos, vMiraPos);
				_float fDistance = XMVectorGetX(XMVector3Length(vDistance));


				_float3	Pos = {};
				_float3	Look = {};

				XMStoreFloat3(&Pos, vMiraPos);
				XMStoreFloat3(&Look, Mira->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));

				Float3Wrapper  MiraPos;
				MiraPos.fValue = Pos;
				MiraPos.fDistanceToPlayer = fDistance;

				Float3Wrapper  MiraLook;
				MiraLook.fValue = Look;
				MiraLook.fDistanceToPlayer = fDistance;


				m_TargetsPos.emplace(MiraPos);
				m_TargetsLook.emplace(MiraLook);
			}
		}

	}

	if (m_TargetsPos.size() >= 3)
	{
		m_isHuntStart = true;
	}



	if (true == m_isHuntStart)
	{
		if (m_iNumKill >= 3)
		{
			m_isHuntStart = false;

			CFsm* pFsm = m_pOwner->Get_Fsm();
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
			return;
		}


		_vector vDistance = XMVectorSubtract(XMLoadFloat3(&m_TargetsPos.top().fValue), vPlayerPos);
		_float fDistance = XMVectorGetX(XMVector3Length(vDistance));

		_float4x4* pPlayerRotationMatrix = static_cast<CPlayer*>(m_pOwner)->Get_RotationMatrixPtr();


		m_fStartTime += fTimeDelta;
		pPlayerTransform->LookAt_Smooth(XMLoadFloat3(&m_TargetsPos.top().fValue), fTimeDelta * 2, pPlayerRotationMatrix);

		if (fDistance <= 30.f)
		{		
			_double Duration = pModel->Get_CurAnimation()->Get_Duration();
			_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
			

			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE, true);


			m_fStartTime = 0.f;
			m_fAccSpeed = 0.f;

			m_TargetsPos.pop();
			m_TargetsLook.pop();
		}
		else
		{
			if (m_fStartTime >= 0.5f)
			{
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::FURR_AIM_LOOP, true);

				m_fAccSpeed += fTimeDelta;
				pPlayerTransform->Go_Dir(vDistance, m_fAccSpeed);
			}
		}
	}
}

void CPlayer_CutAll::End_State()
{
	while (!m_TargetsPos.empty())
	{
		m_TargetsPos.pop();
		m_TargetsLook.pop();
	}

	m_isHuntStart = false;
	m_iNumKill = 0;
}


void CPlayer_CutAll::Check_Collision()
{
	list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Sniper");
	for (auto& Sniper : Snipers)
	{
		if (true == static_cast<CSniper*>(Sniper)->Check_Collision())
		{

			++m_iNumKill;
		}
			
	}

	list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Pistol");
	for (auto& Pistol : Pistols)
	{
		if (true == static_cast<CPistol*>(Pistol)->Check_Collision())
		{

			++m_iNumKill;
		}
			
	}

	list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Mira");
	for (auto& Mira : Miras)
	{
		if (true == static_cast<CMira*>(Mira)->Check_Collision())
		{

			++m_iNumKill;
		}
			
	}

	list<CGameObject*>& Jetpacks = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Jetpack");
	for (auto& Jetpack : Jetpacks)
	{
		if (true == static_cast<CJetpack*>(Jetpack)->Check_Collision())
		{

			++m_iNumKill;
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
