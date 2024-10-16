#include "stdafx.h"
#include "Player_MindControl.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Animation.h"


#include "Sniper.h"
#include "Pistol.h"
#include "Mira.h"

#include "EventNotify.h"

CPlayer_MindControl::CPlayer_MindControl(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::MIND_CONTROL_START_START , pOwner }
{

}

HRESULT CPlayer_MindControl::Initialize()
{
	return S_OK;
}

HRESULT CPlayer_MindControl::Start_State(void* pArg)
{
	if( 0 == m_VisibleEnemy.capacity())
		m_VisibleEnemy.reserve(10);


	FindVisiableEnemy();


	if (m_VisibleEnemy.size() >= m_iCanVisibleNum)
	{
		// 마인드컨트롤 가능
		CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::MIND_CONTROL_START_START, true);


		_uint iSize = m_VisibleEnemy.size();
		for (_uint i = 0; i < iSize; i++)
		{
			CEnemy* pEnemy = static_cast<CEnemy*>(m_VisibleEnemy[i]);
			CEnemy* pTargetEnemy = static_cast<CEnemy*>(m_VisibleEnemy[(i + 1) % iSize]);


			// 각자의 Idle에서 Attack으로 변경해주고있는데 
			// 마인드컨트롤 상태면 강제로 Attack으로 변경하되 방향도 지들이 지들을 바라봐야한다.

			pEnemy->Set_MindControlReady(true, pTargetEnemy);
		}


	}
	else
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);

		static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_MINDCONTROL);
		return S_OK;
	}


	m_pGameInstance->ActiveRefraction(static_cast<CPlayer*>(m_pOwner)->Get_MindControlRefractionTex(), CRenderer::REFRACTION_TYPE::MINDCONTROL);

	m_pGameInstance->Play_Sound(TEXT("MindControlStart.ogg"), SOUND_PLAYEREFFECT, g_fEffectVolume);


	return S_OK;
}

void CPlayer_MindControl::Update(_float fTimeDelta)
{

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();

	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();


	m_fAccTime += fTimeDelta;

	if(m_fAccTime >= 2.f)
		CommandToAttackEachEnemy();
	else if (m_fAccTime >= 1.5f)
	{
		if (CPlayer::PLAYER_ANIMATIONID::MIND_CONTROL_START_TO_IDLE != pModel->Get_CurAnimationIndex())
			TrackPos = 0.f;


		if (false == m_isMCEndSoundActive)
		{
			m_pGameInstance->StopSound(SOUND_PLAYEREFFECT);
			m_pGameInstance->Play_Sound(TEXT("MindControlEnd.ogg"), SOUND_PLAYEREFFECT, 3.f);
			m_isMCEndSoundActive = true;
		}

		pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::MIND_CONTROL_START_TO_IDLE, true);
		m_pGameInstance->UnActiveRefraction();
	}
	if (m_fAccTime >= 0.8f)
	{
		if (false == m_isMCLoopSoundActive)
		{
			m_pGameInstance->StopSound(SOUND_PLAYEREFFECT);
			m_pGameInstance->Play_Sound(TEXT("MindControlLoop.ogg"), SOUND_PLAYEREFFECT, 3.f);
			m_isMCLoopSoundActive = true;
		}	
	}
	

	if (true == m_isOrderCommand)		// 명령을 한번이상 내렸으면 
	{
		if (0.9f <= TrackPos / (_float)Duration)
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
			pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
			pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
			
			return;
		}
	}
}

void CPlayer_MindControl::End_State()
{
	if (true == m_isOrderCommand)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
		pPlayer->Set_StartCountMindControlTime(false);
		pPlayer->Set_MindControlRemainingTime(0.f);
	}


	for (_uint i = 0; i < m_VisibleEnemy.size(); i++)
	{
		Safe_Release(m_VisibleEnemy[i]);
	}
	m_VisibleEnemy.clear();


	m_isOrderCommand = false;
	m_isMCLoopSoundActive = false;
	m_isMCEndSoundActive = false;

	m_fAccTime = 0.f;
}


void CPlayer_MindControl::FindVisiableEnemy()
{
	CTransform* pPlayerTransform = m_pOwner->Get_Transform();

	_vector vPlayerLookNor = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_LOOK));
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_float fVisiableAngle = m_fVisibleAngle * 0.5f;



	list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Sniper");
	for (auto& Sniper : Snipers)
	{
		if (m_VisibleEnemy.size() >= m_iCanVisibleNum)
			return;

		static_cast<CSniper*>(Sniper)->Check_Collision();

		_vector vSniperPos = Sniper->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_vector vDifference = XMVectorSubtract(vSniperPos, vPlayerPos);		// 벡터의 차이
		_float fDistacne = XMVectorGetX(XMVector3Length(vDifference));		// 그 차이난 벡터의 길이

		_vector vDir = XMVector3Normalize(vDifference);
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLookNor, vDir));		// 내적의 결과고

		_float fAngle = XMConvertToDegrees(acos(fDot));

		if (fVisiableAngle >= fAngle &&
			m_fVisiableDistance >= fDistacne)	// 시야각 안에있고 거리도 만족하니
		{
			m_VisibleEnemy.push_back(Sniper);
			Safe_AddRef(Sniper);
		}
	}

	list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Pistol");
	for (auto& Pistol : Pistols)
	{
		if (m_VisibleEnemy.size() >= m_iCanVisibleNum)
			return;

		static_cast<CPistol*>(Pistol)->Check_Collision();

		_vector vPistolPos = Pistol->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_vector vDifference = XMVectorSubtract(vPistolPos, vPlayerPos);		// 벡터의 차이
		_float fDistacne = XMVectorGetX(XMVector3Length(vDifference));		// 그 차이난 벡터의 길이

		_vector vDir = XMVector3Normalize(vDifference);
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLookNor, vDir));		// 내적의 결과고

		_float fAngle = XMConvertToDegrees(acos(fDot));

		if (fVisiableAngle >= fAngle &&
			m_fVisiableDistance >= fDistacne)	// 시야각 안에 잇니
		{
			m_VisibleEnemy.push_back(Pistol);
			Safe_AddRef(Pistol);
		}
	}

	list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Mira");
	for (auto& Mira : Miras)
	{
		if (m_VisibleEnemy.size() >= m_iCanVisibleNum)
			return;

		static_cast<CMira*>(Mira)->Check_Collision();

		_vector vMiraPos = Mira->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_vector vDifference = XMVectorSubtract(vMiraPos, vPlayerPos);		// 벡터의 차이
		_float fDistacne = XMVectorGetX(XMVector3Length(vDifference));		// 그 차이난 벡터의 길이

		_vector vDir = XMVector3Normalize(vDifference);
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLookNor, vDir));		// 내적의 결과고

		_float fAngle = XMConvertToDegrees(acos(fDot));

		if (fVisiableAngle >= fAngle &&
			m_fVisiableDistance >= fDistacne)	// 시야각 안에 잇니
		{
			m_VisibleEnemy.push_back(Mira);
			Safe_AddRef(Mira);
		}
	}

}


void CPlayer_MindControl::CommandToAttackEachEnemy()
{
	if (true == m_isOrderCommand)	// 명령을 이미 내렸으면
		return;


	_uint iSize = m_VisibleEnemy.size();
	for (_uint i = 0; i < iSize; i++)
	{
		CEnemy*  pEnemy =	static_cast<CEnemy*>(m_VisibleEnemy[i]);
		CEnemy*  pTargetEnemy = static_cast<CEnemy*>(m_VisibleEnemy[(i + 1) % iSize]);


		// 각자의 Idle에서 Attack으로 변경해주고있는데 
		// 마인드컨트롤 상태면 강제로 Attack으로 변경하되 방향도 지들이 지들을 바라봐야한다.
		 
		pEnemy->Set_MindControling(true , pTargetEnemy);
	}

	m_isOrderCommand = true;
}




CPlayer_MindControl* CPlayer_MindControl::Create(class CGameObject* pOwner)
{
	CPlayer_MindControl* pInstance = new CPlayer_MindControl(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer_MindControl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_MindControl::Free()
{
	__super::Free();
}
