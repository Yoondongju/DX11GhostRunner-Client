#include "stdafx.h"
#include "Sniper.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "Weapon_Sniper.h"

#include "Sniper_Attack.h"
#include "Sniper_Death.h"
#include "Sniper_Idle.h"

#include "Player.h"
#include "Particle_Blood.h"
#include "Monster_Bullet.h"
#include "Particle_ShockWave.h"

CSniper::CSniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEnemy(pDevice, pContext)
{

}


CSniper::CSniper(const CSniper& Prototype)
    : CEnemy(Prototype)
{

}


HRESULT CSniper::Initialize_Prototype()
{
    return S_OK;
}


HRESULT CSniper::Initialize(void* pArg)
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

    
    m_pTransformCom->Scaling(2.3f, 2.3f, 2.3f);


    m_pModel->SetUp_Animation(CSniper::SNIPER_ANIMATION::IDLE, true);
    m_pFsm->Set_State(CSniper::SNIPER_ANIMATION::IDLE);



    return S_OK;
}

void CSniper::Priority_Update(_float fTimeDelta)
{
    for (auto& pPartObject : m_Parts)
        pPartObject->Priority_Update(fTimeDelta);
}

void CSniper::Update(_float fTimeDelta)
{
    if (true == m_isDead)
    {
        if (m_fDiscard >= 1.f)
            m_pGameInstance->Delete(LEVEL_GAMEPLAY, CRenderer::RG_NONBLEND, this);

        m_fDiscard += fTimeDelta * 0.4f;
    }

    if(true == m_isMindControling)
        m_Parts[PART_SHOCKWAVE]->SetActiveMyParticle(true);


    m_pFsm->Update(fTimeDelta);
    m_pModel->Play_Animation(fTimeDelta);


    for (auto& pPartObject : m_Parts)
        pPartObject->Update(fTimeDelta);


    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CSniper::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif

    for (auto& pPartObject : m_Parts)
        pPartObject->Late_Update(fTimeDelta);
}

HRESULT CSniper::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4((CPipeLine::D3DTS_VIEW)))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
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



    return S_OK;
}

_bool CSniper::Check_Collision()
{
    if (true == m_isDead)
        return false;

    CWeapon_Player* pPlayerWeapon = static_cast<CWeapon_Player*>(static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY))->Get_Part(CPlayer::PART_WEAPON));
    CCollider* pCollider = pPlayerWeapon->Get_Collider();


    if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == pPlayerWeapon->Get_CurType())
    {
        CSubShuriken** ppSubShuriken = pPlayerWeapon->Get_SubShuriken();

        _bool isCollEvenOnce = false;       
        // 한번이라도 충돌했니? 체크하는이유: 어떤표창이 충돌했다고 이야기해도 
        // 다른표창이 충돌안했으면 동일한 프레임에서 Coll체크가 false가 떨어져버림 그래서 기록해야해 

        isCollEvenOnce = m_pColliderCom->Intersect(pCollider);  
        for (_uint i = 0; i < 2; i++)
        {
            if (false == isCollEvenOnce)
            {
                isCollEvenOnce = m_pColliderCom->Intersect(ppSubShuriken[i]->Get_Collider());
            }
        }
           
    }
    else if (CWeapon_Player::WEAPON_TYPE::KATANA == pPlayerWeapon->Get_CurType())
    {
        m_pColliderCom->Intersect(pCollider);
    }
        
    
    if (CSniper::SNIPER_ANIMATION::DEATH_1 != m_pModel->Get_NextAnimationIndex() &&
        m_pColliderCom->IsBoundingCollisionEnter())
    {
        _double& TrackPos = m_pModel->Get_Referene_CurrentTrackPosition();
        TrackPos = 0.0;

        m_pModel->SetUp_Animation(CSniper::SNIPER_ANIMATION::DEATH_1, true);
        m_pFsm->Change_State(CSniper::SNIPER_ANIMATION::DEATH_1);

        static_cast<CParticle_Blood*>(m_Parts[PART_EFFECT])->SetActiveMyParticle(true);

        return true;
    }

    return false;
}

void CSniper::Check_CollByTargetEnemy()
{
    if (true == m_isDead)
        return;

    if (CSniper::SNIPER_ANIMATION::DEATH_1 != m_pModel->Get_CurAnimationIndex())
    {
        _double& TrackPos = m_pModel->Get_Referene_CurrentTrackPosition();
        TrackPos = 0.0;

        m_pModel->SetUp_Animation(CSniper::SNIPER_ANIMATION::DEATH_1, true);
        m_pFsm->Change_State(CSniper::SNIPER_ANIMATION::DEATH_1);

        static_cast<CParticle_Blood*>(m_Parts[PART_EFFECT])->SetActiveMyParticle(true);
    }
}

void CSniper::Check_Collision_Me()
{
    if (true == m_isDead)
        return;

    if (CSniper::SNIPER_ANIMATION::DEATH_1 != m_pModel->Get_CurAnimationIndex())
    {
        _double& TrackPos = m_pModel->Get_Referene_CurrentTrackPosition();
        TrackPos = 0.0;

        m_pModel->SetUp_Animation(CSniper::SNIPER_ANIMATION::DEATH_1, true);
        m_pFsm->Change_State(CSniper::SNIPER_ANIMATION::DEATH_1);

        static_cast<CParticle_Blood*>(m_Parts[PART_EFFECT])->SetActiveMyParticle(true);
    }
}

HRESULT CSniper::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    /* For.Com_Fsm */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Sniper_FSM"),
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
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sniper"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_DeadNoiseTexture"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDeadNoiseTexture), nullptr)))
        return E_FAIL;


    return S_OK;
}

HRESULT CSniper::Ready_Parts()
{
    m_Parts.resize(PART_END);


    CWeapon_Sniper::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WeaponDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("Gun_r");
    WeaponDesc.pOwner = this;
    WeaponDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Weapon_Sniper"), &WeaponDesc)))
        return E_FAIL;

    CParticle_Blood::BLOOD_DESC	BloodDesc{};

    BloodDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BloodDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("spine_01");
    BloodDesc.pOwner = this;
    BloodDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_EFFECT, TEXT("Prototype_GameObject_Particle_Blood"), &BloodDesc)))
        return E_FAIL;


    CMonster_Bullet::BULLET_DESC	BulletDesc{};

    BulletDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BulletDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("Gun_r");
    BulletDesc.pOwner = this;
    BulletDesc.InitWorldMatrix = XMMatrixIdentity();
    BulletDesc.fSpeedPerSec = 20.f;

    if (FAILED(__super::Add_PartObject(PART_BULLET, TEXT("Prototype_GameObject_Monster_Bullet"), &BulletDesc)))
        return E_FAIL;


    CParticle_ShockWave::SHOCKWAVE_DESC	ShockWaveDesc{};

    ShockWaveDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    ShockWaveDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("spine_01");
    ShockWaveDesc.pOwner = this;
    ShockWaveDesc.InitWorldMatrix = XMMatrixIdentity();
    ShockWaveDesc.fSpeedPerSec = 20.f;

    if (FAILED(__super::Add_PartObject(PART_SHOCKWAVE, TEXT("Prototype_GameObject_Particle_ShockWave"), &ShockWaveDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CSniper::Ready_State()
{
    if (FAILED(m_pFsm->Add_State(CSniper_Attack::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CSniper_Death::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CSniper_Idle::Create(this))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSniper::Ready_Change_Layer()
{
    m_strChangeLayerName = L"Layer_Sniper";

    return S_OK;
}

CSniper* CSniper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSniper* pInstance = new CSniper(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSniper"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSniper::Clone(void* pArg)
{
    CSniper* pInstance = new CSniper(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSniper"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSniper::Free()
{
    __super::Free();

    Safe_Release(m_pDeadNoiseTexture);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);


    Safe_Release(m_pColliderCom);
    Safe_Release(m_pFsm);
}
