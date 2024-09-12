#include "stdafx.h"
#include "Jetpack.h"
#include "GameInstance.h"

#include "FreeCamera.h"


#include "Weapon_Jetpack.h"
#include "BackPack_Jetpack.h"

#include "Jetpack_Attack.h"
#include "Jetpack_Death.h"
#include "Jetpack_Fly.h"
#include "Jetpack_Idle.h"
#include "Jetpack_Scared.h"

#include "Animation.h"

#include "Player.h"

CJetpack::CJetpack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject(pDevice, pContext)
{

}


CJetpack::CJetpack(const CJetpack& Prototype)
    : CContainerObject(Prototype)
{

}


HRESULT CJetpack::Initialize_Prototype()
{
    return S_OK;
}


HRESULT CJetpack::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_Parts()))
        return E_FAIL;

    if (FAILED(Ready_State()))
        return E_FAIL;

    if (FAILED(Ready_Change_Layer()))
        return E_FAIL;

    m_pModel->SetUp_Animation(IDLE, true);
    m_pFsm->Set_State(IDLE);

    m_pTransformCom->Scaling(2.4f, 2.4f, 2.4f);


    CCollider* pCollider =  static_cast<CWeapon_Player*>(static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY))->Get_Part(CPlayer::PART_WEAPON))->Get_Collider();

    m_pPlayerWeaponCollider = pCollider;
    Safe_AddRef(m_pPlayerWeaponCollider);

    return S_OK;
}

void CJetpack::Priority_Update(_float fTimeDelta)
{ 
    for (auto& pPartObject : m_Parts)
        pPartObject->Priority_Update(fTimeDelta);

}


void CJetpack::Update(_float fTimeDelta)
{
    if (true == m_isDead)
    {
        if (m_fDiscard >= 1.f)
            m_pGameInstance->Delete(LEVEL_GAMEPLAY, CRenderer::RG_NONBLEND, this);

        m_fDiscard += fTimeDelta * 0.4f;
        return;
    }

    m_pFsm->Update(fTimeDelta);
    m_pRigidBody->Update(fTimeDelta, -1.f, false);  // 중력 안쓸게


    
    m_pModel->Play_Animation(fTimeDelta);

    for (auto& pPartObject : m_Parts)
        pPartObject->Update(fTimeDelta);

    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CJetpack::Late_Update(_float fTimeDelta)
{

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

    for (auto& pPartObject : m_Parts)
        pPartObject->Late_Update(fTimeDelta);
}

HRESULT CJetpack::Render()
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

void CJetpack::Check_Collision()
{
    m_pColliderCom->Intersect(m_pPlayerWeaponCollider);
    if (m_pColliderCom->IsBoundingCollisionEnter())
    {
        _double& TrackPos = m_pModel->Get_Referene_CurrentTrackPosition();
        TrackPos = 0.0;

        m_pModel->SetUp_Animation(CJetpack::JETPACK_ANIMATION::FLY_FALL, true);
        m_pFsm->Change_State(CJetpack::JETPACK_ANIMATION::FLY_FALL);
    }
}

HRESULT CJetpack::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    /* For.Com_RigidBody */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_RigidBody"),
        TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), nullptr)))
        return E_FAIL;
    m_pRigidBody->Set_OwnerTransform(m_pTransformCom);
    m_pRigidBody->Set_IsGravity(false);


    /* For.Com_Fsm */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Jetpack_FSM"),
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
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Jetpack"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_DeadNoiseTexture"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDeadNoiseTexture), nullptr)))
        return E_FAIL;


    return S_OK;
}

HRESULT CJetpack::Ready_Parts()
{
    m_Parts.resize(PARTID::PART_END);

   
    CWeapon_Jetpack::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WeaponDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("Gun_r");
    WeaponDesc.pOwner = this;
    WeaponDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Weapon_Jetpack"), &WeaponDesc)))
        return E_FAIL;

    CBackPack_Jetpack::BACKPACK_DESC	BackPackDesc{};
   BackPackDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
   BackPackDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("spine_01");
   BackPackDesc.pOwner = this;
   BackPackDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_BACKPACK, TEXT("Prototype_GameObject_BackPack_Jetpack"), &BackPackDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CJetpack::Ready_State()
{
    if (FAILED(m_pFsm->Add_State(CJetpack_Attack::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CJetpack_Idle::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CJetpack_Scared::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CJetpack_Fly::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CJetpack_Death::Create(this))))
        return E_FAIL;


    return S_OK;
}

void CJetpack::Ready_Modify_Animation()
{
    vector<CAnimation*>&  Animations = m_pModel->Get_Animations();


    Animations[CJetpack::JETPACK_ANIMATION::FLY_L]->Set_SpeedPerSec(40.f);
    Animations[CJetpack::JETPACK_ANIMATION::FLY_R]->Set_SpeedPerSec(40.f);
}

HRESULT CJetpack::Ready_Change_Layer()
{
    m_strChangeLayerName = L"Layer_Jetpack";

    return S_OK;
}

CJetpack* CJetpack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CJetpack* pInstance = new CJetpack(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CJetpack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CJetpack::Clone(void* pArg)
{
    CJetpack* pInstance = new CJetpack(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CJetpack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CJetpack::Free()
{
    __super::Free();

    Safe_Release(m_pDeadNoiseTexture);
    Safe_Release(m_pPlayerWeaponCollider);


    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pFsm);
    Safe_Release(m_pRigidBody);
}
