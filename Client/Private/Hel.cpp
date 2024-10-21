#include "stdafx.h"
#include "Hel.h"
#include "GameInstance.h"

#include "FreeCamera.h"
#include "Player.h"

#include "Weapon_Hel.h"
#include "Particle_Blood.h"
#include "Particle_Piece.h"
#include "Particle_Jump.h"
#include "Particle_BigSmoke.h"
#include "Particle_Swirl.h"
#include "Particle_Attack.h"
#include "Particle_Electric.h"
#include "Particle_BigElectric.h"

#include "Animation.h"


#include "Hel_Death.h"
#include "Hel_Idle.h"
#include "Hel_BackBlock.h"
#include "Hel_Jump.h"
#include "Hel_JumpEnd.h"

#include "Hel_Dash.h"
#include "Hel_DashAttack.h"
#include "Hel_Attack.h"
#include "Hel_CutScene.h"
#include "Hel_Stun.h"
#include "Hel_RunJump.h"
#include "Hel_Ready.h"
#include "Hel_Sprint.h"
#include "Hel_Raise.h"
#include "Hel_Sleep.h"

#include "BossHpPanel.h"


#include "HelMotionTrail.h"


CHel::CHel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEnemy(pDevice, pContext)
{

}


CHel::CHel(const CHel& Prototype)
    : CEnemy(Prototype)
{

}


HRESULT CHel::Initialize_Prototype()
{
    return S_OK;
}


HRESULT CHel::Initialize(void* pArg)
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

    if (FAILED(Create_BossHp()))
        return E_FAIL;


    Ready_Modify_Animation();



    m_pModel->SetUp_Animation(CHel::HEL_ANIMATION::SLEEP, true);
    m_pFsm->Set_State(CHel::HEL_ANIMATION::IDLE);
    m_pFsm->Change_State(CHel::HEL_ANIMATION::CUTSCENE);


    m_pTransformCom->Scaling(3.5f, 3.5f, 3.5f);

    m_fEnergy = 100.f;
    m_fHp = 100.f;

    m_fCollisionCoolTime = 1.f;
 

    if (FAILED(Ready_PhysX()))          // 스케일까지 세팅해주고 피직스세팅
        return E_FAIL;


    return S_OK;
}

void CHel::Priority_Update(_float fTimeDelta)
{
    for (auto& pPartObject : m_Parts)
        pPartObject->Priority_Update(fTimeDelta);
}

void CHel::Update(_float fTimeDelta)
{
    if (m_isDead)
    {
        if (m_fDiscard >= 1.f)
            m_pGameInstance->Delete(g_CurLevel, CRenderer::RG_NONBLEND, this);

        m_fDiscard += fTimeDelta * 0.4f;
    }

    if (m_fCollisionCoolTime > 0.f)
        m_fCollisionCoolTime -= fTimeDelta;

    
    
    fAddMotionTrailTime += fTimeDelta;
    

    _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_PxTransform.p = { pWorldMatrix->m[3][0], pWorldMatrix->m[3][1] + 40.f, pWorldMatrix->m[3][2] };
    m_pPxRigidDynamic->setGlobalPose(m_PxTransform);

    PhysXComputeCollision();

    if (m_fEnergy <= 0.f)
    {
        if (CHel::HEL_ANIMATION::STUN_TO_IDLE != m_pFsm->Get_CurStateIndex())
        {
            m_pModel->SetUp_Animation(CHel::HEL_ANIMATION::STUN_TO_IDLE, true);
            m_pFsm->Change_State(CHel::HEL_ANIMATION::STUN_TO_IDLE);
        }  
    }

    m_pModel->Play_Animation(fTimeDelta);
    m_pFsm->Update(fTimeDelta);
    m_pRigidBody->Update(fTimeDelta, m_fLandPosY, m_pColliderCom->Get_CurPhysXCollision());


    for (auto& pPartObject : m_Parts)
        pPartObject->Update(fTimeDelta);



    Check_AttackParticle(fTimeDelta);
    Check_ElectricParticle(fTimeDelta);
    Check_BigElectricParticle(fTimeDelta);
    


    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CHel::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif

    for (auto& pPartObject : m_Parts)
        pPartObject->Late_Update(fTimeDelta);


    _uint iAttackParticleSize = m_Particle_Attack.size();
    for (_uint i = 0; i < iAttackParticleSize; ++i)
    {
        if (m_Particle_Attack[i]->IsActive())
            m_Particle_Attack[i]->Late_Update(fTimeDelta);
    }

    _uint iElectricParticleSize = m_Particle_Attack.size();
    for (_uint i = 0; i < iElectricParticleSize; ++i)
    {
        if (m_Particle_Electric[i]->IsActive())
            m_Particle_Electric[i]->Late_Update(fTimeDelta);
    }

    _uint iBigElectricParticleSize = m_Particle_Attack.size();
    for (_uint i = 0; i < iBigElectricParticleSize; ++i)
    {
        if (m_Particle_BigElectric[i]->IsActive())
            m_Particle_BigElectric[i]->Late_Update(fTimeDelta);
    }

    m_pMotionTrail->Late_Update(fTimeDelta);

}

HRESULT CHel::Render()
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


    deque<CHelMotionTrail::MOTION_TRAIL_INFO>& MotionTrailInfo = m_pMotionTrail->Get_MotionTrailInfo();
    if (MotionTrailInfo.size() > 10)
    {
        MotionTrailInfo.pop_front();
    }

    _uint iNumMeshes = m_pModel->Get_NumMeshes();

    CHelMotionTrail::MOTION_TRAIL_INFO  MotionTrail = {};         // 월행 , 본메트릭스, 라이프타임                        
    XMStoreFloat4x4(&MotionTrail.WorldMatrix, m_pTransformCom->Get_WorldMatrix());
    MotionTrail.fLifeTime = 1.f;

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        // 내 모션트레일의 -> 트레일정보의 -> 이 메시에 영향을 주는 뼈행렬의 첫번째 주소를 준다음
        // 저 안에서 값을 채운다   ( 일단 이거 메쉬 1개라고 가정하고 생각한거임 )
        m_pModel->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i, MotionTrail.BoneMatrices[i]);
   
        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(iPassNum)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }

    if (true == m_pMotionTrail->Is_Active() &&  fAddMotionTrailTime >= 0.3f)
    {
        MotionTrailInfo.emplace_back(MotionTrail);
        fAddMotionTrailTime = 0.f;
    }
       

    return S_OK;
}

_bool CHel::Check_Collision()
{
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));

    CWeapon_Player* pPlayerWeapon = static_cast<CWeapon_Player*>(pPlayer->Get_Part(CPlayer::PART_WEAPON));
    CCollider* pCollider = pPlayerWeapon->Get_Collider();

    if (CWeapon_Player::WEAPON_TYPE::KATANA == pPlayerWeapon->Get_CurType())
    {
        m_pColliderCom->Intersect(pCollider);
    }

    if (m_fCollisionCoolTime <= 0.f && m_pColliderCom->IsBoundingCollisionEnter())
    {
        m_Parts[CHel::PARTID::PART_PARTICLE_ATTACK]->SetActiveMyParticle(true);     // << 이게 조각임
        m_pGameInstance->Play_Sound(TEXT("HelPiece.ogg"), SOUND_HEL_PIECE, 1.f);

        if (false == m_isEnterPage2)
        {
            m_fEnergy -= 2.f;
            m_fCollisionCoolTime = 1.f;
        }

        if (m_fEnergy <= 0.f)
        {
            m_fEnergy = 0.f;
            m_pGameInstance->Set_TimeDelayActive(true, 1.f);
        }
          


        return true;
    }

    return false;
}

_bool CHel::Check_CollisionGroggy()       // 패링 3번 이상햇을때 켜짐
{
    if (true == m_isDead)
        return false;

    CWeapon_Player* pPlayerWeapon = static_cast<CWeapon_Player*>(static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY))->Get_Part(CPlayer::PART_WEAPON));
    CCollider* pCollider = pPlayerWeapon->Get_Collider();


    if (CWeapon_Player::WEAPON_TYPE::KATANA == pPlayerWeapon->Get_CurType())
    {
        m_pColliderCom->Intersect(pCollider);
    }

    if (m_fCollisionCoolTime <= 0.f && m_pColliderCom->IsBoundingCollisionEnter())
    {
        _double& TrackPos = m_pModel->Get_Referene_CurrentTrackPosition();
        TrackPos = 0.0;

        static_cast<CParticle_Blood*>(m_Parts[PART_EFFECT])->SetActiveMyParticle(true);
        m_pGameInstance->Play_Sound(TEXT("HitBlood1.ogg"), SOUND_HELBLOOD, 1.f);

        m_fHp -= 17.f;

        if (m_fHp <= 50.f)
        {
            if (false == m_isEnterPage2)
            {
                m_isEnterPage2 = true;
                m_fEnergy = 100.f;

                m_pFsm->Change_State(CHel::HEL_ANIMATION::CUTSCENE);
            }
        }

        if (m_fHp <= 0.f)
        {
            m_pGameInstance->Set_TimeDelayActive(true, 1.f);
            m_fHp = 0.f;
            m_fEnergy = 0.f;

            m_pModel->SetUp_Animation(HEL_ANIMATION::HEAD_CUT_DEAD, true);
            m_pFsm->Change_State(HEL_ANIMATION::HEAD_CUT_DEAD);
        }

        m_fCollisionCoolTime = 1.f;
        return true;
    }

    return false;
}



void CHel::Check_AttackParticle(_float fTimeDelta)
{
    _uint iAttackParticleSize = m_Particle_Attack.size();
    if (true == m_isActiveParticle_Attack)
    {
        m_fAccNextParticleIndexTime_Attack += fTimeDelta;
        for (_uint i = 0; i < m_iNextUnActiveParticleIndex_Attack + 1; ++i)
        {
            _uint iCurrentIndex = (m_iNextUnActiveParticleIndex_Attack + i) % iAttackParticleSize;
            if (false == m_Particle_Attack[iCurrentIndex]->IsActive())
            {
                m_Particle_Attack[iCurrentIndex]->Set_FirstActive(true);
                m_Particle_Attack[iCurrentIndex]->Set_Active(true);
            }
        }
        if (m_fAccNextParticleIndexTime_Attack >= 0.5f)
        {
            m_iNextUnActiveParticleIndex_Attack = (m_iNextUnActiveParticleIndex_Attack + 1) % iAttackParticleSize;
            m_fAccNextParticleIndexTime_Attack = 0.f;
        }
    }
    else
    {
        m_fAccNextParticleIndexTime_Attack = 0.f;
        m_iNextUnActiveParticleIndex_Attack = 0;
    }

    for (_uint i = 0; i < iAttackParticleSize; ++i)
    {
        if (m_Particle_Attack[i]->IsActive())
            m_Particle_Attack[i]->Update(fTimeDelta);
    }
}

void CHel::Check_ElectricParticle(_float fTimeDelta)
{
    _uint iAttackParticleSize = m_Particle_Electric.size();
    if (true == m_isActiveParticle_Electric)
    {
        m_fAccNextParticleIndexTime_Electric += fTimeDelta;
        for (_uint i = 0; i < m_iNextUnActiveParticleIndex_Electric + 1; ++i)
        {
            _uint iCurrentIndex = (m_iNextUnActiveParticleIndex_Electric + i) % iAttackParticleSize;
            if (false == m_Particle_Electric[iCurrentIndex]->IsActive())
            {
                m_Particle_Electric[iCurrentIndex]->Set_FirstActive(true);
                m_Particle_Electric[iCurrentIndex]->Set_Active(true);
            }
        }
        if (m_fAccNextParticleIndexTime_Electric >= 0.3f)
        {
            m_iNextUnActiveParticleIndex_Electric = (m_iNextUnActiveParticleIndex_Electric + 1) % iAttackParticleSize;
            m_fAccNextParticleIndexTime_Electric = 0.f;
        }
    }
    else
    {
        m_fAccNextParticleIndexTime_Electric = 0.f;
        m_iNextUnActiveParticleIndex_Electric = 0;
    }

    for (_uint i = 0; i < iAttackParticleSize; ++i)
    {
        if (m_Particle_Electric[i]->IsActive())
            m_Particle_Electric[i]->Update(fTimeDelta);
    }
}

void CHel::Check_BigElectricParticle(_float fTimeDelta)
{
    _uint iAttackParticleSize = m_Particle_BigElectric.size();
    if (true == m_isActiveParticle_BigElectric)
    {
        m_fAccNextParticleIndexTime_BigElectric += fTimeDelta;
        for (_uint i = 0; i < m_iNextUnActiveParticleIndex_BigElectric + 1; ++i)
        {
            _uint iCurrentIndex = (m_iNextUnActiveParticleIndex_BigElectric + i) % iAttackParticleSize;
            if (false == m_Particle_BigElectric[iCurrentIndex]->IsActive())
            {
                m_Particle_BigElectric[iCurrentIndex]->Set_FirstActive(true);
                m_Particle_BigElectric[iCurrentIndex]->Set_Active(true);
            }
        }
        if (m_fAccNextParticleIndexTime_BigElectric >= 1.f)
        {
            m_iNextUnActiveParticleIndex_BigElectric = (m_iNextUnActiveParticleIndex_BigElectric + 1) % iAttackParticleSize;
            m_fAccNextParticleIndexTime_BigElectric = 0.f;
        }
    }
    else
    {
        m_fAccNextParticleIndexTime_BigElectric = 0.f;
        m_iNextUnActiveParticleIndex_BigElectric = 0;
    }

    for (_uint i = 0; i < iAttackParticleSize; ++i)
    {
        if (m_Particle_BigElectric[i]->IsActive())
            m_Particle_BigElectric[i]->Update(fTimeDelta);
    }
}





void CHel::PhysXComputeCollision()
{
    m_pColliderCom->Set_PrePhysXCollision(m_pColliderCom->Get_CurPhysXCollision());	// 충돌 들어가기전에 항상 이전 충돌정보를 세팅!

    if (true == m_pGameInstance->CollisionUpdate_PlayerToTriangleMeshGeometry(&m_vDir, &m_vDepth, m_pShape, &m_PxTransform, &m_pCollisionDestObject))
    {
        m_pColliderCom->Set_CurPhysXCollision(true);

        _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

        pWorldMatrix->m[3][0] += m_vDir.x * (m_vDepth + 0.1f);
        pWorldMatrix->m[3][1] += m_vDir.y * (m_vDepth + 0.1f);
        pWorldMatrix->m[3][2] += m_vDir.z * (m_vDepth + 0.1f);            // Y축 충돌이 섞여서 문제가 생겨 

        // 충돌한 객체가 벽을 탈수있는애인걸 확인해야함

        if (m_pColliderCom->IsPhysXCollisionEnter())
        {
            
        }
    }


    PxVec3 rayOrigin = m_PxTransform.p;

    PxVec3 vEndPos = { 0.0f, -1.0f, 0.0f };

    PxVec3 rayDirection = vEndPos;		// 내 업벡터의 반대방향

    // 레이 길이는 충분히 크게 설정
    _float rayLength = 500.0f; // 필요에 따라 조정 가능


    PxRaycastBuffer hitInfo;
    PxQueryFilterData data;
    data.flags = PxQueryFlag::eSTATIC;


    _bool status = m_pGameInstance->Get_Scene()->raycast(rayOrigin, rayDirection, rayLength, hitInfo, PxHitFlag::eDEFAULT, data);

    if (status && hitInfo.hasBlock)
    {
        _float fMeshY = hitInfo.block.position.y;
        _float fPlayerY = m_PxTransform.p.y;

        m_fLandPosY = fMeshY;
    }
    else
        m_fLandPosY = -9999.f;	// 지형이 없을때 맥시멈으로 떨어지는위치  
}

HRESULT CHel::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    /* For.Com_Fsm */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Hel_FSM"),
        TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsm), nullptr)))
        return E_FAIL;


    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_RigidBody"),
        TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), nullptr)))
        return E_FAIL;
    m_pRigidBody->Set_OwnerTransform(m_pTransformCom);


    /* For.Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(3.f, 10.f, 3.f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
    ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;



    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Model_Hel"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_DeadNoiseTexture"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDeadNoiseTexture), nullptr)))
        return E_FAIL;


    return S_OK;
}

HRESULT CHel::Ready_Parts()
{
    m_Parts.resize(PART_END);

    CWeapon_Hel::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WeaponDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("weapon_bone");
    WeaponDesc.pOwner = this;
    WeaponDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Weapon_Hel"), &WeaponDesc)))
        return E_FAIL;


    CParticle_Blood::BLOOD_DESC	    BloodDesc{};
    BloodDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BloodDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("spine_01");
    BloodDesc.pOwner = this;
    BloodDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_EFFECT, TEXT("Prototype_GameObject_Particle_Blood"), &BloodDesc)))
        return E_FAIL;

    
   


    _float3				KatanaStartLocal = _float3(-0.145653754, -0.386195779, 0.683916509);
    _float3				KatanaEndLocal = _float3(0.112252407, 0.402730137, 8.51980686);
    _float3             KatanaMiddleLocal;

    KatanaMiddleLocal.x = (KatanaStartLocal.x + KatanaEndLocal.x) * 0.5f;
    KatanaMiddleLocal.y = (KatanaStartLocal.y + KatanaEndLocal.y) * 0.5f;
    KatanaMiddleLocal.z = (KatanaStartLocal.z + KatanaEndLocal.z) * 0.5f;



    CParticle_Piece::PARTICLE_DESC	AttackDesc{};

    AttackDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    AttackDesc.pOwner = this;
    AttackDesc.SpwanPositionLocal = KatanaMiddleLocal;
    if (FAILED(__super::Add_PartObject(PART_PARTICLE_ATTACK, TEXT("Prototype_GameObject_Particle_Piece"), &AttackDesc)))
        return E_FAIL;


    CParticle_Jump::PARTICLE_JUMPEFFECT	    JumpDesc{};
    JumpDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    JumpDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("foot_l");
    JumpDesc.pOwner = this;
    JumpDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_PARTICLE_JUMPSTART, TEXT("Prototype_GameObject_Particle_HelJumpEffect"), &JumpDesc)))
        return E_FAIL;
    
    if (FAILED(__super::Add_PartObject(PART_PARTICLE_JUMPEND, TEXT("Prototype_GameObject_Particle_HelJumpEffect"), &JumpDesc)))
        return E_FAIL;


    CParticle_BigSmoke::PARTICLE_EFFECT	    SmokeDesc{};
    SmokeDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    SmokeDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("spine_01");
    SmokeDesc.pOwner = this;
    SmokeDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_PARTICLE_BIGSMOKE, TEXT("Prototype_GameObject_Particle_BigSmoke"), &SmokeDesc)))
        return E_FAIL;



    CParticle_Swirl::PARTICLE_EFFECT	    SwirlDesc{};
    SwirlDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    SwirlDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("foot_l");
    SwirlDesc.pOwner = this;
    SwirlDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_PARTICLE_SWIRL, TEXT("Prototype_GameObject_Particle_Swirl"), &SwirlDesc)))
        return E_FAIL;


    m_Particle_Attack.reserve(50);
    for (_uint i = 0; i < m_Particle_Attack.capacity(); i++)
    {
        CParticle_Attack::PARTICLE_EFFECT	    AttackDesc{};
        AttackDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
        AttackDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("weapon_bone");
        AttackDesc.pOwner = this;
        AttackDesc.InitWorldMatrix = XMMatrixIdentity();

        CParticle_Attack* pAttackParticle = static_cast<CParticle_Attack*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Attack"), &AttackDesc));
        m_Particle_Attack.emplace_back(pAttackParticle);
    }


    m_Particle_Attack.reserve(10);
    for (_uint i = 0; i < m_Particle_Attack.capacity(); i++)
    {
        CParticle_Electric::PARTICLE_DESC	ElectricDesc{};
        ElectricDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
        ElectricDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("weapon_bone");
        ElectricDesc.pOwner = this;
        ElectricDesc.InitWorldMatrix = XMMatrixIdentity();

        CParticle_Electric* pAttackParticle = static_cast<CParticle_Electric*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Electric"), &ElectricDesc));
        m_Particle_Electric.emplace_back(pAttackParticle);
    }


    m_Particle_Attack.reserve(10);
    for (_uint i = 0; i < m_Particle_Attack.capacity(); i++)
    {
        CParticle_BigElectric::PARTICLE_DESC	BigElectricDesc{};
        BigElectricDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
        BigElectricDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("spine_01");
        BigElectricDesc.pOwner = this;
        BigElectricDesc.InitWorldMatrix = XMMatrixIdentity();

        CParticle_BigElectric* pAttackParticle = static_cast<CParticle_BigElectric*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_BigElectric"), &BigElectricDesc));
        m_Particle_BigElectric.emplace_back(pAttackParticle);
    }



    m_pMotionTrail = static_cast<CHelMotionTrail*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_HelMotionTrail"));
  

    return S_OK;
}

HRESULT CHel::Ready_State()
{
    if (FAILED(m_pFsm->Add_State(CHel_Death::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHel_Idle::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHel_BackBlock::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHel_Jump::Create(this))))
        return E_FAIL;
    if (FAILED(m_pFsm->Add_State(CHel_JumpEnd::Create(this))))
        return E_FAIL;


    if (FAILED(m_pFsm->Add_State(CHel_Dash::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHel_DashAttack::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHel_Attack::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHel_CutScene::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHel_Stun::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHel_RunJump::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHel_Ready::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHel_Sprint::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHel_Raise::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHel_Sleep::Create(this))))
        return E_FAIL;

    return S_OK;
}

HRESULT CHel::Ready_PhysX()
{
    _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_PxTransform = PxTransform(PxMat44(pWorldMatrix->_11));

    PxVec3 vStartPos = { pWorldMatrix->m[1][0] , pWorldMatrix->m[1][1] , pWorldMatrix->m[1][2] };
    PxVec3 vEndPos = { -pWorldMatrix->m[1][0], -pWorldMatrix->m[1][1] , -pWorldMatrix->m[1][2] };
    m_PxTransform = PxTransformFromSegment(vStartPos, vEndPos);


    m_pPxRigidDynamic = m_pGameInstance->Get_Physics()->createRigidDynamic(m_PxTransform);

    PxMaterial* Material = m_pGameInstance->Get_Material();


    vector<CMesh*>& Meshs = m_pModel->Get_Meshes();


    // 4. Triangle Mesh 기반 Shape 생성
    m_pShape = m_pGameInstance->Get_Physics()->createShape(PxCapsuleGeometry(20.f, 1.f), *Material);
    if (!m_pShape) {
        MSG_BOX(TEXT("createShape failed!"));
        return E_FAIL;
    }


    //PxTransform localPose(PxVec3(0.f, 10.f, 0.f));  // (5.f, 0.f, 0.f) 위치로 이동
    //m_pShape->setLocalPose(localPose);
    m_pPxRigidDynamic->attachShape(*m_pShape);
    m_pGameInstance->Get_Scene()->addActor(*m_pPxRigidDynamic);



    return S_OK;
}

HRESULT CHel::Ready_Change_Layer()
{
    m_strChangeLayerName = L"Layer_Hel";

    return S_OK;
}

void CHel::Ready_Modify_Animation()
{
    vector<CAnimation*>& Animations = m_pModel->Get_Animations();

    Animations[HEL_ANIMATION::ATTACK3]->Set_SpeedPerSec(45.f);
    Animations[HEL_ANIMATION::ATTACK4]->Set_SpeedPerSec(45.f);
    
    Animations[HEL_ANIMATION::SPRINT]->Set_SpeedPerSec(18.f);
}

HRESULT CHel::Create_BossHp()
{
    CBossHpPanel::BOSSHP_DESC PanelDesc = {};
    PanelDesc.eBossType = CBossHpPanel::BOSSTYPE::HEL;
    PanelDesc.pOwner = this;
    PanelDesc.fX = g_iWinSizeX * 0.5f;
    PanelDesc.fY = 100.f;
    PanelDesc.fSizeX = 1360;
    PanelDesc.fSizeY = 30;
    PanelDesc.fSpeedPerSec = 10.f;
    PanelDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE1_BOSS, L"Layer_BossHpUI", L"Prototype_GameObject_BossHpPanel", L"No Model", &PanelDesc)))
        return E_FAIL;






    return S_OK;
}

CHel* CHel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHel* pInstance = new CHel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CHel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHel::Clone(void* pArg)
{
    CHel* pInstance = new CHel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CHel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHel::Free()
{
    __super::Free();

    for (_uint i = 0; i < m_Particle_Attack.size(); i++)
    {
        Safe_Release(m_Particle_Attack[i]);
    }
    m_Particle_Attack.clear();

    for (_uint i = 0; i < m_Particle_Electric.size(); i++)
    {
        Safe_Release(m_Particle_Electric[i]);
    }
    m_Particle_Electric.clear();

    for (_uint i = 0; i < m_Particle_BigElectric.size(); i++)
    {
        Safe_Release(m_Particle_BigElectric[i]);
    }
    m_Particle_BigElectric.clear();

   

    Safe_Release(m_pMotionTrail);

    Safe_Release(m_pDeadNoiseTexture);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pFsm);
    Safe_Release(m_pRigidBody);

    Safe_Release(m_pCollisionDestObject);
}
