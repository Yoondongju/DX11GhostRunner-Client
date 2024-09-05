#include "stdafx.h"
#include "..\Public\GrapplingPointUI.h"

#include "GameInstance.h"

#include "Static_Object.h"
#include "PipeLine.h"

CGrapplingPointUI::CGrapplingPointUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CGrapplingPointUI::CGrapplingPointUI(const CGrapplingPointUI& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CGrapplingPointUI::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CGrapplingPointUI::Initialize(void* pArg)
{
	GRAPPLINGPOINT_DESC* pDesc = static_cast<GRAPPLINGPOINT_DESC*>(pArg);
	m_eType = pDesc->eType;

	if (nullptr != pDesc->pCrane)
	{
		m_pCrane = pDesc->pCrane;
		Safe_AddRef(m_pCrane);
	}
	


	UI_DESC			Desc{};

	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = 60;
	Desc.fSizeY = 60;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	if (GRAPPLINGPOINT_TYPE::TYPE_OUTRING == m_eType)
	{
		GRAPPLINGPOINT_DESC Desc = { GRAPPLINGPOINT_TYPE::TYPE_FRAME , nullptr };
		CUIObject* pFrameUI = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_GrapplingPointUI"),&Desc));

		Desc.eType = GRAPPLINGPOINT_TYPE::TYPE_DOT;
		CUIObject* pDotUI = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_GrapplingPointUI"), &Desc));

		if (FAILED(Add_Child(pFrameUI)))
			return E_FAIL;
		if (FAILED(Add_Child(pDotUI)))
			return E_FAIL;

		m_bActivate = false;	// 부모만 꺼줘도 어짜피 자식들 안돌아
	}

	


	return S_OK;
}

void CGrapplingPointUI::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;
	

	for (auto& pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);	
}

void CGrapplingPointUI::Update(_float fTimeDelta)
{
	if (m_eType == GRAPPLINGPOINT_TYPE::TYPE_OUTRING)
	{
		m_bFindPlayer = m_bActivate = Find_Player(fTimeDelta);
	}
	
	if (nullptr != m_pCrane)	// 젤 부모
	{
		_vector vCranePos = m_pCrane->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
		_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

		_vector NDCPos = XMVector3TransformCoord(XMVector3TransformCoord(vCranePos, ViewMatrix), ProjMatrix);

		_float fScreenX = (( XMVectorGetX(NDCPos) + 1.0f) / 2.0f) * m_fViewWidth;
		_float fScreenY = ((1.0f - XMVectorGetY(NDCPos)) / 2.0f) * m_fViewHeight;


		m_fX = fScreenX;
		m_fY = fScreenY;
	}
	else						// 자식들은
	{
		m_fX = m_pParentUI->Get_fX();
		m_fY = m_pParentUI->Get_fY();

		m_fAlpha = static_cast<CGrapplingPointUI*>(m_pParentUI)->Get_Alpha();
	}
	
	if (false == m_bActivate)
		return;




	_float fRotationSpeed = 1.7f;

	switch (m_eType)
	{
	case Client::CGrapplingPointUI::TYPE_DOT:
		break;
	case Client::CGrapplingPointUI::TYPE_FRAME:
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * fRotationSpeed);
	}
		break;
	case Client::CGrapplingPointUI::TYPE_OUTRING:
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * fRotationSpeed * -1.5f);
	}
		break;
	case Client::CGrapplingPointUI::TYPE_END:
		break;
	default:
		break;
	}



	



	
	for (auto& pChildUI : m_childUI_List)
		pChildUI->Update(fTimeDelta);
}

void CGrapplingPointUI::Late_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);
}

HRESULT CGrapplingPointUI::Render()
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

	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha",&m_fAlpha, sizeof(_float))))
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


_bool CGrapplingPointUI::Find_Player(_float fTimeDelta)
{
	_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_vector vCranePos =  m_pCrane->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_float fXdistance = fabs(XMVectorGetX(vCranePos) - XMVectorGetX(vPlayerPos));
	_float fZdistance = fabs(XMVectorGetZ(vCranePos) - XMVectorGetZ(vPlayerPos));



	if (fabs(400 >= fXdistance &&
		600.f >= fZdistance))
	{
		if (m_fAlpha < 1.f)
			m_fAlpha += fTimeDelta * 0.7f;
		else
			m_fAlpha = 1.f;

		return true;
	}	
	else
	{
		// 거리가 멀어졌어
		if (m_fAlpha > 0.f)
		{
			m_fAlpha -= fTimeDelta * 0.9;
			return true;
		}
		else
		{
			m_fAlpha = 0.f;
			return false;
		}
	}

}




HRESULT CGrapplingPointUI::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	switch (m_eType)
	{
	case Client::CGrapplingPointUI::TYPE_DOT:
	{
		/* FOR.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grappling_point_dot"),
			TEXT("Com_Texture_Dot"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CGrapplingPointUI::TYPE_FRAME:
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grappling_point_frame"),
			TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CGrapplingPointUI::TYPE_OUTRING:
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grappling_point_outer_ring"),
			TEXT("Com_Texture_OuterRing"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CGrapplingPointUI::TYPE_END:
		break;
	default:
		break;
	}
	

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}



CGrapplingPointUI* CGrapplingPointUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGrapplingPointUI* pInstance = new CGrapplingPointUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGrapplingPointUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CGrapplingPointUI::Clone(void* pArg)
{
	CGrapplingPointUI* pInstance = new CGrapplingPointUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CGrapplingPointUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrapplingPointUI::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pTextureCom);

	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pCrane);


	for (auto& pUI : m_childUI_List)
	{
		Safe_Release(pUI);
	}
	m_childUI_List.clear();

}
