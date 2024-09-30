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
	enum PARTID { PART_WEAPON, PART_EFFECT, PART_PARTICLE_BLOCK, PART_END };
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
	_bool Check_Collision();
	_bool Check_CollisionGroggy();


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
	_bool		m_isGroggy = { false };


private:
	HRESULT		Ready_Component();
	HRESULT		Ready_Parts();
	HRESULT		Ready_State();

	HRESULT		Ready_Change_Layer();

	void		Ready_Modify_Animation();

public:
	static CElite* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END