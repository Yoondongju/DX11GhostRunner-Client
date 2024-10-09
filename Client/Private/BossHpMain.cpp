#include "stdafx.h"
#include "../Public/BossHpMain.h"
#include "GameInstance.h"

#include "Enemy.h"

#include "BossHpPanel.h"
#include "Elite.h"
#include "Hel.h"

CBossHpMain::CBossHpMain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CBossHpMain::CBossHpMain(const CBossHpMain& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CBossHpMain::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBossHpMain::Initialize(void* pArg)
{
	BOSSHP_DESC* pDesc = static_cast<BOSSHP_DESC*>(pArg);
	m_pOwner = pDesc->pOwner;
	Safe_AddRef(m_pOwner);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_fOriginalSizeX = m_fSizeX;
	m_fOriginalfX = m_fX;

	return S_OK;
}

void CBossHpMain::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);
}

void CBossHpMain::Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;

	CBossHpPanel::BOSSTYPE eBossType = static_cast<CBossHpPanel*>(m_pParentUI)->Get_BossType();

	switch (eBossType)
	{
	case Client::CBossHpPanel::ELITE:
	{
		_float fCurHp = static_cast<CElite*>(m_pOwner)->Get_Hp();
		_float fRatio = fCurHp / 100.f;


		m_fSizeX = m_fOriginalSizeX * fRatio;

		if (m_fSizeX <= 0.f)
			m_fSizeX = 1.f;

		_float fSizeDifference = (m_fOriginalSizeX - m_fSizeX) * 0.5f;
		m_fX = m_fOriginalfX - fSizeDifference;
	}
	break;
	case Client::CBossHpPanel::HEL:
	{
		_float fCurHp = static_cast<CHel*>(m_pOwner)->Get_Hp();
		_float fRatio = fCurHp / 100.f;


		m_fSizeX = m_fOriginalSizeX * fRatio;

		if (m_fSizeX <= 0.f)
			m_fSizeX = 1.f;

		_float fSizeDifference = (m_fOriginalSizeX - m_fSizeX) * 0.5f;
		m_fX = m_fOriginalfX - fSizeDifference;
	}
	break;
	case Client::CBossHpPanel::TYPE_END:
		break;
	default:
		break;
	}



	for (auto& pChildUI : m_childUI_List)
		pChildUI->Update(fTimeDelta);
}

void CBossHpMain::Late_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);
}

HRESULT CBossHpMain::Render()
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

	return S_OK;
}




HRESULT CBossHpMain::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_IconUI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossHp"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}



CBossHpMain* CBossHpMain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossHpMain* pInstance = new CBossHpMain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBossHpMain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CBossHpMain::Clone(void* pArg)
{
	CBossHpMain* pInstance = new CBossHpMain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBossHpMain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossHpMain::Free()
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
