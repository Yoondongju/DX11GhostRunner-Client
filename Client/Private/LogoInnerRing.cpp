#include "stdafx.h"
#include "..\Public\LogoInnerRing.h"

#include "GameInstance.h"

CLogoInnerRing::CLogoInnerRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CLogoInnerRing::CLogoInnerRing(const CLogoInnerRing& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CLogoInnerRing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogoInnerRing::Initialize(void* pArg)
{
	UI_DESC			Desc{};

	Desc.fX = g_iWinSizeX - 200;
	Desc.fY = g_iWinSizeY - 200;
	Desc.fSizeX = 170;
	Desc.fSizeY = 170;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CLogoInnerRing::Priority_Update(_float fTimeDelta)
{

}

void CLogoInnerRing::Update(_float fTimeDelta)
{
	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * 2.f);
	__super::Late_Update(fTimeDelta);
}

void CLogoInnerRing::Late_Update(_float fTimeDelta)
{
	
}

HRESULT CLogoInnerRing::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
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

HRESULT CLogoInnerRing::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LogoInnerRing"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CLogoInnerRing* CLogoInnerRing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLogoInnerRing* pInstance = new CLogoInnerRing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoInnerRing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CLogoInnerRing::Clone(void* pArg)
{
	CLogoInnerRing* pInstance = new CLogoInnerRing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLogoInnerRing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoInnerRing::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
