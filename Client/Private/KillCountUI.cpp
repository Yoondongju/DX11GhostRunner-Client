#include "stdafx.h"
#include "../Public/KillCountUI.h"
#include "GameInstance.h"



CKillCountUI::CKillCountUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CKillCountUI::CKillCountUI(const CKillCountUI& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CKillCountUI::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CKillCountUI::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_vTargetSize = { 70.f, 70.f };
	m_bActivate = false;

	return S_OK;
}

void CKillCountUI::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);
}

void CKillCountUI::Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;

	if (m_fDiffTime < 1.f)
		m_fDiffTime += fTimeDelta * 4.f;
	else
		m_fDiffTime = 1.f;

	if(false == m_pGameInstance->Check_IsPlaying(SOUND_KILL_UI))
		m_pGameInstance->Play_Sound(TEXT("KillCount.ogg"), SOUND_KILL_UI, 7.f);

	if (m_fUnActiveTime >= 0.5f)
	{
		m_fSizeX = 200.f;
		m_fSizeY = 200.f;
		m_fUnActiveTime = 0.f;
		m_fDiffTime = 0.f;

		m_bActivate = false;
		return;
	}

	if (m_fSizeX > m_vTargetSize.x)
	{
		m_fSizeX -= fTimeDelta * 400.f;
	}
	else
	{
		m_fSizeX = m_vTargetSize.x;
		m_fUnActiveTime += fTimeDelta;
	}
		
	if (m_fSizeY > m_vTargetSize.y)
	{
		m_fSizeY -= fTimeDelta * 400.f;
	}
	else
	{
		m_fSizeY = m_vTargetSize.y;
		m_fUnActiveTime += fTimeDelta;
	}
		


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Update(fTimeDelta);
}

void CKillCountUI::Late_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);
}

HRESULT CKillCountUI::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;



	_bool bCheck = true;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bClicked", &bCheck, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ClickedColor", &m_clickedColor, sizeof(_float4))))
		return E_FAIL;

	m_fDiffTime = 1.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiff", &m_fDiffTime, sizeof(_float))))
		return E_FAIL;
	

	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}




HRESULT CKillCountUI::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_IconUI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_KillCount"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;



	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}



CKillCountUI* CKillCountUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKillCountUI* pInstance = new CKillCountUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CKillCountUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CKillCountUI::Clone(void* pArg)
{
	CKillCountUI* pInstance = new CKillCountUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CKillCountUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKillCountUI::Free()
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
