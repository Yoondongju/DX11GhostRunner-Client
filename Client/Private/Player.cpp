#include "stdafx.h"
#include "Player.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "Body_Player.h"
#include "Weapon_Player.h"

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
#include "Player_DashCutAll.h"

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
    _vector vNewPos = { XMVectorGetX(vPos) + 9 , m_fOffsetY, XMVectorGetZ(vPos)+15 };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);



    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_PlayerUI()))
        return E_FAIL;

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Get_KeyState(KEY::P) == KEY_STATE::TAP)
    {
        if (false == m_bFreeWalk)
            m_bFreeWalk = true;
        else
            m_bFreeWalk = false;
    }


    if (false == m_bFreeWalk)
    {
        if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
            m_pTransformCom->Go_Straight(fTimeDelta);
        if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
            m_pTransformCom->Go_Backward(fTimeDelta);

        if (false == static_cast<CBody_Player*>(m_Parts[PARTID::PART_BODY])->IsLandingWall())
        {
            if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
                m_pTransformCom->Go_Left(fTimeDelta);
            if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
                m_pTransformCom->Go_Right(fTimeDelta);
        }
    }
    else
    {
        m_pRigidBody->Set_IsGravity(false);

        if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
            m_pTransformCom->Go_Straight_FreeWalk(fTimeDelta);
        if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
            m_pTransformCom->Go_Backward_FreeWalk(fTimeDelta);

        if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
            m_pTransformCom->Go_Left_FreeWalk(fTimeDelta);
        if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
            m_pTransformCom->Go_Right_FreeWalk(fTimeDelta);
    }
    
   

    POINT ptMousePos = {};
    GetCursorPos(&ptMousePos);

    
    if (false == static_cast<CBody_Player*>(m_Parts[PARTID::PART_BODY])->IsLandingWall())
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
    }
   
    m_ptOldMousePos = ptMousePos;



    if (m_bShaking)
    {
        if (m_iShakingCount >= 0)
        {
                         
            // 짝수일 때 오른쪽으로 회전
     
            // 짝수일때 이전프레임때 줫던힘 + 이번프레임의 힘의 강도
            // 다시 홀수일때 이번프레임의 힘의 강도의 2배  
            // 이번프레임의 힘의강도는 ?>

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
    CBody_Player* pBodyPart = static_cast<CBody_Player*>(m_Parts[PARTID::PART_BODY]);
    

    Compute_DashCoolTime(fTimeDelta);
    Compute_BlockCoolTime(fTimeDelta);
    Compute_CutAllCoolTime(fTimeDelta);



    m_pFsm->Update(fTimeDelta);
    m_pRigidBody->Update(fTimeDelta, m_fLandPosY + m_fOffsetY, pBodyPart->Get_Collider()->Get_CurPhysXCollision());

    // < PVD상에서 정확하게 충돌하는 지점 확인하고싶으면 m_fOffsetY 주석치고 확인해봐
    // 저거 있으니까 헷갈림
    // 점프버그 수정해야하고 , 마우스 회전시 내 up벡터에 뭐가 문제가잇어  마우스를 회전해도 내 캡슐이 회전되면 안대


    for (auto& pPartObject : m_Parts)
        pPartObject->Update(fTimeDelta);

    
    

    if (pBodyPart->Get_Collider()->Get_CurPhysXCollision() || PLAYER_ANIMATIONID::HOOK_UP == m_pFsm->Get_CurStateIndex())
        m_pRigidBody->Set_IsGravity(false);
    else
        m_pRigidBody->Set_IsGravity(true);

}

void CPlayer::Late_Update(_float fTimeDelta)
{
    // 공중에 떠있니? 
    if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::NONE &&
        XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) > (m_fLandPosY + m_fOffsetY) * 4)
    {
        if (false == static_cast<CBody_Player*>(m_Parts[PARTID::PART_BODY])->IsLandingWall())
        {
            _uint iCurStateIndex = m_pFsm->Get_CurStateIndex();
       
            if (PLAYER_ANIMATIONID::JUMP_START != iCurStateIndex &&
                PLAYER_ANIMATIONID::JUMP_LOOP != iCurStateIndex &&
                PLAYER_ANIMATIONID::JUMP_END != iCurStateIndex &&
                PLAYER_ANIMATIONID::HOOK_UP != iCurStateIndex &&   
                PLAYER_ANIMATIONID::SH_ATTACK != iCurStateIndex &&
                PLAYER_ANIMATIONID::CLIMB != iCurStateIndex)
            {
                // 점프할때 포물선방정식이용한 점프 구현해야하고 
                // 무조건 착지 전이면 점프 상태로 돌입 -> 점프상태일때 이동이 안되야하나?
                // 점프할때 이동 돼 안돼 여부 랑 점프할때 애니메이션 여부
    
                m_Parts[PARTID::PART_BODY]->Get_Model()->SetUp_Animation(CPlayer::PLAYER_ANIMATIONID::JUMP_START, false);
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

    if (FAILED(m_pFsm->Add_State(CPlayer_DashCutAll::Create(this))))
        return E_FAIL;


    if (FAILED(m_pFsm->Add_State(CPlayer_Nami::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_MindControl::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_TimeStop::Create(this))))
        return E_FAIL;


    if (FAILED(m_pFsm->Add_State(CPlayer_Sh_Attack1::Create(this))))
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
    m_pGrapplingPoint = static_cast<CGrapplingPointUI*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_UI"), 0));
    Safe_AddRef(m_pGrapplingPoint);     // 얘는 Crane만들때 만들어줬다 이유: Crane을 알야하는데 후에 찾으려면 너무 비용이들어서
   

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




HRESULT CPlayer::Ready_PartObjects()
{
    /* 실제 추가하고 싶은 파트오브젝트의 갯수만큼 밸류를 셋팅해놓자. */
    m_Parts.resize(PART_END);

    CBody_Player::BODY_DESC		BodyDesc{};
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.pOwner = this;
    BodyDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
        return E_FAIL;


    CWeapon_Player::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WeaponDesc.pSocketBoneMatrix = dynamic_cast<CBody_Player*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Weapon_r");
    WeaponDesc.pOwner = this;
    WeaponDesc.InitWorldMatrix = XMMatrixIdentity();
    WeaponDesc.fRotationPerSec = 30.f;
    WeaponDesc.fSpeedPerSec = 30.f;


    if (FAILED(__super::Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Weapon_Player"), &WeaponDesc)))
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

    // 애초에 파트 오브젝트는 레이어에 담겨잇지않아서 사본의 Free가 호출되지않음.
    // State도 마찬가지
    // 레이어에 담기지않는 녀석들은 m_pOwner 이런걸 쓰면안되겠다 릭을 못찾겠어

    // 깨달은점 : 1. 서로 다른 클래스에서 멤버로 순환참조 들고 있게하지말자
    //           2. 만약 Clone Object일경우 다른곳에서 이놈의 레퍼런스카운트를 증가시켜 이 사본의 Free가 호출되지 않는다면 이 사본의 멤버인 객체를 해제할수없다.
    //           3. 만약 레이어에 담기지 않는 오브젝트일경우 Layer쪽에서 호출이 안되기에 Free가 호출안되서 릭날 가능성도 있따...... 



    Safe_Release(m_pFsm);
    Safe_Release(m_pRigidBody);
    Safe_Release(m_pGrapplingPoint);
}
