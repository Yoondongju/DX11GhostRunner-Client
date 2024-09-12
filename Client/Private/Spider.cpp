#include "stdafx.h"
#include "Spider.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "Player.h"

CSpider::CSpider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster(pDevice, pContext)
{

}


CSpider::CSpider(const CSpider& Prototype)
    : CMonster(Prototype)
{

}


HRESULT CSpider::Initialize_Prototype()
{
    return S_OK;
}


HRESULT CSpider::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_Change_Layer()))
        return E_FAIL;

    m_pModel->SetUp_Animation(0, true,0);

    m_pTransformCom->Scaling(2.f, 2.f, 2.f);

    CCollider* pCollider = static_cast<CWeapon_Player*>(static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY))->Get_Part(CPlayer::PART_WEAPON))->Get_Collider();

    m_pPlayerWeaponCollider = pCollider;
    Safe_AddRef(m_pPlayerWeaponCollider);

    return S_OK;
}

void CSpider::Priority_Update(_float fTimeDelta)
{

    //_vector vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
    //_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    //
    //_float fPlayerPosX = XMVectorGetX(vPlayerPos);
    //_float fPlayerPosZ = XMVectorGetZ(vPlayerPos);
    //
    //_float fMyPosX = XMVectorGetX(vMyPos);
    //_float fMyPosZ = XMVectorGetZ(vMyPos);
    //
    //
    //if (fabs(fPlayerPosX - fMyPosX) < 7.f && fabs(fPlayerPosZ - fMyPosZ) < 80.f)
    //{
    //    m_pModel->SetUp_Animation(5, true);
    //}
    //else
    //    m_pModel->SetUp_Animation(0, true);


}

void CSpider::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);


    m_pModel->Play_Animation(fTimeDelta);
}

void CSpider::Late_Update(_float fTimeDelta)
{
   
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

}

HRESULT CSpider::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4((CPipeLine::D3DTS_VIEW)))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
        return E_FAIL;



    _uint iNumMeshes = m_pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModel->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }


    return S_OK;
}

void CSpider::Check_Collision()
{
    //m_pColliderCom->Intersect(m_pPlayerWeaponCollider);
    //if (m_pColliderCom->IsBoundingCollisionEnter())
    //{
    //    m_pModel->SetUp_Animation(CSniper::SNIPER_ANIMATION::DEATH_1, true);
    //    m_pFsm->Change_State(CSniper::SNIPER_ANIMATION::DEATH_1);
    //}
}

HRESULT CSpider::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;





    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Spider"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    return S_OK;
}

HRESULT CSpider::Ready_Change_Layer()
{
    m_strChangeLayerName = L"Layer_Spider";

    return S_OK;
}

CSpider* CSpider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSpider* pInstance = new CSpider(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSpider"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSpider::Clone(void* pArg)
{
    CSpider* pInstance = new CSpider(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSpider"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSpider::Free()
{
    __super::Free();

    Safe_Release(m_pPlayerWeaponCollider);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);
}
