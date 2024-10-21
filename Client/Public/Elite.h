#pragma once


#include "Client_Defines.h"
#include "Enemy.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CCollider;
class CFsm;
END


BEGIN(Client)

class CElite final : public CEnemy
{
public:
	enum PARTID { PART_WEAPON, PART_EFFECT, PART_PARTICLE_BLOCK, PART_PARTICLE_DASHBLOCK, PART_END };
	enum ELITE_ANIMATION
	{
		IDLE_TO_ALERT,
		IDLE,
		WALK_L,
		WALK_R,
		BLOCK_F,
		BLOCK_L,
		BLOCK_R,
		DEATH_1,
		DEATH_2,
		WALK_B,
		WALK_F,
		ALERT_TO_TURBO,
		TURBO_LOOP,
		TURBO_TO_DASH,
		HIT_STUN,
		ALERT_LOOP,
		DASH_TO_ALERDLB,

		CUTSCENE,

		ANIM_END
	};


private:
	CElite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElite(const CElite& Prototype);
	virtual ~CElite() = default;

public:
	virtual  CModel*	Get_Model() override { return m_pModel; }
	virtual  CFsm*		Get_Fsm() override { return m_pFsm; }
	virtual  CCollider* Get_Collider() override { return m_pColliderCom; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool				IsDead() { return m_isDead; }
	void				SetDead() { m_isDead = true; }

public:
	void				Set_Groggy(_bool b) { m_isGroggy = b; }
	_bool				IsGroggy() { return m_isGroggy; }

public:
	void				Set_Energy(_float f) { m_fEnergy = f; }
	_float				Get_Energy() { return m_fEnergy; }
	

	void				Set_Hp(_float f) { m_fHp = f; }
	_float				Get_Hp() { return m_fHp; }

	_bool				IsPage2() { return m_isEnterPage2; }


public:
	class CEliteMotionTrail* Get_MotionTrail() { return m_pMotionTrail; }


public:
	_bool Check_Collision();
	_bool Check_CollisionGroggy();




private:
	class	CEliteMotionTrail* m_pMotionTrail = { nullptr };
	_float	m_fAddMotionTrailTime = { 0.f };  // 모션트레일을 추가할 시간


private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModel = { nullptr };

	CCollider*	m_pColliderCom = { nullptr };
	CFsm*		m_pFsm = { nullptr };

private:
	_bool		m_isDead = { false };
	CTexture*	m_pDeadNoiseTexture = { nullptr };
	_float		m_fDiscard = { 0.f };

private:
	_float		m_fEnergy = {};
	_float		m_fHp = {};

	_bool		m_isEnterPage2 = { false };


	_float		m_fCollisionCoolTime = {};


private:
	_bool		m_isGroggy = { false };

private:
	PxTransform				m_PxTransform = {};
	PxRigidDynamic*			m_pPxRigidDynamic = { nullptr };
	PxShape*				m_pShape = { nullptr };

	PxVec3					m_vDir = {};
	PxReal					m_vDepth = {};

	CGameObject*			m_pCollisionDestObject = { nullptr };		// 내가 충돌한 옵젝


private:
	void		PhysXComputeCollision();

private:
	HRESULT		Ready_Component();
	HRESULT		Ready_Parts();
	HRESULT		Ready_State();

	HRESULT		Ready_PhysX();

	HRESULT		Ready_Change_Layer();

	void		Ready_Modify_Animation();


	HRESULT		Create_BossHp();

public:
	static CElite* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END