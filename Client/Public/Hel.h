#pragma once


#include "Client_Defines.h"
#include "Enemy.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CCollider;
class CFsm;
class CRigidBody;
END


BEGIN(Client)

class CHel final : public CEnemy
{
public:
	enum PARTID 
	{ 
		PART_WEAPON, 
		PART_EFFECT, 
		PART_PARTICLE_ATTACK,
		PART_PARTICLE_JUMPSTART,
		PART_PARTICLE_JUMPEND, 
		PART_PARTICLE_BIGSMOKE,
		PART_PARTICLE_SWIRL,
		PART_END 
	};
	enum HEL_ANIMATION
	{
		SLEEP,
		RAISE,
		IDLE,
		SPRINT,
		BACK_BLOCK,
		READY,
		RUN_JUMP,
		HEAD_CUT_DEAD,
		
		IDLE_TO_JUMP,
		JUMP_TO_IDLE,
		
		STUN_HIT,			// 스턴맞고 점프함
		STUN_TO_IDLE,
		ATTACK_TO_STUN,

		DASH_BACK,
		DASH_TO_IDLE_ATTACK,

		ATTACK1,
		ATTACK2,

		CUTSCENE,

		ATTACK3,
		ATTACK4,

		ANIM_END
	};


private:
	CHel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHel(const CHel& Prototype);
	virtual ~CHel() = default;

public:
	virtual  CModel* Get_Model() override { return m_pModel; }
	virtual  CFsm* Get_Fsm() override { return m_pFsm; }
	virtual  CCollider* Get_Collider() override { return m_pColliderCom; }
	virtual  CRigidBody* Get_RigidBody() override { return m_pRigidBody; }

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
	class CHelMotionTrail* Get_MotionTrail() { return m_pMotionTrail; }


public:
	void				Set_ActiveParticleAttack(_bool b) { m_isActiveParticle_Attack = b; }		// 반드시 수작업으로 꺼야한다.
	void				Set_ActiveParticleElectric(_bool b) { m_isActiveParticle_Electric = b; }		// 반드시 수작업으로 꺼야한다.
	void				Set_ActiveParticleBigElectric(_bool b) { m_isActiveParticle_BigElectric = b; }		// 반드시 수작업으로 꺼야한다.


public:
	_bool Check_Collision();
	_bool Check_CollisionGroggy();


private:
	class CHelMotionTrail* m_pMotionTrail = { nullptr };
	_float	fAddMotionTrailTime = { 0.f };  // 모션트레일을 추가할 시간


private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModel = { nullptr };

	CCollider*	m_pColliderCom = { nullptr };
	CFsm*		m_pFsm = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	_float		m_fLandPosY = { -1000.f };


private:
	vector<class CParticle_Attack*>			m_Particle_Attack;
	vector<class CParticle_Electric*>		m_Particle_Electric;
	vector<class CParticle_BigElectric*>	m_Particle_BigElectric;


	_uint									m_iNextUnActiveParticleIndex_Attack = 0;	// 활성화되지 않은 다음 인덱스
	_bool									m_isActiveParticle_Attack = { false };
	_float									m_fAccNextParticleIndexTime_Attack = { 0.f };


	_uint									m_iNextUnActiveParticleIndex_Electric = 0;	// 활성화되지 않은 다음 인덱스
	_bool									m_isActiveParticle_Electric = { false };
	_float									m_fAccNextParticleIndexTime_Electric = { 0.f };


	_uint									m_iNextUnActiveParticleIndex_BigElectric = 0;	// 활성화되지 않은 다음 인덱스
	_bool									m_isActiveParticle_BigElectric = { false };
	_float									m_fAccNextParticleIndexTime_BigElectric = { 0.f };




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
	void	Check_AttackParticle(_float fTimeDelta);
	void	Check_ElectricParticle(_float fTimeDelta);
	void	Check_BigElectricParticle(_float fTimeDelta);



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
	static CHel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END