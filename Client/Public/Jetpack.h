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

class CJetpack final : public CEnemy
{
public:
	enum PARTID{ PART_WEAPON , PART_BACKPACK , PART_EFFECT , PART_END};
	enum JETPACK_ANIMATION
	{
		ATTACK_1,
		ATTACK_2,
		FLY_B,
		FLY_F,
		FLY_L,
		FLY_R,
		IDLE,
		SCARED,
		
		FLY_FALL,

		ANIM_END
	};

private:
	CJetpack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CJetpack(const CJetpack& Prototype);
	virtual ~CJetpack() = default;

public:
	virtual CModel*		Get_Model() override { return m_pModel; }
	virtual CFsm*		Get_Fsm() override { return m_pFsm; }
	virtual CRigidBody*	Get_RigidBody() override { return m_pRigidBody; }
	CCollider*			Get_Collider() { return m_pColliderCom; }

public:
	void				SetDead() { m_isDead = true; }
	_bool				IsDead() { return m_isDead; }

	_float				Get_Discard() { return m_fDiscard; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	_bool		Check_Collision();


private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModel = { nullptr };

	CCollider*	m_pColliderCom = { nullptr };
	CFsm*		m_pFsm = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };


private:
	_bool		m_isDead = { false };
	CTexture*	m_pDeadNoiseTexture = { nullptr };
	_float		m_fDiscard = { 0.f };


private:
	HRESULT		Ready_Component();
	HRESULT		Ready_Parts();
	HRESULT		Ready_State();
	void		Ready_Modify_Animation();
	HRESULT		Ready_Change_Layer();


public:
	static CJetpack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END