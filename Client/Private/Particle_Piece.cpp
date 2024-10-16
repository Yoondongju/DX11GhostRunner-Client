#include "stdafx.h"
#include "..\Public\Particle_Piece.h"

#include "GameInstance.h"

#include "Hel.h"

CParticle_Piece::CParticle_Piece(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CParticle_Piece::CParticle_Piece(const CParticle_Piece& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CParticle_Piece::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Piece::Initialize(void* pArg)
{
	PARTICLE_DESC* pDesc = static_cast<CParticle_Piece::PARTICLE_DESC*>(pArg);

	m_SpwanPositionLocal = pDesc->SpwanPositionLocal;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void CParticle_Piece::Priority_Update(_float fTimeDelta)
{

}

void CParticle_Piece::Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
	{
		if (m_fDisableTime <= 0.f)
		{
			m_isActiveMyParticle = false;
			m_fDisableTime = 1.f;
			m_pVIBufferCom->ResetTranslation();

			m_iRandomSpread = m_pGameInstance->Get_Random_Interger(0, 2);
		}

		m_fDisableTime -= fTimeDelta;


		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));
		_vector vWorldPos = XMVector3TransformCoord(XMLoadFloat3(&m_SpwanPositionLocal), XMLoadFloat4x4(&m_WorldMatrix));


		m_WorldMatrix.m[3][0] = vWorldPos.m128_f32[0];
		m_WorldMatrix.m[3][1] = vWorldPos.m128_f32[1] + 45.f;
		m_WorldMatrix.m[3][2] = vWorldPos.m128_f32[2];

		if (CHel::HEL_ANIMATION::BACK_BLOCK == m_pOwner->Get_Fsm()->Get_CurStateIndex())
		{
			m_WorldMatrix.m[3][1] = vWorldPos.m128_f32[1] + 75.f;
			m_WorldMatrix.m[3][2] = vWorldPos.m128_f32[2] - 8.f;

			m_iRandomSpread = 0;
		}

		switch (m_iRandomSpread)
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
}

void CParticle_Piece::Late_Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_Piece::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Piece::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleBlock"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_PieceInstance"),  // 1말고 다른 피들은 모델자체가 이상함
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_Piece* CParticle_Piece::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Piece* pInstance = new CParticle_Piece(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Piece"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CParticle_Piece::Clone(void* pArg)
{
	CParticle_Piece* pInstance = new CParticle_Piece(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Piece"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Piece::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
