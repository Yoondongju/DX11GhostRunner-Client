#include "stdafx.h"
#include "Aid_props.h"
#include "GameInstance.h"


CAid_props::CAid_props(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{

}

CAid_props::CAid_props(const CAid_props& Prototype)
    : CGameObject(Prototype)
{

}

HRESULT CAid_props::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAid_props::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CAid_props::Priority_Update(_float fTimeDelta)
{
}

void CAid_props::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
  
    
   

}

void CAid_props::Late_Update(_float fTimeDelta)
{

        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CAid_props::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4((CPipeLine::D3DTS_VIEW)))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;



    _uint iNumMeshes = m_pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CAid_props::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    /* For.Com_Model*/
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strModelPrototypeName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    return S_OK;
}

CAid_props* CAid_props::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAid_props* pInstance = new CAid_props(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CAid_props"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAid_props::Clone(void* pArg)
{
    CAid_props* pInstance = new CAid_props(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CAid_props"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAid_props::Free()
{
    __super::Free();


    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);
}
