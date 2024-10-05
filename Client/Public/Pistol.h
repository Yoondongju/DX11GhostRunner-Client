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

class CPistol final : public CEnemy
{
public:
	enum PARTID { PART_WEAPON, PART_EFFECT, PART_BULLET,PART_END };
	enum PISTOL_ANIMATION
	{
		ATTACK,
		DEATH_1,
		DEATH_2,
		IDLE_1,
		WALK_B,
		WALK_F,
		WALK_L,
		WALK_R,
		IDLE_2,
		TURN_L,
		TURN_R,

		ANIM_END
	};

private:
	CPistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPistol(const CPistol& Prototype);
	virtual ~CPistol() = default;

public:
	virtual CModel* Get_Model() override { return m_pModel; }
	virtual CFsm*	Get_Fsm() override { return m_pFsm; }
	virtual CCollider* Get_Collider() override { return m_pColliderCom; }


public:
	void			SetDead() { m_isDead = true; }
	virtual _bool	IsDead() override { return m_isDead; }

	_float			Get_Discard() { return m_fDiscard; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	_bool		 Check_Collision();
	virtual void Check_CollByTargetEnemy() override;
	virtual void Check_Collision_Me() override;


private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModel = { nullptr };

	CCollider*  m_pColliderCom = { nullptr };
	CFsm*		m_pFsm = { nullptr };
	
private:
	_bool		m_isDead = { false };
	CTexture*	m_pDeadNoiseTexture = { nullptr };
	_float		m_fDiscard = { 0.f };





private:
	HRESULT		Ready_Component();
	HRESULT		Ready_Parts();
	HRESULT		Ready_State();
	HRESULT		Ready_Change_Layer();


public:
	static CPistol* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END