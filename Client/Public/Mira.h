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

class CMira final : public CEnemy
{
public:
	enum PARTID { PART_EFFECT, PART_BULLET,  PART_SHOCKWAVE , PART_END};
	enum MIRA_ANIMATION
	{
		WALKIN_AROUND,
		TURN_L,
		TURN_R,
		IDLE,
		SUISIDE,
		ALERT,
		ATTACK,
		DEATH,
		ANIM_END
	};

private:
	CMira(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMira(const CMira& Prototype);
	virtual ~CMira() = default;

public:
	virtual  CModel*	Get_Model() override { return m_pModel; }
	virtual  CFsm*		Get_Fsm() override { return m_pFsm; }
	virtual  CCollider* Get_Collider() override { return m_pColliderCom; }

public:
	virtual _bool		IsDead() override { return m_isDead; }
	void				SetDead() { m_isDead = true; }


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

private:
	CCollider*	m_pColliderCom = { nullptr };
	CFsm*		m_pFsm = { nullptr };

private:
	_bool		m_isDead = { false };
	CTexture*	m_pDeadNoiseTexture = { nullptr };
	_float		m_fDiscard = { 0.f };




private:
	HRESULT		Ready_Component();
	HRESULT		Ready_State();
	HRESULT		Ready_Change_Layer();

	HRESULT		Reday_Parts();

public:
	static CMira* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END