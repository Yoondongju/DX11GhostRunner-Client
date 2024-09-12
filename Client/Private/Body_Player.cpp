#include "stdafx.h"
#include "..\Public\Body_Player.h"

#include "Player.h"
#include "Weapon_Player.h"

#include "FreeCamera.h"

#include "GameInstance.h"
#include "Animation.h"

#include "Mesh.h"
#include "Static_Object.h"

CBody_Player::CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{

}

CBody_Player::CBody_Player(const CBody_Player& Prototype)
	: CPartObject{ Prototype }
{

}

const _float4x4* CBody_Player::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PhysX()))
		return E_FAIL;


	Ready_Modify_Animation();


	m_pModelCom->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::IDLE, true);

	return S_OK;
}

void CBody_Player::Priority_Update(_float fTimeDelta)
{

}

void CBody_Player::Update(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	CTransform* pPlayerTransform = pPlayer->Get_Transform();



	if (true == m_pModelCom->Play_Animation(fTimeDelta))			// 애니메이션이 끝났으면 걍 무조건 Idle
	{

	}


	// 내 점프 시간이 길었을때
	_float fJumpProgresstime = pPlayer->Get_JumpProgressTime();

	if (fJumpProgresstime > 1.5f)
	{
		_double CurTrackPosition = m_pModelCom->Get_Referene_CurrentTrackPosition();
		_double Duration = m_pModelCom->Get_CurAnimation()->Get_Duration();

		if (0.6 <= CurTrackPosition / Duration)
		{
			if (false == pPlayer->Get_Shaking())
			{
				_vector vRightVector = pPlayerTransform->Get_State(CTransform::STATE_RIGHT);
				vRightVector = XMVector3Normalize(vRightVector);

				pPlayer->Set_Shake(fJumpProgresstime * 3.f / 60.f, fJumpProgresstime * 3.f / 60.f, vRightVector);
			}
		}
	}




	m_PxTransform.p = { m_WorldMatrix.m[3][0], m_WorldMatrix.m[3][1] + pPlayer->Get_OffsetY() * 0.5f  , m_WorldMatrix.m[3][2] };
	m_pPxRigidDynamic->setGlobalPose(m_PxTransform);



	ProcessCollision();		// 충돌했을때 해야하는것 처리

	if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
		ProcessInRange();		// 어떤 범위안에 들어왔을때 처리



	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

}


void CBody_Player::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBody_Player::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBody_Player::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	/* FOR.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_PhysX"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom))))
		return E_FAIL;




	return S_OK;
}


void CBody_Player::Ready_Modify_Animation()
{
	vector<CAnimation*>& Animations = m_pModelCom->Get_Animations();

	//Animations[CPlayer::PLAYER_ANIMATIONID::JUMP_START]->Set_SpeedPerSec(70.f);
	//Animations[CPlayer::PLAYER_ANIMATIONID::JUMP_END]->Set_SpeedPerSec(35.f);
	//
	//
	//Animations[CPlayer::PLAYER_ANIMATIONID::HOOK_UP]->Set_SpeedPerSec(40.f);
	//
	//
	//
	Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_R1]->Set_SpeedPerSec(50.f);
	Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_R2]->Set_SpeedPerSec(50.f);
	Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_R3]->Set_SpeedPerSec(60.f);
	
	Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_L1]->Set_SpeedPerSec(50.f);
	Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_L2]->Set_SpeedPerSec(50.f);
	Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_L3]->Set_SpeedPerSec(60.f);
	//
	//
	Animations[CPlayer::PLAYER_ANIMATIONID::SH_ATTACK]->Set_SpeedPerSec(26.f);
	//
	//
	//Animations[CPlayer::PLAYER_ANIMATIONID::CLIMB]->Set_NextAnimLerpDuration(0.1f);
	//
	//
	Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_R1]->Set_NextAnimLerpDuration(0.08f);
	Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_R2]->Set_NextAnimLerpDuration(0.1f);
	Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_R3]->Set_NextAnimLerpDuration(0.1f);
	
	Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_L1]->Set_NextAnimLerpDuration(0.08f);
	Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_L2]->Set_NextAnimLerpDuration(0.1f);
	Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_L3]->Set_NextAnimLerpDuration(0.1f);
	//
	//
	//Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_R1]->Set_Duration(25.0);
	//Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_R2]->Set_Duration(25.0);
	//Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_R3]->Set_Duration(25.0);
	//
	//Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_L1]->Set_Duration(25.0);
	//Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_L2]->Set_Duration(25.0);
	//Animations[CPlayer::PLAYER_ANIMATIONID::ATTACK_L3]->Set_Duration(25.0);

}

HRESULT CBody_Player::Ready_PhysX()
{
	m_PxTransform = PxTransform(PxMat44(m_pTransformCom->Get_WorldMatrix_Ptr()->_11));

	PxVec3 vStartPos = { m_pParentMatrix->m[1][0] , m_pParentMatrix->m[1][1] , m_pParentMatrix->m[1][2] };
	PxVec3 vEndPos = { -m_pParentMatrix->m[1][0], -m_pParentMatrix->m[1][1] , -m_pParentMatrix->m[1][2] };
	m_PxTransform = PxTransformFromSegment(vStartPos, vEndPos);


	m_pPxRigidDynamic = m_pGameInstance->Get_Physics()->createRigidDynamic(m_PxTransform);

	PxMaterial* Material = m_pGameInstance->Get_Material();


	vector<CMesh*>& Meshs = m_pModelCom->Get_Meshes();


	// 4. Triangle Mesh 기반 Shape 생성
	m_pShape = m_pGameInstance->Get_Physics()->createShape(PxCapsuleGeometry(7.f, 3.f), *Material);
	if (!m_pShape) {
		MSG_BOX(TEXT("createShape failed!"));
		return E_FAIL;
	}


	//PxTransform localPose(PxVec3(0.f, 10.f, 0.f));  // (5.f, 0.f, 0.f) 위치로 이동
	//m_pShape->setLocalPose(localPose);
	m_pPxRigidDynamic->attachShape(*m_pShape);
	m_pGameInstance->Get_Scene()->addActor(*m_pPxRigidDynamic);


	// 충돌계산하는걸 직접불러야함 
	// 내가 만든 강체쓰고 
	// 스타트포즈가 내 업쪽이고   엔드포즈가 내 업에 반대쪽 
	// PxVec3바꿀려면 내 _float3나 _vector의 성분 x,y,z하나씩대입해서 만들어줘야한다
	// m_Tranform == Px에서 제공하는 트랜스폼이고  
	// 내 엔진에 트랜스폼위치를 m_Tranform로 갱신해야하는데 이과정이 PxTransformSegment라는 함수로 갱신해야한다.

	// m_Tranform.p가 내 위치임
	// PxTriangleMeshGeometry가 정점정보지 
	// PX컴퓨트트라이앵글메쉬페네트리에이션 이 함수로 충돌비교를한다
	//  setGlobalPose로 내 PXTransform 의 정보를 세팅해준다



	return S_OK;
}



void CBody_Player::ProcessCollision()
{
	//m_pColliderCom->Update();

	PhysXComputeCollision();


	

}

void CBody_Player::PhysXComputeCollision()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	CTransform* pPlayerTransform = pPlayer->Get_Transform();



	m_pColliderCom->Set_PrePhysXCollision(m_pColliderCom->Get_CurPhysXCollision());	// 충돌 들어가기전에 항상 이전 충돌정보를 세팅!
	m_bClimbing = false;

	if (true == m_bLandingWall)
	{
		LandWall();
	}

	if (true == m_pGameInstance->CollisionUpdate_PlayerToTriangleMeshGeometry(&m_vDir, &m_vDepth, m_pShape, &m_PxTransform, &m_pCollisionDestObject))
	{
		m_pColliderCom->Set_CurPhysXCollision(true);

		_float4x4* pParentMatrix = const_cast<_float4x4*>(m_pParentMatrix);
		pParentMatrix->m[3][0] += m_vDir.x * (m_vDepth + 0.1f);
		pParentMatrix->m[3][1] += m_vDir.y * (m_vDepth + 0.1f);
		pParentMatrix->m[3][2] += m_vDir.z * (m_vDepth + 0.1f);

		// 충돌한 객체가 벽을 탈수있는애인걸 확인해야함

		if (m_pColliderCom->IsPhysXCollisionEnter())
		{
			CStatic_Object* pStaticObject = dynamic_cast<CStatic_Object*>(m_pCollisionDestObject);
			if (pStaticObject)
			{
				if (pStaticObject->IsCanLandWall())
				{
					m_bLandingWall = true;


					PxVec3 rayOrigin = m_PxTransform.p;
					_vector vRightNormal = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_RIGHT));
					PxVec3 rayDirection = { XMVectorGetX(vRightNormal),XMVectorGetY(vRightNormal),XMVectorGetZ(vRightNormal) };
					_float rayLength = 500.0f;

					if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
					{
						if(CWeapon_Player::WEAPON_TYPE::KATANA ==  pPlayer->Get_CurWeaponType())
							m_pModelCom->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::JUMP_LOOP, true);
						else if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == pPlayer->Get_CurWeaponType())
							m_pModelCom->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_RUN_WALL_L, true);

						m_iLandWallDir = -1;
						rayDirection *= -1.f; // 반대방향으로 바꾸고
					}
					else
					{
						if (CWeapon_Player::WEAPON_TYPE::KATANA == pPlayer->Get_CurWeaponType())
							m_pModelCom->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::RUN_WALL_R, true);
						else if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == pPlayer->Get_CurWeaponType())
							m_pModelCom->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::SH_RUN_WALL_R, true);

						m_iLandWallDir = 1;
					}



					PxRaycastBuffer hitInfo;
					PxQueryFilterData data;
					data.flags = PxQueryFlag::eSTATIC;

					_bool status = m_pGameInstance->Get_Scene()->raycast(rayOrigin, rayDirection, rayLength, hitInfo, PxHitFlag::eDEFAULT, data);

					if (status && hitInfo.hasBlock)
					{
						_float fDistance = hitInfo.block.distance;
						m_fPreLandingDistance = fDistance;


						_float4x4* pParentMatrix = const_cast<_float4x4*>(m_pParentMatrix);
						pParentMatrix->m[3][0] += hitInfo.block.normal.x * (m_fPreLandingDistance * 0.8f);
						//pParentMatrix->m[3][1] += rayDirection.y * (fDistance);
						pParentMatrix->m[3][2] += hitInfo.block.normal.z * (m_fPreLandingDistance * 0.8f);

					}
				}

			}
		}
	}

	else if (false == m_bLandingWall)
	{
		m_pColliderCom->Set_CurPhysXCollision(false);

		if (true == m_bRotationChange) // 회전에 변화가 있었니?
		{
			_vector vLookNormal = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_LOOK));

			_float	fRotationSpeed = -m_fFinalRotation;
			_float	fCurRotationSpeed = fRotationSpeed * 0.04f;


			if (m_iLandWallDir > 0)
			{
				if (m_fAccRotationSpeed + fCurRotationSpeed >= -m_fFinalRotation)
				{
					_matrix RotationMatrix = XMMatrixRotationAxis(vLookNormal, XMConvertToRadians(fCurRotationSpeed));
					pPlayer->Get_Transform()->Turn(RotationMatrix, pPlayer->Get_RotationMatrixPtr());

					m_fAccRotationSpeed += fCurRotationSpeed;
				}
				else
				{
					_float fRemainingRotation = -m_fFinalRotation - m_fAccRotationSpeed;

					_matrix RotationMatrix = XMMatrixRotationAxis(vLookNormal, XMConvertToRadians(fRemainingRotation));
					pPlayer->Get_Transform()->Turn(RotationMatrix, pPlayer->Get_RotationMatrixPtr());

					m_fAccRotationSpeed = 0.f;
					m_bRotationChange = false;
				}

			}
			else if (m_iLandWallDir < 0)
			{
				if (m_fAccRotationSpeed + fCurRotationSpeed <= -m_fFinalRotation)
				{
					_matrix RotationMatrix = XMMatrixRotationAxis(vLookNormal, XMConvertToRadians(fCurRotationSpeed));
					pPlayer->Get_Transform()->Turn(RotationMatrix, pPlayer->Get_RotationMatrixPtr());

					m_fAccRotationSpeed += fCurRotationSpeed;
				}
				else
				{
					_float fRemainingRotation = -m_fFinalRotation - m_fAccRotationSpeed;

					_matrix RotationMatrix = XMMatrixRotationAxis(vLookNormal, XMConvertToRadians(fRemainingRotation));
					pPlayer->Get_Transform()->Turn(RotationMatrix, pPlayer->Get_RotationMatrixPtr());

					m_fAccRotationSpeed = 0.f;
					m_bRotationChange = false;
				}
			}

		}


		// 만약 충돌을 안했는데 플레이어 Y와 메쉬의 Y 차이가 offset보다 크면 진짜 충돌을 안해서 중력이 들어가야하고
		// 충돌 안햇는데 플레이어 Y와 메쉬의 Y 차이가 offset보다 작거나 같으면 중력적용을 안해야해
		// 그럼 메쉬의 Y를 구해야한다.

		PxVec3 rayOrigin = m_PxTransform.p;

		PxVec3 vEndPos = { 0.0f, -1.0f, 0.0f };

		PxVec3 rayDirection = vEndPos;		// 내 업벡터의 반대방향

		// 레이 길이는 충분히 크게 설정
		_float rayLength = 500.0f; // 필요에 따라 조정 가능


		PxRaycastBuffer hitInfo;
		PxQueryFilterData data;
		data.flags = PxQueryFlag::eSTATIC;


		_bool status = m_pGameInstance->Get_Scene()->raycast(rayOrigin, rayDirection, rayLength, hitInfo, PxHitFlag::eDEFAULT, data);

		if (status && hitInfo.hasBlock)
		{
			_float fMeshY = hitInfo.block.position.y;
			_float fPlayerY = m_PxTransform.p.y;

			pPlayer->Set_LandPosY(fMeshY);
		}
		else
			pPlayer->Set_LandPosY(-100.f);	// 지형이 없을때 맥시멈으로 떨어지는위치  
	}

}

void CBody_Player::ProcessInRange()
{
	m_bClimbing = false;

	_vector vLookNormal = XMVector3Normalize(m_pOwner->Get_Transform()->Get_State(CTransform::STATE_LOOK));
	_float fPlayerY = XMVectorGetY(m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION));

	list<CGameObject*>& ClimbObjects = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, L"Layer_Climb_Object");
	for (auto& ClimbObject : ClimbObjects)
	{
		// 플레이어의 룩 방향으로 레이를 쏴
		// 그 레이랑 히트의 거리를 비겨해

		_float fObjectY = XMVectorGetY(ClimbObject->Get_Transform()->Get_State(CTransform::STATE::STATE_POSITION));
		
		if (40.f <= fabs(fPlayerY - fObjectY))
			continue;



		PxVec3 rayOrigin = m_PxTransform.p;
		PxVec3 rayDirection = { XMVectorGetX(vLookNormal),XMVectorGetY(vLookNormal),XMVectorGetZ(vLookNormal) };
		_float rayLength = 500.0f;


		PxRaycastBuffer hitInfo;
		PxQueryFilterData data;
		data.flags = PxQueryFlag::eSTATIC;

		_bool status = m_pGameInstance->Get_Scene()->raycast(rayOrigin, rayDirection, rayLength, hitInfo, PxHitFlag::eDEFAULT, data);

		if (status && hitInfo.hasBlock)
		{
			PxRigidActor* pHitActor = hitInfo.block.actor;
			const PxRigidActor* pClimbObjectActor = static_cast<CStatic_Object*>(ClimbObject)->Get_RigidStatic();

			if (pClimbObjectActor != pHitActor)
				return;


			_float fDistance = hitInfo.block.distance;			
			if (m_fCanClimbDistance >= fDistance)
			{
				m_bClimbing = true;
				break;
			}
		}
	}

}

void CBody_Player::LandWall()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
	CTransform* pPlayerTransform = pPlayer->Get_Transform();


	PxVec3 rayOrigin = m_PxTransform.p;
	_vector vRightNormal = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_RIGHT));
	PxVec3 rayDirection = { XMVectorGetX(vRightNormal),XMVectorGetY(vRightNormal),XMVectorGetZ(vRightNormal) };
	_float rayLength = 500.0f;

	rayDirection *= (_float)m_iLandWallDir;

	_vector vLookNormal = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE_LOOK));


	_float	fRotationSpeed = 40.f * m_iLandWallDir;
	_float	fCurRotationSpeed = fRotationSpeed * 0.03f;


	if (m_iLandWallDir > 0)
	{
		if (m_fAccRotationSpeed + fCurRotationSpeed <= 40.f * m_iLandWallDir)
		{
			_matrix RotationMatrix = XMMatrixRotationAxis(vLookNormal, XMConvertToRadians(fCurRotationSpeed));
			pPlayer->Get_Transform()->Turn(RotationMatrix, pPlayer->Get_RotationMatrixPtr());

			m_fAccRotationSpeed += fCurRotationSpeed;
			m_bRotationChange = true;
		}
		else
		{
			_float fRemainingRotation = (40.f * m_iLandWallDir) - m_fAccRotationSpeed;

			_matrix RotationMatrix = XMMatrixRotationAxis(vLookNormal, XMConvertToRadians(fRemainingRotation));
			pPlayer->Get_Transform()->Turn(RotationMatrix, pPlayer->Get_RotationMatrixPtr());

			m_fAccRotationSpeed = 40.f * m_iLandWallDir;
		}
	}
	else if (m_iLandWallDir < 0)
	{
		if (m_fAccRotationSpeed + fCurRotationSpeed >= 40.f * m_iLandWallDir)
		{
			_matrix RotationMatrix = XMMatrixRotationAxis(vLookNormal, XMConvertToRadians(fCurRotationSpeed));
			pPlayer->Get_Transform()->Turn(RotationMatrix, pPlayer->Get_RotationMatrixPtr());

			m_fAccRotationSpeed += fCurRotationSpeed;
			m_bRotationChange = true;
		}
		else
		{
			_float fRemainingRotation = (40.f * m_iLandWallDir) - m_fAccRotationSpeed;

			_matrix RotationMatrix = XMMatrixRotationAxis(vLookNormal, XMConvertToRadians(fRemainingRotation));
			pPlayer->Get_Transform()->Turn(RotationMatrix, pPlayer->Get_RotationMatrixPtr());

			m_fAccRotationSpeed = 40.f * m_iLandWallDir;
		}
	}



	PxRaycastBuffer hitInfo;
	PxQueryFilterData data;
	data.flags = PxQueryFlag::eSTATIC;

	_bool status = m_pGameInstance->Get_Scene()->raycast(rayOrigin, rayDirection, rayLength, hitInfo, PxHitFlag::eDEFAULT, data);

	if (status && hitInfo.hasBlock)
	{
		_float fDistance = hitInfo.block.distance;

		if (fDistance >= m_fPreLandingDistance * 10.f)
		{
			m_bLandingWall = false;
			m_fFinalRotation = m_fAccRotationSpeed;

			m_fAccRotationSpeed = 0.f;
		}

	}
	else
	{
		m_bLandingWall = false;
		m_fFinalRotation = m_fAccRotationSpeed;

		m_fAccRotationSpeed = 0.f;
	}

}


CBody_Player* CBody_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Player* pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBody_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CBody_Player::Clone(void* pArg)
{
	CBody_Player* pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBody_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();

	if (m_pPxRigidDynamic)
		m_pPxRigidDynamic->release();

	if (m_pShape)
		m_pShape->release();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);

	Safe_Release(m_pCollisionDestObject);
}
