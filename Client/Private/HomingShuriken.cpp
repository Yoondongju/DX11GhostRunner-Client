#include "stdafx.h"
#include "HomingShuriken.h"

#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
#include "Animation.h"


#include "Sniper.h"
#include "Pistol.h"
#include "Mira.h"
#include "Jetpack.h"

#include "SwordTrail.h"
#include "ShurikenTrail.h"

#include "EventNotify.h"

CHomingShuriken::CHomingShuriken(class CGameObject* pOwner)
	: CState{ CPlayer::PLAYER_ANIMATIONID::RIFT_PICKUP , pOwner }
{

}

HRESULT CHomingShuriken::Initialize()
{
	return S_OK;
}

HRESULT CHomingShuriken::Start_State(void* pArg)
{
	if (m_TargetsPos.capacity() < 10)
		m_TargetsPos.reserve(10);

	FindVisiableEnemy();

	sort(m_TargetsPos.begin(), m_TargetsPos.end());




	if (m_TargetsPos.size() >= 1)  // 한마리이상있으면  가능
	{

	}
	else							// 불가능
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);

		static_cast<CEventNotify*>(m_pGameInstance->Find_Notify(LEVEL_GAMEPLAY))->Set_Active(true, CEventNotify::TEXT_EVENT::UNABLE_HOMING_SH);
		return S_OK;
	}


	static_cast<CPlayer*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_PARTICLE_CUTALL)->SetActiveMyParticle(true);

	CWeapon_Player* pMainShuriken = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));
	pMainShuriken->Set_HomingShurikenActive(true);

	m_pGameInstance->Play_Sound(TEXT("espark1.mp3"), SOUND_PLAYER, 4.f);

	return S_OK;
}

void CHomingShuriken::Update(_float fTimeDelta)
{
	Check_Collision();

	CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
	CWeapon_Player* pMainShuriken = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));

	
	CTransform* pShurikenTransform = pMainShuriken->Get_Transform();
	_vector vShurikenPos = pShurikenTransform->Get_State(CTransform::STATE::STATE_POSITION);

	


	if (false == m_isHuntStart)
	{
		if (m_fAccScaling < 15.f)
		{
			m_fAccScaling += fTimeDelta * 15.f;
			pShurikenTransform->Scaling(m_fAccScaling, m_fAccScaling, m_fAccScaling);
		}
	}	
	else if (true == m_isHuntStart && false == m_isDownScale)
	{
		if (m_fAccScaling >= 100.f)
			m_fAccScaling = true;

		m_fAccScaling += fTimeDelta * 110.f;
		pShurikenTransform->Scaling(m_fAccScaling, m_fAccScaling, m_fAccScaling);
	}
	else
	{
		if (m_fAccScaling <= 80.f)
			m_fAccScaling = false;

		m_fAccScaling -= fTimeDelta * 95.f;
		pShurikenTransform->Scaling(m_fAccScaling, m_fAccScaling, m_fAccScaling);
	}
	


	_vector vRightNor = XMVector3Normalize(pShurikenTransform->Get_State(CTransform::STATE_RIGHT));
	pShurikenTransform->Turn(vRightNor, fTimeDelta * 150.f);	// 계속 돌아 whsskehfdk rmsid


	if (m_fAccScaling >= 15.f)
	{
		m_isHuntStart = true;
		pMainShuriken->Set_HomingStartHunt(true);

		_float3 ShScale = pShurikenTransform->Get_Scaled();
		_float4x4* pWorldMatrix = pMainShuriken->Get_PartObjectComBindWorldMatrixPtr();

		_float4x4* pBeforeWorldMatrix = pShurikenTransform->Get_WorldMatrix_Ptr();
	

		XMStoreFloat3((_float3*)pWorldMatrix->m[0],  XMVector3Normalize(XMLoadFloat3((_float3*)pBeforeWorldMatrix->m[0])) * ShScale.x);
		XMStoreFloat3((_float3*)pWorldMatrix->m[1],  XMVector3Normalize(XMLoadFloat3((_float3*)pBeforeWorldMatrix->m[1])) * ShScale.y);
		XMStoreFloat3((_float3*)pWorldMatrix->m[2],  XMVector3Normalize(XMLoadFloat3((_float3*)pBeforeWorldMatrix->m[2])) * ShScale.z);

		pMainShuriken->Get_Transform()->Set_WorldMatrix(*pWorldMatrix);			// 원래 이 녀석의 고유한 행렬은 로컬의 오프셋잡아주기위함이엿지만 스킬일때 잠깐 바꿔치기해
	}

	if (true == m_isHuntStart)
	{
		if (m_iNumKill > m_TargetsPos.size() - 1)		
		{
			m_fStartTime += fTimeDelta;

			if (m_fStartTime >= 0.5f)
			{
				m_isHuntStart = false;
				CFsm* pFsm = m_pOwner->Get_Fsm();
				pModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_IDLE, true);
				pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);
			}
			return;
		}


		_vector vDistance = XMVectorSubtract(XMLoadFloat3(m_TargetsPos[m_iNumKill].pValue), vShurikenPos);
		_float fDistance = XMVectorGetX(XMVector3Length(vDistance));


		_vector vOvershootPos = XMLoadFloat3(m_TargetsPos[m_iNumKill].pValue) + XMVector3Normalize(vDistance) * 100.f;
		_vector vNewDistance = XMVectorSubtract(vOvershootPos, vShurikenPos);
		_float fNewDistance = XMVectorGetX(XMVector3Length(vNewDistance));

		
		if (fNewDistance <= 110.f) 
		{			
			if(m_iNumKill < m_TargetsPos.size())
				++m_iNumKill;

			m_fStartTime = 0.f;
			m_fAccSpeed = 0.f;

			XMStoreFloat3(&m_StartPos, vShurikenPos);
		}

		m_fStartTime += fTimeDelta;
		

		if (m_fStartTime >= 0.1f)
		{
			CShurikenTrail* pMainShurikenTrail = pMainShuriken->Get_ShurikenTrail();
			pMainShurikenTrail->Set_Active(true);

			m_fAccSpeed += fTimeDelta;

			_int iRandom = m_pGameInstance->Get_Random_Interger(0, 1);

			_vector vWaveAxis = {};
			_float  fWaveAmplitude = 2.f;


			switch (iRandom)
			{
			case 0:
				vWaveAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				break;
			case 1:
				vWaveAxis = XMVectorSet(1.f, 0.f, 0.f, 0.f);
				fWaveAmplitude = 7.f;
				break;
			default:
				break;
			}
			pShurikenTransform->Go_Dir(XMVector3Normalize(vDistance), m_fAccSpeed);
		}
		//else
		//{
		//	CShurikenTrail* pMainShurikenTrail = pMainShuriken->Get_ShurikenTrail();
		//	pMainShurikenTrail->Set_Active(false);
		//}
		
	}
}

void CHomingShuriken::End_State()
{
	m_TargetsPos.clear();

	m_isHuntStart = false;
	m_iNumKill = 0;
	m_fAccSpeed = 0.f;


																
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	pPlayer->Set_StartCountHomingShTime(false);
	pPlayer->Set_HomingShRemainingTime(0.f);


	CWeapon_Player* pMainShuriken = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));
	pMainShuriken->Set_HomingShurikenActive(false);
	pMainShuriken->Set_HomingStartHunt(false);

	pMainShuriken->Get_Transform()->Scaling(1.f, 1.f, 1.f);
	m_fAccScaling = 1.f;
	m_isDownScale = false;

	CShurikenTrail* pMainShurikenTrail = pMainShuriken->Get_ShurikenTrail();
	pMainShurikenTrail->Set_Active(false);
}


void CHomingShuriken::FindVisiableEnemy()
{
	CWeapon_Player* pMainShuriken = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));
	CTransform* pShurikenTransform = pMainShuriken->Get_Transform();
	_vector vShurikenPos = pShurikenTransform->Get_State(CTransform::STATE::STATE_POSITION);


	//list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Sniper");
	//for (auto& Sniper : Snipers)
	//{
	//	const _float4x4* pSniperMatrix = Sniper->Get_Transform()->Get_WorldMatrix_Ptr();
	//
	//	_float3* pSniperPos = (_float3*)(pSniperMatrix->m[3]);
	//	_float3* pSniperLook = (_float3*)(pSniperMatrix->m[2]);
	//
	//	_vector vSniperPos = XMLoadFloat3(pSniperPos);
	//	_vector vDistance = XMVectorSubtract(vShurikenPos, vSniperPos);
	//	_float fDistance = XMVectorGetX(XMVector3Length(vDistance));
	//
	//
	//	vSniperPos = XMVectorSetY(vSniperPos, vSniperPos.m128_f32[1] + 40.f);
	//
	//
	//	Float3Wrapper  SniperPos;
	//	SniperPos.pValue = pSniperPos;
	//	SniperPos.fDistanceToPlayer = fDistance;
	//
	//	Float3Wrapper  SniperLook;
	//	SniperLook.pValue = pSniperLook;
	//	SniperLook.fDistanceToPlayer = fDistance;
	//
	//	m_TargetsPos.emplace_back(SniperPos);
	//}
	//
	//list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Pistol");
	//for (auto& Pistol : Pistols)
	//{
	//	const _float4x4* pPistolMatrix = Pistol->Get_Transform()->Get_WorldMatrix_Ptr();
	//
	//	_float3* pPistolPos = (_float3*)(pPistolMatrix->m[3]);
	//	_float3* pPistolLook = (_float3*)(pPistolMatrix->m[2]);
	//
	//	_vector vPistolPos = XMLoadFloat3(pPistolPos);
	//	_vector vDistance = XMVectorSubtract(vShurikenPos, vPistolPos);
	//	_float fDistance = XMVectorGetX(XMVector3Length(vDistance));
	//
	//	vPistolPos = XMVectorSetY(vPistolPos, vPistolPos.m128_f32[1] + 40.f);
	//
	//
	//	Float3Wrapper  PistolPos;
	//	PistolPos.pValue = pPistolPos;
	//	PistolPos.fDistanceToPlayer = fDistance;
	//
	//	Float3Wrapper  PistolLook;
	//	PistolLook.pValue = pPistolLook;
	//	PistolLook.fDistanceToPlayer = fDistance;
	//
	//	m_TargetsPos.emplace_back(PistolPos);
	//}
	//
	//list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Mira");
	//for (auto& Mira : Miras)
	//{
	//	const _float4x4* pMiraMatrix = Mira->Get_Transform()->Get_WorldMatrix_Ptr();
	//
	//	_float3* pMiraPos = (_float3*)(pMiraMatrix->m[3]);
	//	_float3* pMiraLook = (_float3*)(pMiraMatrix->m[2]);
	//
	//	_vector vMiraPos = XMLoadFloat3(pMiraPos);
	//	_vector vDistance = XMVectorSubtract(vShurikenPos, vMiraPos);
	//	_float fDistance = XMVectorGetX(XMVector3Length(vDistance));
	//
	//
	//	vMiraPos = XMVectorSetY(vMiraPos, vMiraPos.m128_f32[1] + 40.f);
	//
	//
	//	Float3Wrapper  MiraPos;
	//	MiraPos.pValue = pMiraPos;
	//	MiraPos.fDistanceToPlayer = fDistance;
	//
	//	Float3Wrapper  MiraLook;
	//	MiraLook.pValue = pMiraLook;
	//	MiraLook.fDistanceToPlayer = fDistance;
	//
	//
	//	m_TargetsPos.emplace_back(MiraPos);
	//}

	list<CGameObject*>& Jetpacks = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Jetpack");
	for (auto& Jetpack : Jetpacks)
	{
		if (static_cast<CEnemy*>(Jetpack)->IsDead())
			continue;

		const _float4x4* pJetpackWorldMatrix = Jetpack->Get_Transform()->Get_WorldMatrix_Ptr();

		_float3* pJetpackPos = (_float3*)(pJetpackWorldMatrix->m[3]);
		_float3* pJetpackLook = (_float3*)(pJetpackWorldMatrix->m[2]);

		_vector vJetpackPos = XMLoadFloat3(pJetpackPos);
		_vector vDistance = XMVectorSubtract(vShurikenPos, vJetpackPos);
		_float fDistance = XMVectorGetX(XMVector3Length(vDistance));		// 처음 정렬하기위해 필요한값이고


		vJetpackPos = XMVectorSetY(vJetpackPos, vJetpackPos.m128_f32[1] + 30.f);

		


		Float3Wrapper  JetpackPos;
		JetpackPos.pValue = pJetpackPos;
		JetpackPos.fDistanceToPlayer = fDistance;

		Float3Wrapper  JetpackLook;
		JetpackLook.pValue = pJetpackLook;
		JetpackLook.fDistanceToPlayer = fDistance;


		m_TargetsPos.emplace_back(JetpackPos);
	}

	XMStoreFloat3(&m_StartPos, vShurikenPos);
}

void CHomingShuriken::Check_Collision()
{
	CWeapon_Player* pMainShuriken = static_cast<CWeapon_Player*>(static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));
	CTransform* pShurikenTransform = pMainShuriken->Get_Transform();
	_vector vShurikenPos = pShurikenTransform->Get_State(CTransform::STATE::STATE_POSITION);

	//list<CGameObject*>& Snipers = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Sniper");
	//for (auto& Sniper : Snipers)
	//{
	//	if (true == static_cast<CSniper*>(Sniper)->Check_Collision())
	//	{
	//		++m_iNumKill;
	//
	//		m_fStartTime = 0.f;
	//		m_fAccSpeed = 0.f;
	//
	//		XMStoreFloat3(&m_StartPos, vShurikenPos);
	//	}
	//
	//}
	//
	//list<CGameObject*>& Pistols = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Pistol");
	//for (auto& Pistol : Pistols)
	//{
	//	if (true == static_cast<CPistol*>(Pistol)->Check_Collision())
	//	{
	//		++m_iNumKill;
	//
	//		m_fStartTime = 0.f;
	//		m_fAccSpeed = 0.f;
	//
	//		XMStoreFloat3(&m_StartPos, vShurikenPos);
	//	}
	//
	//}
	//
	//list<CGameObject*>& Miras = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Mira");
	//for (auto& Mira : Miras)
	//{
	//	if (true == static_cast<CMira*>(Mira)->Check_Collision())
	//	{
	//		++m_iNumKill;
	//
	//		m_fStartTime = 0.f;
	//		m_fAccSpeed = 0.f;
	//
	//		XMStoreFloat3(&m_StartPos, vShurikenPos);
	//	}
	//
	//}

	list<CGameObject*>& Jetpacks = m_pGameInstance->Get_GameObjects(g_CurLevel, L"Layer_Jetpack");
	for (auto& Jetpack : Jetpacks)
	{
		if (true == static_cast<CJetpack*>(Jetpack)->Check_Collision())
		{

		}
	
	}
}

CHomingShuriken* CHomingShuriken::Create(class CGameObject* pOwner)
{
	CHomingShuriken* pInstance = new CHomingShuriken(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHomingShuriken"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHomingShuriken::Free()
{
	__super::Free();
}
