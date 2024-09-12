#include "stdafx.h"
#include "..\Public\NumberUI.h"

#include "GameInstance.h"

#include "Player.h"
#include "Weapon_Player.h"

CNumberUI::CNumberUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CNumberUI::CNumberUI(const CNumberUI& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CNumberUI::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CNumberUI::Initialize(void* pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_vOriginSize.x = m_fSizeX;
	m_vOriginSize.y = m_fSizeY;


	m_vTargetSize.x = m_fSizeX + 50.f;
	m_vTargetSize.y = m_fSizeY + 50.f;

	m_bActivate = false;

	return S_OK;
}

void CNumberUI::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);
}

void CNumberUI::Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;

	
	// 숫자가 쫘아악 커지고
	// 숫자가 다음숫자로 넘어갈때
	// 원래 사이즈로 바꾼다

	if (m_iPreNumber != m_iNumber)
	{
		m_bChangeEnter = false;

		m_fAlpha = 1.f;
		m_fSizeX = m_vOriginSize.x;
		m_fSizeY = m_vOriginSize.y;
	}

	if (!m_bChangeEnter && 
		m_vTargetSize.x > m_fSizeX &&
		m_vTargetSize.y > m_fSizeY)
	{
		m_fAlpha -= fTimeDelta * 3.f;
		m_fSizeX += fTimeDelta * 110.f;
		m_fSizeY += fTimeDelta * 110.f;
	}
	else
	{
		m_bChangeEnter = true;
	}

	
	m_iPreNumber = m_iNumber;

	for (auto& pChildUI : m_childUI_List)
		pChildUI->Update(fTimeDelta);
}

void CNumberUI::Late_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);
}

HRESULT CNumberUI::Render()
{
	if (false == m_bActivate)
		return S_OK;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumber", &m_iNumber, sizeof(_int))))
		return E_FAIL;




	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}




HRESULT CNumberUI::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_NumberUI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;



	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Number"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;



	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}



CNumberUI* CNumberUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNumberUI* pInstance = new CNumberUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CNumberUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CNumberUI::Clone(void* pArg)
{
	CNumberUI* pInstance = new CNumberUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNumberUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNumberUI::Free()
{
	// 레이어 에서 얘의 Free 들어왓고
	// 자식들이 자기 부모를 계속 지운단말이야

	for (auto& pUI : m_childUI_List)
	{
		Safe_Release(pUI);
	}
	m_childUI_List.clear();

	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pTextureCom);

	Safe_Release(m_pVIBufferCom);

}
