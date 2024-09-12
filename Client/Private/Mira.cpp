#include "stdafx.h"
#include "Mira.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "Animation.h"

#include "Mira_Attack.h"
#include "Mira_Idle.h"
#include "Mira_Death.h"
#include "Mira_Suiside.h"
#include "Mira_WalkIn.h"

#include "Player.h"

CMira::CMira(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster(pDevice, pContext)
{

}


CMira::CMira(const CMira& Prototype)
    : CMonster(Prototype)
{

}


HRESULT CMira::Initialize_Prototype()
{
    return S_OK;
}


HRESULT CMira::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_State()))
        return E_FAIL;

    if (FAILED(Ready_Change_Layer()))
        return E_FAIL;


    m_pModel->SetUp_Animation(WALKIN_AROUND, true);
    m_pFsm->Set_State(WALKIN_AROUND);

    m_pTransformCom->Scaling(2.5f, 2.4f, 2.5f);


    CCollider* pCollider = static_cast<CWeapon_Player*>(static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY))->Get_Part(CPlayer::PART_WEAPON))->Get_Collider();

    m_pPlayerWeaponCollider = pCollider;
    Safe_AddRef(m_pPlayerWeaponCollider);


    return S_OK;
}

void CMira::Priority_Update(_float fTimeDelta)
{
    

}

void CMira::Update(_float fTimeDelta)
{
    if (true == m_isDead)
    {
        if (m_fDiscard >= 1.f)
            m_pGameInstance->Delete(LEVEL_GAMEPLAY, CRenderer::RG_NONBLEND, this);

        m_fDiscard += fTimeDelta * 0.4f;
        return;
    }
       
        

    m_pFsm->Update(fTimeDelta);
    m_pModel->Play_Animation(fTimeDelta);
    

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CMira::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

}

HRESULT CMira::Render()
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


    _uint iPassNum = 0;
    if (true == m_isDead)
    {
        iPassNum = 1;


        _float dissovleAmount = 0.5f;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveAmount", &m_fDiscard, sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pDeadNoiseTexture->Bind_ShadeResource(m_pShaderCom, "g_NoiseTexture", 0)))
            return E_FAIL;
    }



    _uint iNumMeshes = m_pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModel->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(iPassNum)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }

#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

void CMira::Check_Collision()
{
    m_pColliderCom->Intersect(m_pPlayerWeaponCollider);
    if (m_pColliderCom->IsBoundingCollisionEnter())
    {
        _double& TrackPos = m_pModel->Get_Referene_CurrentTrackPosition();
        TrackPos = 0.0;

        m_pModel->SetUp_Animation(CMira::MIRA_ANIMATION::DEATH, true);
        m_pFsm->Change_State(CMira::MIRA_ANIMATION::DEATH);
    }
}

HRESULT CMira::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    /* For.Com_Fsm */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Mira_FSM"),
        TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsm), nullptr)))
        return E_FAIL;


    /* For.Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(3.f, 10.f, 3.f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
    ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;



    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Mira"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_DeadNoiseTexture"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDeadNoiseTexture), nullptr)))
        return E_FAIL;
    

    return S_OK;
}

HRESULT CMira::Ready_State()
{
    if (FAILED(m_pFsm->Add_State(CMira_Attack::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CMira_Death::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CMira_Idle::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CMira_WalkIn::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CMira_Suiside::Create(this))))
        return E_FAIL;

    
    return S_OK;
}

HRESULT CMira::Ready_Change_Layer()
{
    m_strChangeLayerName = L"Layer_Mira";

    return S_OK;
}

CMira* CMira::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMira* pInstance = new CMira(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMira"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMira::Clone(void* pArg)
{
    CMira* pInstance = new CMira(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMira"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMira::Free()
{
    __super::Free();

    Safe_Release(m_pDeadNoiseTexture);
    Safe_Release(m_pPlayerWeaponCollider);


    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pFsm);
}
