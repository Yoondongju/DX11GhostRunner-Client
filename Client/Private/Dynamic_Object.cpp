#include "stdafx.h"
#include "Dynamic_Object.h"
#include "GameInstance.h"

#include "Terrain.h"

#include "LandObject.h"
#include "Mesh.h"

#pragma comment(lib, "PhysXCooking_64.lib")

CDynamic_Object::CDynamic_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CDynamic_Object::CDynamic_Object(const CDynamic_Object& Prototype)
    : CGameObject(Prototype)
{

}

HRESULT CDynamic_Object::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDynamic_Object::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;


    //if (FAILED(Ready_PhysX()))
    //    return E_FAIL;




    return S_OK;
}

void CDynamic_Object::Priority_Update(_float fTimeDelta)
{

}

void CDynamic_Object::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);



    

}

void CDynamic_Object::Late_Update(_float fTimeDelta)
{
   
}

HRESULT CDynamic_Object::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4((CPipeLine::D3DTS_VIEW)))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    _float fAlpha = 0.7f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
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

HRESULT CDynamic_Object::Ready_Component()
{
    


    return S_OK;
}




CDynamic_Object* CDynamic_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDynamic_Object* pInstance = new CDynamic_Object(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDynamic_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDynamic_Object::Clone(void* pArg)
{
    CDynamic_Object* pInstance = new CDynamic_Object(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDynamic_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDynamic_Object::Free()
{
    __super::Free();




    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);
}
