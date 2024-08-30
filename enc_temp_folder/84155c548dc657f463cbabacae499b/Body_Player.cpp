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
	/* ���������� ���� �����͵��� ��� ���ϳ�. */
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
	if (true == m_pModelCom->Play_Animation(fTimeDelta))			// �ִϸ��̼��� �������� �� ������ Idle
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
		// ���� �浹�� ���ߴµ� �÷��̾� Y�� �޽��� Y ���̰� offset���� ũ�� ��¥ �浹�� ���ؼ� �߷��� �����ϰ�
		// �浹 ���޴µ� �÷��̾� Y�� �޽��� Y ���̰� offset���� �۰ų� ������ �߷������� ���ؾ���
		// �׷� �޽��� Y�� ���ؾ��Ѵ�.
		 
		PxVec3 rayOrigin = m_PxTransform.p;
		PxVec3 rayDirection = vEndPos;		// �� �������� �ݴ����

		// ���� ���̴� ����� ũ�� ����
		_float rayLength = 100.0f; // �ʿ信 ���� ���� ����


		PxRaycastBuffer hitInfo;
		PxQueryFilterData data;
		data.flags = PxQueryFlag::eSTATIC;


		_bool status = m_pGameInstance->Get_Scene()->raycast(rayOrigin, rayDirection, rayLength, hitInfo , PxHitFlag::eDEFAULT , data);

		if (status && hitInfo.hasBlock)
		{
			_float fMeshY = hitInfo.block.position.y;
			_float fPlayerY = m_PxTransform.p.y;
			

			if (m_pPxRigidDynamic == hitInfo.block.actor)	// ���� ���� �浹�Ѱ� �Ѿ
			{
				int b = 1;
			}
			else
			{
				int a = 1;
			}


			if (static_cast<CPlayer*>(m_pOwner)->Get_OffsetY() > fabs(fPlayerY - fMeshY))
			{
				// �浹�� ���ߴµ� ���� �޽�y�� �÷��̾�y�� ���̰� ũ�ٸ� �߷��� == �浹���޾�
				m_isCollision = false;
			}
			else
			{
				// �浹���޴µ� �� offset�� �Ÿ��� ���� �����Ѱź��� �۰ų� ���ٸ� �߷������� (�پ��ִ»��·� ��)
				// �о���ʰ� �� ��ġ �״��
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


	// 4. Triangle Mesh ��� Shape ����
	m_pShape = m_pGameInstance->Get_Physics()->createShape(PxCapsuleGeometry(5.f, 20.f), *Material);
	if (!m_pShape) {
		MSG_BOX(TEXT("createShape failed!"));
		return E_FAIL;
	}


	PxTransform localPose(PxVec3(0.f, 0.f, 0.f));  // (5.f, 0.f, 0.f) ��ġ�� �̵�
	m_pShape->setLocalPose(localPose);
	m_pPxRigidDynamic->attachShape(*m_pShape);
	m_pGameInstance->Get_Scene()->addActor(*m_pPxRigidDynamic);


	// �浹����ϴ°� �����ҷ����� 
	// ���� ���� ��ü���� 
	// ��ŸƮ��� �� �����̰�   ������� �� ���� �ݴ��� 
	// PxVec3�ٲܷ��� �� _float3�� _vector�� ���� x,y,z�ϳ��������ؼ� ���������Ѵ�
	// m_Tranform == Px���� �����ϴ� Ʈ�������̰�  
	// �� ������ Ʈ��������ġ�� m_Tranform�� �����ؾ��ϴµ� �̰����� PxTransformSegment��� �Լ��� �����ؾ��Ѵ�.

	// m_Tranform.p�� �� ��ġ��
	// PxTriangleMeshGeometry�� ���������� 
	// PX��ǻƮƮ���̾ޱ۸޽����Ʈ�����̼� �� �Լ��� �浹�񱳸��Ѵ�
	//  setGlobalPose�� �� PXTransform �� ������ �������ش�



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
