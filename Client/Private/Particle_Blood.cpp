#include "stdafx.h"
#include "..\Public\Particle_Blood.h"

#include "GameInstance.h"

CParticle_Blood::CParticle_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CParticle_Blood::CParticle_Blood(const CParticle_Blood& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CParticle_Blood::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Blood::Initialize(void* pArg)
{
	BLOOD_DESC* pDesc = static_cast<CParticle_Blood::BLOOD_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	return S_OK;
}

void CParticle_Blood::Priority_Update(_float fTimeDelta)
{

}

void CParticle_Blood::Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
	{
		if (m_fDisableTime <= 0.f)
		{
			m_isActiveMyParticle = false;
			m_fDisableTime = 1.f;
			m_pVIBufferCom->ResetTranslation();
		}

		m_fDisableTime -= fTimeDelta;



		_int iRandom = m_pGameInstance->Get_Random_Interger(0, 3);
		switch (iRandom)
		{
		case 0:
			m_pVIBufferCom->Spread(fTimeDelta * 2.f);
			break;
		case 1:
			m_pVIBufferCom->Spread2(fTimeDelta * 2.f);
			break;
		case 2:
			m_pVIBufferCom->Spread3(fTimeDelta * 2.f);
			break;
		default:
			break;
		}		
	}
	else
	{
		_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}
		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
	}
}

void CParticle_Blood::Late_Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_Blood::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	_uint iRow = 3;
	_uint iCol = 3;

	_uint iRandomCoord = m_pGameInstance->Get_Random(0, iRow * iRow);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iAtlasRow", &iRow, sizeof(iRow))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iAtlasCol", &iCol, sizeof(iCol))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iAtlasRandomIndex", &iRandomCoord, sizeof(iRandomCoord))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Blood::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Blood"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Blood1Instance"),  // 1말고 다른 피들은 모델자체가 이상함
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_Blood* CParticle_Blood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Blood* pInstance = new CParticle_Blood(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Blood"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CParticle_Blood::Clone(void* pArg)
{
	CParticle_Blood* pInstance = new CParticle_Blood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Blood"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Blood::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
