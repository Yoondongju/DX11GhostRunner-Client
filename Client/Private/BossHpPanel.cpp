#include "stdafx.h"
#include "../Public/BossHpPanel.h"
#include "GameInstance.h"

#include "BossHpMain.h"
#include "BossHpEnergy.h"

#include "Enemy.h"

CBossHpPanel::CBossHpPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CBossHpPanel::CBossHpPanel(const CBossHpPanel& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CBossHpPanel::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBossHpPanel::Initialize(void* pArg)
{
	BOSSHP_DESC* pDesc = static_cast<BOSSHP_DESC*>(pArg);

	m_pOwner = pDesc->pOwner;
	Safe_AddRef(m_pOwner);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;



	return S_OK;
}

void CBossHpPanel::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);
}

void CBossHpPanel::Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;



	for (auto& pChildUI : m_childUI_List)
		pChildUI->Update(fTimeDelta);
}

void CBossHpPanel::Late_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);
}

HRESULT CBossHpPanel::Render()
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


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();



	_float	fFontSize = 0.8f;
	_uint iTextLength = wcslen(L"Elite");		// 글자 길이

	_float fPosX = (g_iWinSizeX - iTextLength * 15) * 0.5f;

	m_pGameInstance->Render_Text(TEXT("Font_145"), L"Elite",
		XMVectorSet(fPosX, m_fY - 50.f, 0.f, 1.f),
		fFontSize,
		XMVectorSet(1.f, 1.f, 1.f, 1.f),
		0.f,
		XMVectorSet(0.f, 0.f, 0.f, 1.f));


	return S_OK;
}



HRESULT CBossHpPanel::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_IconUI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossPanel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossHpPanel::Ready_Child()
{
	CBossHpMain::BOSSHP_DESC MainDesc = {};
	MainDesc.pOwner = m_pOwner;
	MainDesc.fX = m_fX;
	MainDesc.fY = m_fY;
	MainDesc.fSizeX = m_fSizeX;
	MainDesc.fSizeY = m_fSizeY;
	MainDesc.fSpeedPerSec = 10.f;
	MainDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CUIObject* pChildMain = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_BossHpMain", &MainDesc));
	pChildMain->SetParentUI(this);
	m_childUI_List.emplace_back(pChildMain);


	CBossHpEnergy::BOSSHP_DESC EnergyDesc = {};
	EnergyDesc.pOwner = m_pOwner;
	EnergyDesc.fX = m_fX;
	EnergyDesc.fY = m_fY;
	EnergyDesc.fSizeX = m_fSizeX;
	EnergyDesc.fSizeY = m_fSizeY;
	EnergyDesc.fSpeedPerSec = 10.f;
	EnergyDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CUIObject* pChildEnergy = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_BossHpEnergy", &EnergyDesc));
	pChildEnergy->SetParentUI(this);
	m_childUI_List.emplace_back(pChildEnergy);

	return S_OK;
}



CBossHpPanel* CBossHpPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossHpPanel* pInstance = new CBossHpPanel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBossHpPanel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CBossHpPanel::Clone(void* pArg)
{
	CBossHpPanel* pInstance = new CBossHpPanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBossHpPanel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossHpPanel::Free()
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

	Safe_Release(m_pOwner);
}
