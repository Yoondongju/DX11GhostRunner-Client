#include "stdafx.h"
#include "Decorative_Object.h"
#include "GameInstance.h"


CDecorative_Object::CDecorative_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{

}

CDecorative_Object::CDecorative_Object(const CDecorative_Object& Prototype)
    : CGameObject(Prototype)
{

}

HRESULT CDecorative_Object::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDecorative_Object::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CDecorative_Object::Priority_Update(_float fTimeDelta)
{
}

void CDecorative_Object::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
  
    
   

}

void CDecorative_Object::Late_Update(_float fTimeDelta)
{

        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CDecorative_Object::Render()
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

HRESULT CDecorative_Object::Ready_Component()
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

CDecorative_Object* CDecorative_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDecorative_Object* pInstance = new CDecorative_Object(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDecorative_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDecorative_Object::Clone(void* pArg)
{
    CDecorative_Object* pInstance = new CDecorative_Object(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDecorative_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDecorative_Object::Free()
{
    __super::Free();


    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);
}
