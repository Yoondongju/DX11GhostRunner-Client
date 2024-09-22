#include "stdafx.h"
#include "..\Public\Weapon_Player.h"

#include "Player.h"

#include "GameInstance.h"


CWeapon_Player::CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon_Player::CWeapon_Player(const CWeapon_Player& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CWeapon_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Player::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;


	m_pSubShuriken[0] = pDesc->pSubShuriSken[0];
	m_pSubShuriken[1] = pDesc->pSubShuriSken[1];




	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_KatanaOriginMatrix = {
		0.181342006, 0.655698836, 0.207122847, 0.00000000,
		-0.963511229, 0.238839343, 0.0874769986, 0.00000000,
		0.0110353436, -0.301334143, 0.944287121, 0.00000000,
		0.0750860050, -0.0355550535, 0.0480655581, 1.00000000
	};

	m_ShurikenOriginMatrix = {
		0.181342006, 0.655698836, 0.207122847, 0.00000000,
		-0.963511229, 0.238839343, 0.0874769986, 0.00000000,
		0.0110353436, -0.301334143, 0.944287121, 0.00000000,
		0.0750860050, -0.0355550535, 0.0480655581, 1.00000000
	};



	XMStoreFloat4x4(&m_KatanaOriginMatrix, XMMatrixMultiply( XMLoadFloat4x4(&m_KatanaOriginMatrix), XMMatrixRotationZ(XMConvertToRadians(90.0f))));

	m_pTransformCom->Set_WorldMatrix(m_KatanaOriginMatrix);

	return S_OK;
}

void CWeapon_Player::Priority_Update(_float fTimeDelta)
{

}

void CWeapon_Player::Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}


	if (m_eCurType == SHURIKEN)
	{
		m_pTransformCom->Set_WorldMatrix(m_ShurikenOriginMatrix);
	
		if (false == m_isAttacking)
		{
			XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

			m_isActiveMyParticle = false;

			m_pSubShuriken[0]->SetActiveMyParticle(false);
			m_pSubShuriken[1]->SetActiveMyParticle(false);
		}		
		else
		{
			m_isActiveMyParticle = true;

			m_pSubShuriken[0]->SetActiveMyParticle(true);
			m_pSubShuriken[1]->SetActiveMyParticle(true);
		}
	}
	else
	{
		m_pTransformCom->Set_WorldMatrix(m_KatanaOriginMatrix);
		m_isActiveMyParticle = false;

		m_pSubShuriken[0]->SetActiveMyParticle(false);
		m_pSubShuriken[1]->SetActiveMyParticle(false);

		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
	}
		
	



	m_pColliderCom[m_eCurType]->Update(&m_WorldMatrix);

	if (m_eCurType == SHURIKEN)
	{
		for (_uint i = 0; i < 2; i++)
		{
			m_pSubShuriken[i]->Update(&m_WorldMatrix , m_isAttacking, fTimeDelta);
		}
	}
}

void CWeapon_Player::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	if (m_eCurType == SHURIKEN)
	{
		for (_uint i = 0; i < 2; i++)
		{
			m_pSubShuriken[i]->Late_Update(fTimeDelta);
		}
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom[m_eCurType]);
#endif
}

HRESULT CWeapon_Player::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	

	_uint		iNumMeshes = m_pModelCom[m_eCurType]->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom[m_eCurType]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom[m_eCurType]->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CWeapon_Player::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Katana"),
		TEXT("Com_Katana"), reinterpret_cast<CComponent**>(&m_pModelCom[WEAPON_TYPE::KATANA]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Shuriken"),
		TEXT("Com_Shuriken"), reinterpret_cast<CComponent**>(&m_pModelCom[WEAPON_TYPE::SHURIKEN]))))
		return E_FAIL;



	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(2.f, 2.f, 11.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vExtents.z);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_OBB"),
		TEXT("Com_Collider_Katana"), reinterpret_cast<CComponent**>(&m_pColliderCom[WEAPON_TYPE::KATANA]), &ColliderDesc)))
		return E_FAIL;


	/* FOR.Com_Collider */
	ColliderDesc.vExtents = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vExtents.z);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_OBB"),
		TEXT("Com_Collider_Shuriken"), reinterpret_cast<CComponent**>(&m_pColliderCom[WEAPON_TYPE::SHURIKEN]), &ColliderDesc)))
		return E_FAIL;







	
	return S_OK;
}



CWeapon_Player* CWeapon_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Player* pInstance = new CWeapon_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Player::Clone(void* pArg)
{
	CWeapon_Player* pInstance = new CWeapon_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Player::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	for (_uint i = 0; i < WEAPON_TYPE::WEAPON_TYPE_END; i++)
	{
		Safe_Release(m_pModelCom[i]);
	}

	for (_uint i = 0; i < WEAPON_TYPE::WEAPON_TYPE_END; i++)
	{
		Safe_Release(m_pColliderCom[i]);
	}

	for (_uint i = 0; i < 2; i++)
	{
		Safe_Release(m_pSubShuriken[i]);
	}
	

}
