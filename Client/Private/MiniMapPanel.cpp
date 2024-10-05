#include "stdafx.h"
#include "../Public/MiniMapPanel.h"
#include "GameInstance.h"



CMiniMapPanel::CMiniMapPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CMiniMapPanel::CMiniMapPanel(const CMiniMapPanel& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CMiniMapPanel::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CMiniMapPanel::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Create_ChildUI()))
		return E_FAIL;
	




	return S_OK;
}

void CMiniMapPanel::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);
}

void CMiniMapPanel::Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;



	for (auto& pChildUI : m_childUI_List)
		pChildUI->Update(fTimeDelta);
}

void CMiniMapPanel::Late_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);
}

HRESULT CMiniMapPanel::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;



	_bool bCheck = false;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bClicked", &bCheck, sizeof(_bool))))
		return E_FAIL;




	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}




HRESULT CMiniMapPanel::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_IconUI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MiniMapPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}


HRESULT CMiniMapPanel::Create_ChildUI()
{
	CUIObject::UI_DESC MiniMapMarkerDesc = {};

	MiniMapMarkerDesc.fX = 0;
	MiniMapMarkerDesc.fY = 0;
	MiniMapMarkerDesc.fSizeX = 20;
	MiniMapMarkerDesc.fSizeY = 20;
	MiniMapMarkerDesc.fSpeedPerSec = 10.f;
	MiniMapMarkerDesc.fRotationPerSec = XMConvertToRadians(90.0f);



	CUIObject* pChildUI = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_MiniMapPlayer", &MiniMapMarkerDesc));
	pChildUI->SetParentUI(this);
	m_childUI_List.emplace_back(pChildUI);

	return S_OK;
}



CMiniMapPanel* CMiniMapPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniMapPanel* pInstance = new CMiniMapPanel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMiniMapPanel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CMiniMapPanel::Clone(void* pArg)
{
	CMiniMapPanel* pInstance = new CMiniMapPanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMiniMapPanel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniMapPanel::Free()
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

