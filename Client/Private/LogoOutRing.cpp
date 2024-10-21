#include "stdafx.h"
#include "..\Public\LogoOutRing.h"

#include "GameInstance.h"

CLogoOutRing::CLogoOutRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CLogoOutRing::CLogoOutRing(const CLogoOutRing& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CLogoOutRing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogoOutRing::Initialize(void* pArg)
{
	UI_DESC			Desc{};

	Desc.fX = g_iWinSizeX - 200;
	Desc.fY = g_iWinSizeY - 200;
	Desc.fSizeX = 200;
	Desc.fSizeY = 200;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CLogoOutRing::Priority_Update(_float fTimeDelta)
{

}

void CLogoOutRing::Update(_float fTimeDelta)
{
	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * 2.f);
	__super::Late_Update(fTimeDelta);
}

void CLogoOutRing::Late_Update(_float fTimeDelta)
{
	
}

HRESULT CLogoOutRing::Render()
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

HRESULT CLogoOutRing::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LogoOutRing"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CLogoOutRing* CLogoOutRing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLogoOutRing* pInstance = new CLogoOutRing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLogoOutRing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CLogoOutRing::Clone(void* pArg)
{
	CLogoOutRing* pInstance = new CLogoOutRing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLogoOutRing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoOutRing::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
