#include "stdafx.h"
#include "..\Public\Body_Player.h"

#include "Player.h"
#include "FreeCamera.h"

#include "GameInstance.h"
#include "Animation.h"

#include "Mesh.h"

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
	if (true == m_pModelCom->Play_Animation(fTimeDelta))			// 애니메이션이 끝났으면 걍 무조건 Idle
	{
		m_pModelCom->SetUp_Animation(m_pModelCom->Get_EndNextAnimationIndex(), true);
	}


	PxVec3 vStartPos = { m_WorldMatrix.m[1][0] , m_WorldMatrix.m[1][1] , m_WorldMatrix.m[1][2] };
	PxVec3 vEndPos = { -m_WorldMatrix.m[1][0], -m_WorldMatrix.m[1][1] , -m_WorldMatrix.m[1][2] };

	m_PxTransform = PxTransformFromSegment(vStartPos, vEndPos);

	m_PxTransform.p = { m_WorldMatrix.m[3][0], m_WorldMatrix.m[3][1], m_WorldMatrix.m[3][2] };

	m_pPxRigidDynamic->setGlobalPose(m_PxTransform);


	if (true == m_pGameInstance->CollisionUpdate_PlayerToTriangleMeshGeometry(&m_vDir, &m_vDepth, m_pShape, &m_PxTransform))
	{
		_float4x4* pParentMatrix = const_cast<_float4x4*>(m_pParentMatrix);
		pParentMatrix->m[3][0] += m_vDir.x * m_vDepth;
		pParentMatrix->m[3][1] += m_vDir.y * m_vDepth;
		pParentMatrix->m[3][2] += m_vDir.z * m_vDepth;

		m_isCollision = true;
	}
	else
	{		
		// 만약 충돌을 안했는데 플레이어 Y와 메쉬의 Y 차이가 offset보다 크면 진짜 충돌을 안해서 중력이 들어가야하고
		// 충돌 안햇는데 플레이어 Y와 메쉬의 Y 차이가 offset보다 작거나 같으면 중력적용을 안해야해
		// 그럼 메쉬의 Y를 구해야한다.
		 
		PxVec3 rayOrigin = m_PxTransform.p;
		PxVec3 rayDirection = vEndPos;		// 내 업벡터의 반대방향

		// 레이 길이는 충분히 크게 설정
		_float rayLength = 100.0f; // 필요에 따라 조정 가능


		PxRaycastBuffer hitInfo;
		PxQueryFilterData data;
		data.flags = PxQueryFlag::eSTATIC;


		_bool status = m_pGameInstance->Get_Scene()->raycast(rayOrigin, rayDirection, rayLength, hitInfo , PxHitFlag::eDEFAULT , data);

		if (status && hitInfo.hasBlock)
		{
			_float fMeshY = hitInfo.block.position.y;
			_float fPlayerY = m_PxTransform.p.y;
			

			if (m_pPxRigidDynamic == hitInfo.block.actor)	// 지가 지랑 충돌한건 넘어가
			{
				int b = 1;
			}
			else
			{
				int a = 1;
			}


			if (static_cast<CPlayer*>(m_pOwner)->Get_OffsetY() > fabs(fPlayerY - fMeshY))
			{
				// 충돌은 안했는데 아직 메쉬y와 플레이어y의 차이가 크다면 중력줘 == 충돌안햇어
				m_isCollision = false;
			}
			else
			{
				// 충돌안햇는데 그 offset의 거리가 내가 설정한거보다 작거나 같다면 중력주지마 (붙어있는상태로 봐)
				// 밀어내진않고 그 위치 그대로
				m_isCollision = true;
			}
		}
		else
			m_isCollision = false;
		
	}	
}


void CBody_Player::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

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

	return S_OK;
}


void CBody_Player::Ready_Modify_Animation()
{
	vector<CAnimation*>& Animations = m_pModelCom->Get_Animations();

	//Animations[CPlayer::PLAYER_ANIMATIONID::HOOK_UP]->Set_SpeedPerSec(20.f);
	Animations[CPlayer::PLAYER_ANIMATIONID::HOOK_DOWN]->Set_SpeedPerSec(40.f);
}

HRESULT CBody_Player::Ready_PhysX()
{
	m_PxTransform = PxTransform(PxMat44(m_pTransformCom->Get_WorldMatrix_Ptr()->_11));

	m_pPxRigidDynamic = m_pGameInstance->Get_Physics()->createRigidDynamic(m_PxTransform);

	PxMaterial* Material = m_pGameInstance->Get_Material();


	vector<CMesh*>& Meshs = m_pModelCom->Get_Meshes();


	// 4. Triangle Mesh 기반 Shape 생성
	m_pShape = m_pGameInstance->Get_Physics()->createShape(PxCapsuleGeometry(5.f, 20.f), *Material);
	if (!m_pShape) {
		MSG_BOX(TEXT("createShape failed!"));
		return E_FAIL;
	}


	PxTransform localPose(PxVec3(0.f, 0.f, 0.f));  // (5.f, 0.f, 0.f) 위치로 이동
	m_pShape->setLocalPose(localPose);
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

	if(m_pPxRigidDynamic)
		m_pPxRigidDynamic->release();

	if (m_pShape)
		m_pShape->release();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
