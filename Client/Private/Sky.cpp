#include "stdafx.h"
#include "..\Public\Sky.h"

#include "GameInstance.h"
#include "FreeCamera.h"

CSky::CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSky::CSky(const CSky& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CSky::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSky::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pFreeCameraTransform = m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY)->Get_Transform();
	Safe_AddRef(m_pFreeCameraTransform);

	return S_OK;
}

void CSky::Priority_Update(_float fTimeDelta)
{

}

void CSky::Update(_float fTimeDelta)
{
	
}

void CSky::Late_Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta * 0.5f;

	

	m_pGameInstance->Add_RenderObject(CRenderer::RG_PRIORITY, this);
}

HRESULT CSky::Render()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pFreeCameraTransform->Get_State(CTransform::STATE_POSITION));


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
		return E_FAIL;


	_uint iPassNum = 0;
	if (LEVEL_STAGE2_BOSS == g_CurLevel)
	{
		iPassNum = 1;
	}



	if (FAILED(m_pShaderCom->Begin(iPassNum)))
		return E_FAIL;


	if (FAILED(m_pModelCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pModelCom->Render()))
		return E_FAIL;
	
	

	return S_OK;
}

HRESULT CSky::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	///* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky_Blue"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CSky* CSky::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSky* pInstance = new CSky(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSky"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSky::Clone(void* pArg)
{
	CSky* pInstance = new CSky(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSky"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pFreeCameraTransform);
}
