#include "stdafx.h"
#include "..\Public\Wire_Player.h"

#include "GameInstance.h"
#include "Player.h"
#include "GrapplingPointUI.h"
#include "Static_Object.h"

#include "Player_Hook.h"

CWire_Player::CWire_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWire_Player::CWire_Player(const CWire_Player& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CWire_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWire_Player::Initialize(void* pArg)
{
	WIRE_DESC* pDesc = static_cast<WIRE_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;


	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;





	return S_OK;
}

void CWire_Player::Priority_Update(_float fTimeDelta)
{

}



void CWire_Player::Update(_float fTimeDelta)
{
	if (false == m_bActive)
		return;


	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));


	_vector vStartPos = XMLoadFloat3((_float3*)m_WorldMatrix.m[3]);



	CFsm* pFsm = m_pOwner->Get_Fsm();

	CGrapplingPointUI* pGrapplingPoint = static_cast<CPlayer_Hook*>(pFsm->Get_CurState())->Get_GrapUI();
	_vector vGrapPointPos = pGrapplingPoint->Get_Crane()->Get_Transform()->Get_State(CTransform::STATE_POSITION);


	m_pVIBufferCom->Update_VertexData(vStartPos, vGrapPointPos);
}

void CWire_Player::Late_Update(_float fTimeDelta)
{
	if (false == m_bActive)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CWire_Player::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom[0]->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom[1]->Bind_ShadeResource(m_pShaderCom, "g_NoiseTexture", 0)))
		return E_FAIL;

	


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWire_Player::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleShuriken"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[0]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_DeadNoiseTexture"),
		TEXT("Com_NoiseTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[1]))))
		return E_FAIL;


	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Wire"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CWire_Player* CWire_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWire_Player* pInstance = new CWire_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWire_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWire_Player::Clone(void* pArg)
{
	CWire_Player* pInstance = new CWire_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWire_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWire_Player::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pTextureCom[0]);
	Safe_Release(m_pTextureCom[1]);

	Safe_Release(m_pVIBufferCom);
}
