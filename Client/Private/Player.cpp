#include "stdafx.h"
#include "Player.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "Body_Player.h"
#include "Weapon_Player.h"
#include "SwordTrail.h"
#include "ShurikenTrail.h"

#include "SubShuriken.h"
#include "Wire_Player.h"

#include "Player_Idle.h"

#include "Player_Dash.h"
#include "Player_Sh_Dash.h"

#include "Player_Block1.h"
#include "Player_Block2.h"
#include "Player_Block3.h"

#include "Player_Attack1.h"
#include "Player_Attack2.h"
#include "Player_Attack3.h"

#include "Player_CutAll.h"

#include "HomingShuriken.h"

#include "Player_Nami.h"
#include "Player_MindControl.h"
#include "Player_TimeStop.h"

#include "Player_Sh_Attack1.h"

#include "Player_Hook.h"
#include "Player_Climb.h"

#include "Player_Walk.h"
#include "Player_Run.h"
#include "Player_SH_Walk.h"
#include "Player_SH_Run.h"

#include "Player_Jump.h"
#include "Player_JumpFall.h"

#include "GrapplingPointUI.h"

#include "Particle_Block.h"
#include "Particle_CutAll.h"
#include "Particle_Nami.h"


CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject(pDevice, pContext)
{

}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainerObject(Prototype)
{

}



HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_State()))
        return E_FAIL;


    XMStoreFloat4x4(&m_RotationMatrix, XMMatrixIdentity());

    m_eCurAnimationID = PLAYER_ANIMATIONID::IDLE;
    m_pFsm->Set_State(PLAYER_ANIMATIONID::IDLE);


    _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector vNewPos = { XMVectorGetX(vPos) + 9 , m_fOffsetY, XMVectorGetZ(vPos) + 15 };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);



    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_PlayerUI()))
        return E_FAIL;

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    if (true == m_pGameInstance->IsTimeDelayActive())
        fTimeDelta *= m_fTimeDelayLerpRatio;

    if (m_pGameInstance->Get_KeyState(KEY::P) == KEY_STATE::TAP)
    {
        if (false == m_bFreeWalk)
            m_bFreeWalk = true;
        else
            m_bFreeWalk = false;
    }

    static _bool asd = false; 
    if (m_pGameInstance->Get_KeyState(KEY::O) == KEY_STATE::TAP)
    {
        asd = true;
    }


    POINT ptMousePos = {};
    GetCursorPos(&ptMousePos);


    if (true == asd)
    {
        CTransform* pTrans = m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY)->Get_Transform();
    
        if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
            pTrans->Go_Straight_FreeWalk(fTimeDelta);
        if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
            pTrans->Go_Backward_FreeWalk(fTimeDelta);
    
        if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
            pTrans->Go_Left_FreeWalk(fTimeDelta);
        if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
            pTrans->Go_Right_FreeWalk(fTimeDelta);
    
        if (g_hWnd == GetFocus() && m_pGameInstance->Get_KeyState(KEY::RBUTTON) == KEY_STATE::HOLD)
        {
            _long	MouseMove = {};
    
    
            if (MouseMove = ptMousePos.x - m_ptOldMousePos.x)
            {
                pTrans->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.07f, &m_RotationMatrix);
            }
    
            if (MouseMove = ptMousePos.y - m_ptOldMousePos.y)
            {
                pTrans->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.07f, &m_RotationMatrix);
            }
        }
        m_ptOldMousePos = ptMousePos;
    
        return;
    }
      

    CBody_Player* pBodyPlayer = static_cast<CBody_Player*>(m_Parts[PARTID::PART_BODY]);

    if (false == m_bFreeWalk)
    {
        _uint iCurStateIndex = m_pFsm->Get_CurStateIndex();
        _float fAddSpeed = 1.f;

        if (CPlayer::PLAYER_ANIMATIONID::JUMP_START == iCurStateIndex ||
            CPlayer::PLAYER_ANIMATIONID::JUMP_LOOP == iCurStateIndex)
        {
            fAddSpeed = 2.f;
        }

        if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
            m_pTransformCom->Go_Straight(fTimeDelta * fAddSpeed);
        if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
            m_pTransformCom->Go_Backward(fTimeDelta * fAddSpeed);

        if (false == static_cast<CBody_Player*>(m_Parts[PARTID::PART_BODY])->IsLandingWall())
        {
            if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
                m_pTransformCom->Go_Left(fTimeDelta * fAddSpeed);
            if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
                m_pTransformCom->Go_Right(fTimeDelta * fAddSpeed);
        }
    }
    else
    {
        m_pRigidBody->Set_IsGravity(false);

        if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
            m_pTransformCom->Go_Straight_FreeWalk(fTimeDelta * 4);
        if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
            m_pTransformCom->Go_Backward_FreeWalk(fTimeDelta * 4);

        if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
            m_pTransformCom->Go_Left_FreeWalk(fTimeDelta * 4);
        if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
            m_pTransformCom->Go_Right_FreeWalk(fTimeDelta * 4);
    }
    
   

 
    if (false == pBodyPlayer->IsLandingWall())
    {
        if (g_hWnd == GetFocus() && m_pGameInstance->Get_KeyState(KEY::RBUTTON) == KEY_STATE::HOLD)
        {
            _long	MouseMove = {};
           
        
            if (MouseMove = ptMousePos.x - m_ptOldMousePos.x)
            {
                m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.07f, &m_RotationMatrix);
            }
        
            if (MouseMove = ptMousePos.y - m_ptOldMousePos.y)
            {
                m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.07f, &m_RotationMatrix);
            }
        }

        //_long	MouseMove = {};
        //
        //if (MouseMove = ptMousePos.x - m_ptOldMousePos.x)
        //{
        //    m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.2f, &m_RotationMatrix);
        //}
        //
        //if (MouseMove = ptMousePos.y - m_ptOldMousePos.y)
        //{
        //    m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.2f, &m_RotationMatrix);
        //}
    }
   
    m_ptOldMousePos = ptMousePos;



    if (m_bShaking)
    {
        if (m_iShakingCount >= 0)
        {                      
            // ¦���� �� ���������� ȸ��
     
            // ¦���϶� ���������Ӷ� �Z���� + �̹��������� ���� ����
            // �ٽ� Ȧ���϶� �̹��������� ���� ������ 2��  
            // �̹��������� ���ǰ����� ?>

            _float fShakeForce = { 0.f };

            if (m_iShakingCount % 2 == 0)
            {    
                fShakeForce = m_fForce * ((_float)m_iShakingCount / m_iOriginShakingCount);
                         
                m_fCurRotation = fShakeForce;
            }
            else
            {                          
                m_fCurRotation = -m_fPreRotation * 2;
            }
          
            Active_Shake(0.2f * (m_fCurRotation - m_fPreRotation) , XMLoadFloat3(&m_ShakeAsix));
            m_fPreRotation = m_fCurRotation;

        }       
        else
        {
            m_bShaking = false;
    
            m_iShakingCount = 0;
            m_iShakingCount = 0;
    
            m_fForce = { 0.f };
            m_ShakeAsix = {};

            m_fCurRotation = 0.f;
            m_fPreRotation = 0.f; 
        }
    }


    
    for (auto& pPartObject : m_Parts)
        pPartObject->Priority_Update(fTimeDelta);

}

void CPlayer::Update(_float fTimeDelta)
{
    if (true == m_pGameInstance->IsTimeDelayActive())
        fTimeDelta *= m_fTimeDelayLerpRatio;


    CBody_Player* pBodyPart = static_cast<CBody_Player*>(m_Parts[PARTID::PART_BODY]);
    

    Compute_DashCoolTime(fTimeDelta);
    Compute_BlockCoolTime(fTimeDelta);
    Compute_CutAllCoolTime(fTimeDelta);
    Compute_TimeStopCoolTime(fTimeDelta);
    Compute_NamiCoolTime(fTimeDelta);
    Compute_MindControlCoolTime(fTimeDelta);
    Compute_HomingShCoolTime(fTimeDelta);


    m_pFsm->Update(fTimeDelta);
    m_pRigidBody->Update(fTimeDelta, m_fLandPosY + m_fOffsetY, pBodyPart->Get_Collider()->Get_CurPhysXCollision());

    // < PVD�󿡼� ��Ȯ�ϰ� �浹�ϴ� ���� Ȯ���ϰ������ m_fOffsetY �ּ�ġ�� Ȯ���غ�
    // ���� �����ϱ� �򰥸�
    // �������� �����ؾ��ϰ� , ���콺 ȸ���� �� up���Ϳ� ���� �������վ�  ���콺�� ȸ���ص� �� ĸ���� ȸ���Ǹ� �ȴ�


    for (auto& pPartObject : m_Parts)
        pPartObject->Update(fTimeDelta);
    
    
    

    if (pBodyPart->Get_Collider()->Get_CurPhysXCollision() || PLAYER_ANIMATIONID::HOOK_UP == m_pFsm->Get_CurStateIndex())
        m_pRigidBody->Set_IsGravity(false);
    else
        m_pRigidBody->Set_IsGravity(true);

}

void CPlayer::Late_Update(_float fTimeDelta)
{
    if (true == m_pGameInstance->IsTimeDelayActive())
        fTimeDelta *= m_fTimeDelayLerpRatio;

    // ���߿� ���ִ�? 
    // �� ��ġ�� -9999�ų� 
    _float fPlayerY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    _float fDistance = fabs(fPlayerY - m_fLandPosY);


    if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::NONE && fDistance  > 200.f) 
    {
        if (false == static_cast<CBody_Player*>(m_Parts[PARTID::PART_BODY])->IsLandingWall())
        {
            _uint iCurStateIndex = m_pFsm->Get_CurStateIndex();
            CBody_Player* pBodyPlayer = static_cast<CBody_Player*>(m_Parts[PARTID::PART_BODY]);

            if (
                PLAYER_ANIMATIONID::JUMP_START != iCurStateIndex &&
                PLAYER_ANIMATIONID::JUMP_LOOP != iCurStateIndex &&
                PLAYER_ANIMATIONID::JUMP_END != iCurStateIndex &&
                PLAYER_ANIMATIONID::HOOK_UP != iCurStateIndex &&   
                PLAYER_ANIMATIONID::SH_ATTACK != iCurStateIndex &&
                PLAYER_ANIMATIONID::CLIMB != iCurStateIndex && 
                PLAYER_ANIMATIONID::NAMI_AIM_ATTACK_TO_IDLE != iCurStateIndex &&
                PLAYER_ANIMATIONID::NAMI_IDLE_TO_AIM != iCurStateIndex &&
                PLAYER_ANIMATIONID::FURR_AIM_LOOP != iCurStateIndex )
            {
                // �����Ҷ� �������������̿��� ���� �����ؾ��ϰ� 
                // ������ ���� ���̸� ���� ���·� ���� -> ���������϶� �̵��� �ȵǾ��ϳ�?
                // �����Ҷ� �̵� �� �ȵ� ���� �� �����Ҷ� �ִϸ��̼� ����
    
                pBodyPlayer->Get_Model()->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::JUMP_START, false);
                m_pFsm->Change_State(CPlayer::PLAYER_ANIMATIONID::JUMP_LOOP);
            }
        }
    
    }


    for (auto& pPartObject : m_Parts)
        pPartObject->Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{


    return S_OK;
}


void CPlayer::Active_Shake(_float fForce, _fvector vAsix)
{
    m_pTransformCom->Turn(vAsix, fForce, &m_RotationMatrix);
    --m_iShakingCount;
}


HRESULT CPlayer::Ready_Component()
{
    /* For.Com_Fsm */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Player_FSM"),
        TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsm), nullptr)))
        return E_FAIL;



    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TimeStopRefraction"),
        TEXT("Com_Tex1"), reinterpret_cast<CComponent**>(&m_pTimeStopRefractionTex), nullptr)))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MindControlRefraction"),
        TEXT("Com_Tex2"), reinterpret_cast<CComponent**>(&m_pMindControlRefractionTex), nullptr)))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BlurMask"),
        TEXT("Com_Tex3"), reinterpret_cast<CComponent**>(&m_pBlurMaskTex), nullptr)))
        return E_FAIL;


    

    /* For.Com_RigidBody*/
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_RigidBody"),
        TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), nullptr)))
        return E_FAIL;

    m_pRigidBody->Set_OwnerTransform(m_pTransformCom);



    return S_OK;
}

HRESULT	CPlayer::Ready_State()
{
    if (FAILED(m_pFsm->Add_State(CPlayer_Idle::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Dash::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Sh_Dash::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Block1::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Block2::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Block3::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Attack1::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Attack2::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Attack3::Create(this))))
        return E_FAIL;


    if (FAILED(m_pFsm->Add_State(CPlayer_CutAll::Create(this))))
        return E_FAIL;


    if (FAILED(m_pFsm->Add_State(CPlayer_Nami::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_MindControl::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_TimeStop::Create(this))))
        return E_FAIL;


    if (FAILED(m_pFsm->Add_State(CPlayer_Sh_Attack1::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CHomingShuriken::Create(this))))       // �̸� ���ߤ� �Ĵٽ� ���󺹱���
        return E_FAIL;
    

    if (FAILED(m_pFsm->Add_State(CPlayer_Hook::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Climb::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Walk::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Run::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Sh_Walk::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Sh_Run::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Jump::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_JumpFall::Create(this))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_PlayerUI()
{
    return S_OK;
}

void CPlayer::Compute_DashCoolTime(_float fTimeDelta)
{
    if (true == m_bStartCountDashTime)
        m_fDashRemainingTime += fTimeDelta;


    if (m_fDashCoolTime <= m_fDashRemainingTime)
        m_bDashActive = true;
    else if (m_fDashCoolTime > m_fDashRemainingTime)
        m_bDashActive = false;
}

void CPlayer::Compute_BlockCoolTime(_float fTimeDelta)
{
    if (true == m_bStartCountBlockTime)
        m_fBlockRemainingTime += fTimeDelta;


    if (m_fBlockCoolTime <= m_fBlockRemainingTime)
        m_bBlockActive = true;
    else if (m_fBlockCoolTime > m_fBlockRemainingTime)
        m_bBlockActive = false;
}

void CPlayer::Compute_CutAllCoolTime(_float fTimeDelta)
{
    if (true == m_bStartCountCutAllTime)
        m_fCutAllRemainingTime += fTimeDelta;


    if (m_fCutAllCoolTime <= m_fCutAllRemainingTime)
        m_bCutAllActive = true;
    else if (m_fCutAllCoolTime > m_fCutAllRemainingTime)
        m_bCutAllActive = false;
}

void CPlayer::Compute_TimeStopCoolTime(_float fTimeDelta)
{
    if (true == m_bStartCountTimeStopTime)
        m_fTimeStopRemainingTime += fTimeDelta;


    if (m_fTimeStopCoolTime <= m_fTimeStopRemainingTime)
        m_bTimeStopActive = true;
    else if (m_fTimeStopCoolTime > m_fTimeStopRemainingTime)
        m_bTimeStopActive = false;
}

void CPlayer::Compute_NamiCoolTime(_float fTimeDelta)
{
    if (true == m_bStartCountNamiTime)
        m_fNamiRemainingTime += fTimeDelta;


    if (m_fNamiCoolTime <= m_fNamiRemainingTime)
        m_bNamiActive = true;
    else if (m_fNamiCoolTime > m_fNamiRemainingTime)
        m_bNamiActive = false;
}

void CPlayer::Compute_MindControlCoolTime(_float fTimeDelta)
{
    if (true == m_bStartCountMindControlTime)
        m_fMindControlRemainingTime += fTimeDelta;


    if (m_fMindControlCoolTime <= m_fMindControlRemainingTime)
        m_bMindControlActive = true;
    else if (m_fMindControlCoolTime > m_fMindControlRemainingTime)
        m_bMindControlActive = false;
}

void CPlayer::Compute_HomingShCoolTime(_float fTimeDelta)
{
    if (true == m_bStartCountHomingShTime)
        m_fHomingShRemainingTime += fTimeDelta;


    if (m_fHomingShCoolTime <= m_fHomingShRemainingTime)
        m_bHomingShActive = true;
    else if (m_fHomingShCoolTime > m_fHomingShRemainingTime)
        m_bHomingShActive = false;
}




HRESULT CPlayer::Ready_PartObjects()
{
    /* ���� �߰��ϰ� ���� ��Ʈ������Ʈ�� ������ŭ ����� �����س���. */
    m_Parts.resize(PARTID::PART_END);

    CBody_Player::BODY_DESC		BodyDesc{};
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.pOwner = this;
    BodyDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
        return E_FAIL;


    CWeapon_Player::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WeaponDesc.pSocketBoneMatrix = static_cast<CBody_Player*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Weapon_r");
    WeaponDesc.pOwner = this;
    WeaponDesc.InitWorldMatrix = XMMatrixIdentity();
    WeaponDesc.fRotationPerSec = 30.f;
    WeaponDesc.fSpeedPerSec = 30.f;



    CSubShuriken::SUBSHURIKEN_DESC  SubShurikenDesc = {};
    SubShurikenDesc.fRotationPerSec = 30.f;
    SubShurikenDesc.fSpeedPerSec = 30.f;
    SubShurikenDesc.MyOffSet = _float3{ -0.3f ,0.f,-0.3f };
    SubShurikenDesc.pOwner = this;
    SubShurikenDesc.iNumSubShuriken = 0;     // ù��° �� ������

    WeaponDesc.pSubShuriSken[0] = static_cast<CSubShuriken*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_SubShuriken", &SubShurikenDesc));



    SubShurikenDesc.MyOffSet = _float3{ 0.05f, -0.3f, -0.6f };
    SubShurikenDesc.iNumSubShuriken = 1;     // �ι�° �� ������ 

    WeaponDesc.pSubShuriSken[1] = static_cast<CSubShuriken*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_SubShuriken", &SubShurikenDesc));
    
    if (FAILED(__super::Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Weapon_Player"), &WeaponDesc)))
        return E_FAIL;



    CWire_Player::WIRE_DESC		WireDesc{};
    WireDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WireDesc.pSocketBoneMatrix = static_cast<CBody_Player*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Weapon_l");
    WireDesc.pOwner = this;
    WireDesc.InitWorldMatrix = XMMatrixIdentity();
    WireDesc.fRotationPerSec = 30.f;
    WireDesc.fSpeedPerSec = 30.f;

    if (FAILED(__super::Add_PartObject(PART_WIRE, TEXT("Prototype_GameObject_Wire_Player"), &WireDesc)))
        return E_FAIL;



    _float3				KatanaStartLocal = _float3(-0.0776796862, -0.202111647, 0.839834869);
    _float3				KatanaEndLocal = _float3(0.0776796862, 1.43591797, 9.31558323);
    _float3             KatanaMiddleLocal;

    KatanaMiddleLocal.x = (KatanaStartLocal.x + KatanaEndLocal.x) * 0.5f;
    KatanaMiddleLocal.y = (KatanaStartLocal.y + KatanaEndLocal.y) * 0.5f;
    KatanaMiddleLocal.z = (KatanaStartLocal.z + KatanaEndLocal.z) * 0.5f;


    CParticle_Block::PARTICLE_BLOCKEFFECT  BlockDesc = {};
    BlockDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BlockDesc.pOwner = this;
    BlockDesc.SpwanPositionLocal = KatanaMiddleLocal;
    if (FAILED(__super::Add_PartObject(PART_PARTICLE_BLOCK, TEXT("Prototype_GameObject_Particle_BlockEffect"), &BlockDesc)))
        return E_FAIL;



    CParticle_CutAll::PARTICLE_CUTALLEFFECT  CutAllDesc = {};
    CutAllDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    CutAllDesc.pSocketMatrix = static_cast<CBody_Player*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Weapon_l");
    CutAllDesc.pOwner = this;
    CutAllDesc.SpwanPositionLocal = KatanaStartLocal;
    if (FAILED(__super::Add_PartObject(PART_PARTICLE_CUTALL, TEXT("Prototype_GameObject_Particle_CutAllEffect"), &CutAllDesc)))
        return E_FAIL;



    CParticle_Nami::PARTICLE_NAMIEFFECT  NamiDesc = {};
    NamiDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    NamiDesc.pSocketMatrix = static_cast<CBody_Player*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("pelvis");
    NamiDesc.pOwner = this;
    if (FAILED(__super::Add_PartObject(PART_PARTICLE_NAMI, TEXT("Prototype_GameObject_Particle_NamiEffect"), &NamiDesc)))
        return E_FAIL;


    return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    __super::Free();

    // ���ʿ� ��Ʈ ������Ʈ�� ���̾ ��������ʾƼ� �纻�� Free�� ȣ���������.
    // State�� ��������
    // ���̾ ������ʴ� �༮���� m_pOwner �̷��� ����ȵǰڴ� ���� ��ã�ھ�

    // �������� : 1. ���� �ٸ� Ŭ�������� ����� ��ȯ���� ��� �ְ���������
    //           2. ���� Clone Object�ϰ�� �ٸ������� �̳��� ���۷���ī��Ʈ�� �������� �� �纻�� Free�� ȣ����� �ʴ´ٸ� �� �纻�� ����� ��ü�� �����Ҽ�����.
    //           3. ���� ���̾ ����� �ʴ� ������Ʈ�ϰ�� Layer�ʿ��� ȣ���� �ȵǱ⿡ Free�� ȣ��ȵǼ� ���� ���ɼ��� �ֵ�...... 


    Safe_Release(m_pTimeStopRefractionTex);
    Safe_Release(m_pMindControlRefractionTex);

    Safe_Release(m_pBlurMaskTex);

    Safe_Release(m_pFsm);
    Safe_Release(m_pRigidBody);
}
