#pragma once


#include "Client_Defines.h"
#include "ContainerObject.h"

#include "Weapon_Player.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CFsm;
class CRigidBody;
END


BEGIN(Client)

class CPlayer final : public CContainerObject
{
public:
	enum PARTID
	{ 
		PART_BODY, 
		PART_WEAPON,  
		PART_WIRE ,
		PART_PARTICLE_BLOCK,
		PART_PARTICLE_CUTALL,
		PART_PARTICLE_NAMI,
		PART_END 
	};
	enum PLAYER_ANIMATIONID
	{
		CLIMB,
		DASH_B,
		DASH_L,
		DASH_R,
		BLOCK_R1,
		BLOCK_R2,
		BLOCK_R3,
		HOOK_UP,

		BLOCK_L1,
		BLOCK_L2,
		BLOCK_L3,

		ATTACK_R1,
		ATTACK_R2,
		ATTACK_R3,

		ATTACK_L1,
		ATTACK_L2,
		ATTACK_L3,

		JUMP_IDLE,

		DASH_F,
		IDLE,

		JUMP_START,
		JUMP_LOOP,
		JUMP_END,

		RUN,
		RUN_WALL_L,
		RUN_WALL_R,

		SLIDE,
		WALK,

		FORCE_PUSH,

		FURR_AIM_LOOP,							// 정신집중
		FURR_AIM_TO_IDLE,
		FURR_IDLE_TO_AIM,

		RIFT_PICKUP,							// 이거 어따쓰지 ?

		FURR_AIM_TRICUT,

		MIND_CONTROL_START_START,				// 마인드컨트롤

		NAMI_AIM_ATTACK_TO_IDLE,				// 나미동작후에 가로로 한번 쫙 베는거 

		MIND_CONTROL_START_TO_IDLE,				// 마인드컨트롤 해제하는거? 왼쪽손으로 바스라지는 느낌의 애니

		DUMMY,

		NAMI_IDLE_TO_AIM,						// 왼손 쫙 펴는거 

		DUMMY2,


		SH_ATTACK,

		SH_DASH_B,
		SH_DASH_F,
		SH_DASH_L,
		SH_DASH_R,

		SH_IDLE,
		SH_RUN,
		SH_WALK,
		SH_RUN_WALL_L,
		SH_RUN_WALL_R,
		SH_KAT_TO_SHUR,				// 카타나에서 수리켄
		SH_SHUR_TO_KAT,				// 수리켄에서 카타나

		HACK_AIM_LOOP,

		TIME_STOP,					// 시간정지

		DUMMY3,						// 대쉬하면서 한번 베기 (쓰지말자 이상함)
		FURR_DASH_TRICUT,			// 대쉬하면서 여러번 베기

		DEATH_1,
		DEATH_2,

		DRAW_SWORD,
		LOOKAT_HAND,
		SPIN_A,
		SPIN_B,


		PLAYER_ANIMATION_END
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	class CFsm*		   Get_Fsm() override { return m_pFsm; }
	class CRigidBody*  Get_RigidBody() override { return m_pRigidBody; }

	PLAYER_ANIMATIONID	Get_CurAnimationID() { return m_eCurAnimationID; }

	const _float4x4& Get_RotationMatrix() { return m_RotationMatrix; }
	_float4x4*		 Get_RotationMatrixPtr() { return &m_RotationMatrix; }

public:
	_float				Get_OffsetY() { return m_fOffsetY; }

	_float				Get_LandPosY() { return m_fLandPosY; }
	void				Set_LandPosY(_float fLandY) { m_fLandPosY = fLandY; }

	_bool				Get_Shaking() { return m_bShaking; }

	void				Set_JumpProgressTime(_float fTime) { m_fJumpProgressTime = fTime; }
	_float				Get_JumpProgressTime() { return m_fJumpProgressTime; }


	void				Set_CanSwapWeapon(_bool b) { m_isCanSwapWeapon = b; }
	_bool				IsCanSwapWeapon() { return m_isCanSwapWeapon; }


public: // DASH
	void				Set_DashActive(_bool b) { m_bDashActive = b; }
	_bool				IsDashActive() { return m_bDashActive; }

	const _float&		Get_DashCoolTime() { return m_fDashCoolTime; }
	const _float&		Get_DashRemainingTime() { return m_fDashRemainingTime; }

	void				Set_DashRemainingTime(_float fTime) { m_fDashRemainingTime = fTime; }
	void				Set_StartCountDashTime(_bool b) { m_bStartCountDashTime = b; }
	

public: // BLOCK
	void				Set_BlockActive(_bool b) { m_bBlockActive = b; }
	_bool				IsBlockActive() { return m_bBlockActive; }

	const _float&		Get_BlockCoolTime() { return m_fBlockCoolTime; }
	const _float&		Get_BlockRemainingTime() { return m_fBlockRemainingTime; }

	void				Set_BlockRemainingTime(_float fTime) { m_fBlockRemainingTime = fTime; }
	void				Set_StartCountBlockTime(_bool b) { m_bStartCountBlockTime = b; }


public: // CUTALL
	void				Set_CutAllActive(_bool b) { m_bCutAllActive = b; }
	_bool				IsCutAllActive() { return m_bCutAllActive; }

	const _float&		Get_CutAllCoolTime() { return m_fCutAllCoolTime; }
	const _float&		Get_CutAllRemainingTime() { return m_fCutAllRemainingTime; }

	void				Set_CutAllRemainingTime(_float fTime) { m_fCutAllRemainingTime = fTime; }
	void				Set_StartCountCutAllTime(_bool b) { m_bStartCountCutAllTime = b; }


public: // TIMESTOP
	void				Set_TimeStopActive(_bool b) { m_bTimeStopActive = b; }
	_bool				IsTimeStopActive() { return m_bTimeStopActive; }

	const _float&		Get_TimeStopCoolTime() { return m_fTimeStopCoolTime; }
	const _float&		Get_TimeStopRemainingTime() { return m_fTimeStopRemainingTime; }

	void				Set_TimeStopRemainingTime(_float fTime) { m_fTimeStopRemainingTime = fTime; }
	void				Set_StartCountTimeStopTime(_bool b) { m_bStartCountTimeStopTime = b; }


public: // NAMI
	void				Set_NamiActive(_bool b) { m_bNamiActive = b; }
	_bool				IsNamiActive() { return m_bNamiActive; }

	const _float&		Get_NamiCoolTime() { return m_fNamiCoolTime; }
	const _float&		Get_NamiRemainingTime() { return m_fNamiRemainingTime; }

	void				Set_NamiRemainingTime(_float fTime) { m_fNamiRemainingTime = fTime; }
	void				Set_StartCountNamiTime(_bool b) { m_bStartCountNamiTime = b; }


public: // MindControl
	void				Set_MindControlActive(_bool b) { m_bMindControlActive = b; }
	_bool				IsMindControlActive() { return m_bMindControlActive; }

	const _float&		Get_MindControlCoolTime() { return m_fMindControlCoolTime; }
	const _float&		Get_MindControlRemainingTime() { return m_fMindControlRemainingTime; }

	void				Set_MindControlRemainingTime(_float fTime) { m_fMindControlRemainingTime = fTime; }
	void				Set_StartCountMindControlTime(_bool b) { m_bStartCountMindControlTime = b; }


public: // HomingSh
	void				Set_HomingShActive(_bool b) { m_bHomingShActive = b; }
	_bool				IsHomingShActive() { return m_bHomingShActive; }

	const _float&		Get_HomingShCoolTime() { return m_fHomingShCoolTime; }
	const _float&		Get_HomingShRemainingTime() { return m_fHomingShRemainingTime; }

	void				Set_HomingShRemainingTime(_float fTime) { m_fHomingShRemainingTime = fTime; }
	void				Set_StartCountHomingShTime(_bool b) { m_bStartCountHomingShTime = b; }



public:
	CWeapon_Player::WEAPON_TYPE		Get_CurWeaponType()
	{
		return static_cast<CWeapon_Player*>(m_Parts[PARTID::PART_WEAPON])->Get_CurType();
	}
	CWeapon_Player::WEAPON_TYPE		Get_PreWeaponType()
	{
		return static_cast<CWeapon_Player*>(m_Parts[PARTID::PART_WEAPON])->Get_PreType();
	}



public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


	void    Set_Shake(_uint iShakeCount, _float fForce, _fvector vAsix)
	{
		if (iShakeCount % 2 != 0)
		{
			iShakeCount += 1;
		}
		else if (iShakeCount == 0)
			iShakeCount = 2;


		m_iShakingCount = iShakeCount;
		m_iOriginShakingCount = m_iShakingCount;

		m_bShaking = true;
		m_fForce = fForce;
		XMStoreFloat3(&m_ShakeAsix, vAsix);
	}
	void	Active_Shake(_float fForce, _fvector vAsix);



private:
	CFsm*					m_pFsm = { nullptr };
	CRigidBody*				m_pRigidBody = { nullptr };

	PLAYER_ANIMATIONID		m_eCurAnimationID = { PLAYER_ANIMATION_END };
	POINT					m_ptOldMousePos = {};


	_float4x4				m_RotationMatrix = {};


private:
	_float					m_fOffsetY = { 35.f };
	_float					m_fLandPosY = { -1000.f };		// 내가 착지해야할 Y위치는 어디니? 메쉬가 내 발아래있으면 메쉬의 Y위치고 없으면 0 



private:
	_bool				m_bShaking = false;
	_uint				m_iOriginShakingCount = { 0 };
	_int				m_iShakingCount = 0;

	_float				m_fForce = { 0.f };			// 쉐이킹에서 사용할 회전힘의 강도
	_float				m_fCurRotation = { 0.f };	// 쉐이킹에서 사용할 회전값
	_float				m_fPreRotation = { 0.f };	// 쉐이킹에서 사용할 회전값

	_float3				m_ShakeAsix = {};

private:
	_float				m_fJumpProgressTime = { 0.f };	// 점프 진행시간


	_bool				m_isCanSwapWeapon = { true };

private:		// DASH
	_float				m_fDashCoolTime = { 3.f };			// 대쉬 쿨타임
	_float				m_fDashRemainingTime = { 3.f };		// 대쉬 활성화까지 남은 시간
	_bool				m_bDashActive = { true };
	_bool				m_bStartCountDashTime = { false };	// 대쉬 시간을 재야하는 시점


private:		// BLOCK
	_float				m_fBlockCoolTime = { 2.f };			// 막기 쿨타임
	_float				m_fBlockRemainingTime = { 2.f };	// 막기 활성화까지 남은 시간
	_bool				m_bBlockActive = { true };
	_bool				m_bStartCountBlockTime = { false };	// 막기 시간을 재야하는 시점


private:		// CUTALL
	_float				m_fCutAllCoolTime = { 3.f };		// 컷올 쿨타임
	_float				m_fCutAllRemainingTime = { 3.f };	// 컷올 활성화까지 남은 시간
	_bool				m_bCutAllActive = { true };
	_bool				m_bStartCountCutAllTime = { false };	// 컷올 시간을 재야하는 시점


private:		// TimeStop
	_float				m_fTimeStopCoolTime = { 5.f };		
	_float				m_fTimeStopRemainingTime = { 5.f };
	_bool				m_bTimeStopActive = { true };
	_bool				m_bStartCountTimeStopTime = { false };

	_float				m_fTimeDelayLerpRatio = {8.f};		// 게임인스턴스에서 0.1만큼 델타타임에 곱햇으니 얜 절반은 복구시키자


private:		// NAMI
	_float				m_fNamiCoolTime = { 3.f };		
	_float				m_fNamiRemainingTime = { 3.f };	
	_bool				m_bNamiActive = { true };
	_bool				m_bStartCountNamiTime = { false };	


private:		// MINDCONTROL
	_float				m_fMindControlCoolTime = { 3.f };		
	_float				m_fMindControlRemainingTime = { 3.f };	
	_bool				m_bMindControlActive = { true };
	_bool				m_bStartCountMindControlTime = { false };	


private:		// HOMING_SH
	_float				m_fHomingShCoolTime = { 3.f };		
	_float				m_fHomingShRemainingTime = { 3.f };	
	_bool				m_bHomingShActive = { true };
	_bool				m_bStartCountHomingShTime = { false };	




private:
	_bool							m_bFreeWalk = { false };


private:
	HRESULT		Ready_Component();

	HRESULT		Ready_PartObjects();

	HRESULT		Ready_State();

	HRESULT		Ready_PlayerUI();

private:
	void		Compute_DashCoolTime(_float fTimeDelta);
	void		Compute_BlockCoolTime(_float fTimeDelta);
	void		Compute_CutAllCoolTime(_float fTimeDelta);
	void		Compute_TimeStopCoolTime(_float fTimeDelta);
	void		Compute_NamiCoolTime(_float fTimeDelta);
	void		Compute_MindControlCoolTime(_float fTimeDelta);
	void		Compute_HomingShCoolTime(_float fTimeDelta);


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END