#include "stdafx.h"
#include "Hel_CutScene.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

#include "Player.h"
#include "FreeCamera.h"

CHel_CutScene::CHel_CutScene(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::CUTSCENE , pOwner }
{

}

HRESULT CHel_CutScene::Initialize()
{
	m_TrigerPos_FromJump_Page2 = { -781.493164, 34.9999962 - 35.f , -60.9446106 };			//  35 플레이어Y오프셋임 플레이어로측정해서그럼
	m_TrigerPos_FromBlock_Page2 = { 2584.25879, -690.606384 - 35.f , -55.9093895 };			//  35 플레이어Y오프셋임 플레이어로측정해서그럼

	m_fInitialYSpeed = 1500.f;

	return S_OK;
}

HRESULT CHel_CutScene::Start_State(void* pArg)
{
	CHel* pHel = static_cast<CHel*>(m_pOwner);

	if (true == pHel->IsPage2())
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));

		CTransform* pHelTransform = pHel->Get_Transform();
		CTransform* pPlayerTransform = pPlayer->Get_Transform();



		_vector vRight = { -0.0123771112, -9.32721450e-05, -0.999918640, 0.00000000 };
		_vector vUp = { -0.0471226722, 0.998892903, 0.000490342616, 0.00000000 };
		_vector vLook = { 0.998816073, 0.0471246205, -0.0123669896, 0.00000000 };

		_vector vNewPos = { -3641.47900, 35.0000038, 171.1805286, 1.00000000 };

		pPlayerTransform->Set_State(CTransform::STATE_RIGHT, vRight);
		pPlayerTransform->Set_State(CTransform::STATE_UP, vUp);
		pPlayerTransform->Set_State(CTransform::STATE_LOOK, vLook);
		pPlayerTransform->Set_State(CTransform::STATE_POSITION, vNewPos);

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





		_vector vHelRight = { 0.208447903, -0.00000000, 3.49380541, 0.0000000 };
		_vector vHelUp = { 0.00000000, 3.50000000, 0.00000000, 0.00000000 };
		_vector vHelLook = { -3.49379349, 0.00000000, 0.208447188, 0.00000000 };
		_vector vHelPos = { -3601.91406, 2.26679776e-06, 168.8199997, 1.00000000 };

		pHelTransform->Set_State(CTransform::STATE_RIGHT, vHelRight);
		pHelTransform->Set_State(CTransform::STATE_UP, vHelUp);
		pHelTransform->Set_State(CTransform::STATE_LOOK, vHelLook);
		pHelTransform->Set_State(CTransform::STATE_POSITION, vHelPos);



		CModel* pModel = m_pOwner->Get_Model();
		CFsm* pFsm = m_pOwner->Get_Fsm();
		_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
		TrackPos = 0.0;
		pModel->SetUp_Animation(CHel::HEL_ANIMATION::DASH_BACK, true);


		CRigidBody* pRigidBody = m_pOwner->Get_RigidBody();
		pRigidBody->Set_Velocity(_float3(0.f, 0.f, 0.f));
		pRigidBody->Set_Accel(_float3(0.f, 0.f, 0.f));
		pRigidBody->Set_ZeroTimer();


		_vector vHelLookNor = XMVector3Normalize(pHelTransform->Get_State(CTransform::STATE_LOOK));
		pRigidBody->Add_Force_Direction(-vHelLookNor, 150, Engine::CRigidBody::ACCELERATION);
		pRigidBody->Add_Force_Direction(-vHelLookNor, 150, Engine::CRigidBody::VELOCITYCHANGE);

	}
	else
	{
		CTransform* pHelTransform = pHel->Get_Transform();
		
		_vector vHelRight = { 0.208447903, -0.00000000, 3.49380541, 0.0000000 };
		_vector vHelUp = { 0.00000000, 3.50000000, 0.00000000, 0.00000000 };
		_vector vHelLook = { -3.49379349, 0.00000000, 0.208447188, 0.00000000 };
		_vector vHelPos = { -3601.91406, 2.26679776e-06, 168.8199997, 1.00000000 };

		pHelTransform->Set_State(CTransform::STATE_RIGHT, vHelRight);
		pHelTransform->Set_State(CTransform::STATE_UP, vHelUp);
		pHelTransform->Set_State(CTransform::STATE_LOOK, vHelLook);
		pHelTransform->Set_State(CTransform::STATE_POSITION, vHelPos);



		CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
		CTransform* pCamTransform = pCamera->Get_Transform();
		
		pCamTransform->Set_State(CTransform::STATE_RIGHT, XMVectorSet(0.f, 0.f, -1.f, 0.f));
		pCamTransform->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		pCamTransform->Set_State(CTransform::STATE_LOOK, XMVectorSet(1.f, 0.f, 0.f, 0.f));
		pCamera->Set_FollowPlayer(false);
		pCamera->FollowHel(pHel);

		_float4& fOffsetHel = pCamera->Get_OffsetByHelRef();
		fOffsetHel = { 1050.f, 1000.f, -1000.f , 1.f };
	}

	return S_OK;
}


void CHel_CutScene::Update(_float fTimeDelta)
{
	CHel* pHel = static_cast<CHel*>(m_pOwner);
	if (true == pHel->IsPage2())
	{
		Page2(fTimeDelta);
	}
	else
		Page1(fTimeDelta);
}



void CHel_CutScene::End_State()
{
	m_fAccRotationSpeed = 0.f;
	m_fInitialYSpeed = 0.f;
	m_t = 0.f;

	m_isStartTurn = false;
	m_isStartJump = false;
	m_isEndJump = false;
	m_isActiveNamiCutScene = false;

	CModel* pModel = m_pOwner->Get_Model();
	vector<class CAnimation*>& Anims = pModel->Get_Animations();
	Anims[CHel::HEL_ANIMATION::RUN_JUMP]->Set_SpeedPerSec(30.f);


	CHel* pHel = static_cast<CHel*>(m_pOwner);
	if (true == pHel->IsPage2())
	{
		CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
		CTransform* pCamTransform = pCamera->Get_Transform();
		pCamera->Set_FollowPlayer(true);
	}

}


_vector CHel_CutScene::BezierCurve(_float t, const _fvector& P0, const _fvector& P1, const _fvector& P2, const _fvector& P3)
{
	return XMVectorAdd(
		XMVectorAdd(
			XMVectorScale(P0, (1 - t) * (1 - t) * (1 - t)),
			XMVectorScale(P1, 3 * t * (1 - t) * (1 - t))
		),
		XMVectorAdd(
			XMVectorScale(P2, 3 * t * t * (1 - t)),
			XMVectorScale(P3, t * t * t)
		)
	);
}

void CHel_CutScene::Page1(_float fTimeDelta)
{
	CModel* pModel = m_pOwner->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();
	CHel* pHel = static_cast<CHel*>(m_pOwner);


	if (m_pGameInstance->Get_KeyState(KEY::I) == KEY_STATE::TAP)
	{
		pFsm->Change_State(CHel::HEL_ANIMATION::CUTSCENE);
	}


	_float4 TargetOffset = { 0.f, 70.f, -70.f , 1.f };
	CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
	_float4& fOffsetHel = pCamera->Get_OffsetByHelRef();
	
	
	_vector DistanceVec = XMVectorSubtract(XMLoadFloat4(&TargetOffset) , XMLoadFloat4(&fOffsetHel));
	_float fDistanceToTarget = XMVectorGetX(XMVector3Length(DistanceVec));
	
	if (fDistanceToTarget < 5.f)
	{
		pHel->Get_Part(CHel::PARTID::PART_PARTICLE_SWIRL)->SetActiveMyParticle(true);

		pModel->SetUp_Animation(CHel::HEL_ANIMATION::RAISE, true);
		pFsm->Change_State(CHel::HEL_ANIMATION::RAISE);
		return;
	}
	
	_float fSpeedFactor = 0.01f;  
	_float fSpeed = fDistanceToTarget * fSpeedFactor;
	
	
	_vector P0 = XMLoadFloat4(&fOffsetHel);	   // 카메라 위치
	_vector P1 = XMVectorSet(fOffsetHel.x - 15.f, fOffsetHel.y - 5.f, fOffsetHel.z - 10.f, 0.f); // P1은 카메라의 y축 높이를 유지
	_vector P2 = XMVectorSet(TargetOffset.x , TargetOffset.y + 10.f, TargetOffset.z - 5.f, 0.f); // P2는 타겟의 z축 방향으로 조정
	_vector P3 = XMLoadFloat4(&TargetOffset);  // 타겟 위치
	

	// 이동 로직
	
	
	_float fAccSpeed = 3.f;

	if (fDistanceToTarget < 110.f)
	{
		fAccSpeed = 9.f;
	}

	m_t += fTimeDelta * fSpeed * fAccSpeed / fDistanceToTarget;
	if (m_t > 1.0f)
		m_t = 1.0f;
	
	

	_vector vNewCameraPos = BezierCurve(m_t, P0, P1, P2, P3);
	XMStoreFloat4(&fOffsetHel , vNewCameraPos);

	pCamera->FollowHel(pHel);
}


void CHel_CutScene::Page2(_float fTimeDelta)
{
	CModel* pModel = m_pOwner->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();
	CHel* pHel = static_cast<CHel*>(m_pOwner);

	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();

	if (false == m_isStartJump && 0.9f < (_float)TrackPos / Duration)
	{
		pModel->SetUp_Animation(CHel::HEL_ANIMATION::IDLE, true);
		m_isStartTurn = true;
	}

	if (true == m_isStartTurn)
	{
		CTransform* pHelTransform = pHel->Get_Transform();
		_float	fRotationSpeed = 180.f;
		_float	fCurRotationSpeed = fRotationSpeed * fTimeDelta;

		if (m_fAccRotationSpeed + fCurRotationSpeed <= fRotationSpeed)		// 턴하는즁
		{
			_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fCurRotationSpeed));
			pHelTransform->Turn(RotationMatrix, nullptr);

			m_fAccRotationSpeed += fCurRotationSpeed;
		}
		else
		{
			if (false == m_isStartJump)
			{
				_vector vCurPos = pHelTransform->Get_State(CTransform::STATE_POSITION);
				_float fDistanceToTrigerPos = XMVectorGetX(XMVector3Length(vCurPos - XMLoadFloat3(&m_TrigerPos_FromJump_Page2)));
				if (fDistanceToTrigerPos <= 400.f)
				{
					TrackPos = 0.0;

					vector<class CAnimation*>& Anims = pModel->Get_Animations();
					Anims[CHel::HEL_ANIMATION::RUN_JUMP]->Set_SpeedPerSec(15.f);

					pModel->SetUp_Animation(CHel::HEL_ANIMATION::RUN_JUMP, true);
					m_isStartJump = true;
					m_pGameInstance->ActiveBlur(nullptr, CRenderer::BLUR_TYPE::MOTION_BLUR);
				}
				else
				{
					_float fRemainingRotation = fRotationSpeed - m_fAccRotationSpeed;
					_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRemainingRotation));
					pHelTransform->Turn(RotationMatrix, nullptr);

					m_fAccRotationSpeed = fRotationSpeed;
					pModel->SetUp_Animation(CHel::HEL_ANIMATION::SPRINT, true);
					pHelTransform->Go_Straight(fTimeDelta * 95.f);


					CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
					CTransform* pCamTransform = pCamera->Get_Transform();

					_float4& OffSetHel = pCamera->Get_OffsetByHelRef();
					if (-70 > OffSetHel.z)
					{
						_float fMaxSpeed = 800.f;
						_float fMinSpeed = 10.f;
						_float fSpeedFactor = 1.7f;
						_float fSpeed = { 1.f };

						_float fDistance = fabs(OffSetHel.z - (-70.f));

						// 거리에 비례한 속도 계산 (최대 속도를 넘지 않도록)
						fSpeed = min(fDistance * fSpeedFactor, fMaxSpeed);
						OffSetHel.z += fTimeDelta * fSpeed;
					}
						

					pCamTransform->Set_State(CTransform::STATE_RIGHT, XMVectorSet(0.f, 0.f, -1.f, 0.f));
					pCamTransform->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
					pCamTransform->Set_State(CTransform::STATE_LOOK, XMVectorSet(1.f, 0.f, 0.f, 0.f));

					pCamera->Set_FollowPlayer(false);
					pCamera->FollowHel(pHel);

				}
			}
			else
			{
				CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
				pCamera->FollowHel(pHel);


				_vector vCurPos = pHelTransform->Get_State(CTransform::STATE_POSITION);
				_float fDistanceToTrigerPos = fabs(vCurPos.m128_f32[0] - m_TrigerPos_FromBlock_Page2.x);
				if (fDistanceToTrigerPos <= 100.f)		// 도착햇다면 
				{
					if (false == m_isActiveNamiCutScene)
					{
						pModel->SetUp_Animation(CHel::HEL_ANIMATION::IDLE, true);
						m_pGameInstance->UnActiveBlur();

						CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
						CModel* pPlayerBodyModel = pPlayer->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
						CFsm* pFsm = pPlayer->Get_Fsm();


						pCamera->Set_FollowPlayer(true);
						pPlayerBodyModel->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::NAMI_IDLE_TO_AIM, true);
						pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::DUMMY3);

						m_isActiveNamiCutScene = true;
					}
					return;
				}

				_float fHorizontalSpeed = 70.f;
				pHelTransform->Go_Straight(fTimeDelta * fHorizontalSpeed);


				_vector vHelCurPos = pHelTransform->Get_State(CTransform::STATE_POSITION);
				_float fTargetHeight = 600.f;
				_float fHeightDiff = fTargetHeight - vHelCurPos.m128_f32[1];	// 높이차이 


				_float fMaxYSpeed = 6000.f;
				_float fAcceleration = 6500;	// 가속도 (값을 조정해 가속하는 속도를 결정)
				_float fDeceleration = 6500.f;  // 감속도 (감속하는 속도를 결정)


				if (false == m_isEndJump && fHeightDiff > 0) // 목표보다 아직 아래에 있을 때
				{
					m_fInitialYSpeed += fAcceleration * fTimeDelta * 5.f;
				}
				else // 목표보다 높아졌다면
				{
					m_isEndJump = true;
					m_fInitialYSpeed -= fDeceleration * fTimeDelta * 4.f;
				}
				pHelTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vHelCurPos, m_fInitialYSpeed * fTimeDelta));
			}
		}
	}
}



CHel_CutScene* CHel_CutScene::Create(class CGameObject* pOwner)
{
	CHel_CutScene* pInstance = new CHel_CutScene(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_CutScene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_CutScene::Free()
{
	__super::Free();
}
