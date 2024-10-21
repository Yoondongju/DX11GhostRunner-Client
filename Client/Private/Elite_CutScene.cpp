#include "stdafx.h"
#include "Elite_CutScene.h"

#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"

#include "FreeCamera.h"

#include "PartObject.h"
#include "Player.h"

CElite_CutScene::CElite_CutScene(class CGameObject* pOwner)
	: CState{ CElite::ELITE_ANIMATION::CUTSCENE , pOwner }
{

}


HRESULT CElite_CutScene::Initialize()
{
	m_CutSceneExitPointPage1 = { 535.953247, 17.1262779, -1916.94250 };

	m_CutSceneExitPointPage2 = { 535.953247, 17.1262779, -3869.74463 };

	return S_OK;
}


HRESULT CElite_CutScene::Start_State(void* pArg)
{
	CElite* pElite = static_cast<CElite*>(m_pOwner);

	if (true == pElite->IsPage2())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));

		CTransform* pEliteTransform = pElite->Get_Transform();
		CTransform* pPlayerTransform = pPlayer->Get_Transform();


		_vector vElitePos = pEliteTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	



		_vector vRight = { -0.956862569, -3.20142135e-08, -0.290515333, 0.00000000 };
		_vector vUp = { 0.0142296404, 0.998797059, -0.0468661934, 0.00000000 };
		_vector vLook = { 0.290164500, -0.0489808470, -0.955717087, 0.00000000 };
		pPlayerTransform->Set_State(CTransform::STATE_RIGHT, vRight);
		pPlayerTransform->Set_State(CTransform::STATE_UP, vUp);
		pPlayerTransform->Set_State(CTransform::STATE_LOOK, vLook);


		vRight = XMVector3Normalize(vRight);
		vUp = XMVector3Normalize(vUp);
		vLook = XMVector3Normalize(vLook);

		_float4x4 RotationMatrix =
		{
			XMVectorGetX(vRight),XMVectorGetY(vRight) , XMVectorGetZ(vRight), 0.00000000,
			XMVectorGetX(vUp),XMVectorGetY(vUp) , XMVectorGetZ(vUp), 0.00000000,
			 XMVectorGetX(vLook),XMVectorGetY(vLook) , XMVectorGetZ(vLook), 0.00000000,
			 0.f,0.f,0.f,1.f
		};

		_float4x4* pRotationMatrix = pPlayer->Get_RotationMatrixPtr();

		*pRotationMatrix = RotationMatrix;
	


		_vector vEliteRight = { 3.37232661, -0.00000000, 0.936715424, 0.00000000 };
		_vector vEliteUp = { 0.00000000, 3.50000000, 0.00000000, 0.00000000 };
		_vector vEliteLook = { -0.936715126, 0.00000000, 3.37232566, 0.00000000 };
		pEliteTransform->Set_State(CTransform::STATE_RIGHT, vEliteRight);
		pEliteTransform->Set_State(CTransform::STATE_UP, vEliteUp);
		pEliteTransform->Set_State(CTransform::STATE_LOOK, vEliteLook);



		_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
		_vector vTargetLook = { -0.0145271355, 0.0699414089, -0.997434914, 0.00000000 };
		vTargetLook = XMVector3Normalize(vTargetLook);
		vPlayerLook = XMVector3Normalize(vPlayerLook);



		_vector vSetElitePos = {};
		_vector vSetPlayerPos = { m_CutSceneExitPointPage1.x, XMVectorGetY(vPlayerPos) , m_CutSceneExitPointPage1.z};

		pPlayerTransform->Set_State(CTransform::STATE_POSITION, vSetPlayerPos);

		vSetPlayerPos += vPlayerLook * 40.f;
		vSetElitePos = { XMVectorGetX(vSetPlayerPos), XMVectorGetY(vElitePos),XMVectorGetZ(vSetPlayerPos) };
	
		pEliteTransform->Set_State(CTransform::STATE_POSITION, vSetElitePos);
	
	

		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();


		pPlayer->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model()->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::FURR_AIM_TRICUT, true);
		pModel->SetUp_Animation(CElite::ELITE_ANIMATION::BLOCK_F, true);
		pElite->Get_Part(CElite::PARTID::PART_PARTICLE_BLOCK)->SetActiveMyParticle(true);

		m_pGameInstance->StopSound(SOUND_BGM);
		m_pGameInstance->PlayBGM(TEXT("ElitePage2.ogg"), 1.f);
	}
	else
	{
		CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
		CTransform* pCamTransform = pCamera->Get_Transform();

		pCamTransform->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
		pCamTransform->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		pCamTransform->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));

		m_pGameInstance->StopSound(SOUND_BGM);
		m_pGameInstance->PlayBGM(TEXT("ElitePage1.ogg"), 1.f);
	}

		
	return S_OK;
}


void CElite_CutScene::Update(_float fTimeDelta)
{
	CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
	CElite* pElite = static_cast<CElite*>(m_pOwner);

	if (false == pElite->IsPage2() && false == m_pGameInstance->Check_IsPlaying(SOUND_ELITE))
	{
		m_pGameInstance->SetPlayeSpeed(SOUND_ELITE, 20.f);
		m_pGameInstance->Play_Sound(TEXT("EliteCutSceneWalk.ogg"), SOUND_ELITE, 2.f);
	}
	

	_vector vCurPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_float4& OffsetEliteRef = pCamera->Get_OffsetByEliteRef();

	if (false == pElite->IsPage2())
	{		
		pCamera->Set_FollowPlayer(false);
		pCamera->FollowElite(pElite);

		_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_CutSceneExitPointPage1) - vCurPos));
		if (fDistance >= 30.f)
		{
			if (OffsetEliteRef.y <= 80.f)
				OffsetEliteRef.y += fTimeDelta * 8.f;

			if (OffsetEliteRef.z >= -90.f)
				OffsetEliteRef.z -= fTimeDelta * 10.f;

			m_pOwner->Get_Transform()->Go_Straight(fTimeDelta * 30.f * m_fSpeed);
		}
		else
		{
			CModel* pModel = m_pOwner->Get_Model();
			CFsm* pFsm = m_pOwner->Get_Fsm();

			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
			CModel* pPlayerModel = pPlayer->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
			CFsm* pPlayerFsm = pPlayer->Get_Fsm();

			_bool	bDashBlock = true;
			pModel->SetUp_Animation(CElite::ELITE_ANIMATION::DASH_TO_ALERDLB, true);
			pFsm->Change_State(CElite::ELITE_ANIMATION::BLOCK_F, -1, &bDashBlock);


			pPlayerModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::TIME_STOP, true);
			pPlayerFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::TIME_STOP);

		}

		if (fDistance <= 1700.f)
		{
			_float	fRotationSpeed = 180.f;
			_float	fCurRotationSpeed = fRotationSpeed * fTimeDelta * 0.2f;

			CTransform* pCamTransform = pCamera->Get_Transform();

			if (m_fAccRotationSpeed + fCurRotationSpeed <= fRotationSpeed)
			{
				_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fCurRotationSpeed));
				pCamTransform->Turn(RotationMatrix, nullptr);

				m_fAccRotationSpeed += fCurRotationSpeed;



				if (OffsetEliteRef.z >= -110.f)
					OffsetEliteRef.z -= fTimeDelta * 4.5f;
			}
			else
			{
				_float fRemainingRotation = fRotationSpeed - m_fAccRotationSpeed;

				_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRemainingRotation));
				pCamTransform->Turn(RotationMatrix, nullptr);

				m_fAccRotationSpeed = fRotationSpeed;

				m_fSpeed = 2.f;
			}
		}
	}
	else         // Page 2ÀÏ¶§
	{
		if (m_fAccRotationSpeed >= 377.8f)
		{
			pCamera->Set_FollowPlayer(false);
			pCamera->FollowElite(pElite);

			CModel* pModel = m_pOwner->Get_Model();
			CFsm* pFsm = m_pOwner->Get_Fsm();
			pModel->SetUp_Animation(CElite::ELITE_ANIMATION::WALK_B, true);

			_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_CutSceneExitPointPage2) - vCurPos));
			if (fDistance >= 175.f)
			{
				OffsetEliteRef.y = 80.f;

				m_pOwner->Get_Transform()->Go_Backward(fTimeDelta * 22.f * m_fSpeed);
			}
			else
			{
				_bool	bDashBlock = true;

				CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
				pPlayer->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model()->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);
				pPlayer->Get_Fsm()->Change_State(CPlayer::PLAYER_ANIMATIONID::IDLE);

				pModel->SetUp_Animation(CElite::ELITE_ANIMATION::IDLE_TO_ALERT, true);
				pFsm->Change_State(CElite::ELITE_ANIMATION::IDLE_TO_ALERT);

				_vector vPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);
				m_pOwner->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.m128_f32[0], 4.0262779, vPos.m128_f32[2], 1.f));
			}
		}
		else
		{
			_float	fRotationSpeed = 377.8f;
			_float	fCurRotationSpeed = fRotationSpeed * fTimeDelta * 0.55f;

			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
			_float4x4* pPlayerRotationMatrix = pPlayer->Get_RotationMatrixPtr();

			CTransform* pEliteTransform = pElite->Get_Transform();
			CTransform* pPlayerTransform = pPlayer->Get_Transform();


			m_fAccTime += fTimeDelta;
			if (m_fAccTime >= 0.5f)
			{
				if(true == pElite->Get_Part(CElite::PARTID::PART_PARTICLE_BLOCK)->IsActiveMyParticle())
					pElite->Get_Part(CElite::PARTID::PART_PARTICLE_BLOCK)->SetActiveMyParticle(false);
				else
					pElite->Get_Part(CElite::PARTID::PART_PARTICLE_BLOCK)->SetActiveMyParticle(true);

				m_fAccTime = 0.f;
			}
				


			if (m_fAccRotationSpeed + fCurRotationSpeed <= fRotationSpeed)
			{
				_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fCurRotationSpeed));
				pPlayerTransform->Turn(RotationMatrix, pPlayerRotationMatrix);
				pEliteTransform->Turn(RotationMatrix, nullptr);

				_vector vNewPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
				_vector vElitePos = pEliteTransform->Get_State(CTransform::STATE_POSITION);
				_vector vSetPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
				_vector vSetElitePos = {};


				vSetPlayerPos += XMVector3Normalize(vNewPlayerLook) * 40.f;
				vSetElitePos = { XMVectorGetX(vSetPlayerPos), XMVectorGetY(vElitePos),XMVectorGetZ(vSetPlayerPos) };
				pEliteTransform->Set_State(CTransform::STATE_POSITION, vSetElitePos);

				m_fAccRotationSpeed += fabs(fCurRotationSpeed);
			}
			else
			{
				_float fRemainingRotation = fRotationSpeed - m_fAccRotationSpeed;

				_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRemainingRotation));
				pPlayerTransform->Turn(RotationMatrix, pPlayerRotationMatrix);
				pEliteTransform->Turn(RotationMatrix, nullptr);

				_vector vNewPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
				_vector vElitePos = pEliteTransform->Get_State(CTransform::STATE_POSITION);
				_vector vSetPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
				_vector vSetElitePos = {};


				vSetPlayerPos += XMVector3Normalize(vNewPlayerLook) * 40.f;
				vSetElitePos = { XMVectorGetX(vSetPlayerPos), XMVectorGetY(vElitePos),XMVectorGetZ(vSetPlayerPos) };
				pEliteTransform->Set_State(CTransform::STATE_POSITION, vSetElitePos);

				m_fAccRotationSpeed = fRotationSpeed;
			}
		}

	}
	
}

void CElite_CutScene::End_State()
{
	m_pGameInstance->StopSound(SOUND_ELITE);


	m_fSpeed = 1.f;
	m_fAccTime = 0.f;
	m_fAccRotationSpeed = 0.f;

	CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
	pCamera->Set_FollowPlayer(true);

	static_cast<CElite*>(m_pOwner)->Set_Groggy(false);
}


CElite_CutScene* CElite_CutScene::Create(class CGameObject* pOwner)
{
	CElite_CutScene* pInstance = new CElite_CutScene(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CElite_CutScene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElite_CutScene::Free()
{
	__super::Free();
}
