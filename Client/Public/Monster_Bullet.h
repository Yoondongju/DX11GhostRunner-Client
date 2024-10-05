#pragma once


#include "Client_Defines.h"
#include "PartObject.h"


BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END




BEGIN(Client)

class CMonster_Bullet final : public CPartObject		//	Sniper와 Pistol이 쓰는 총알임니다
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };

	}BULLET_DESC;

private:
	CMonster_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Bullet(const CMonster_Bullet& Prototype);
	virtual ~CMonster_Bullet() = default;


public:
	void				Set_Attacking(_bool b) { m_isAttacking = b; }
	const _float3&		Get_PreAttackPos() { return m_PreAttackPos; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModel = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };

private:
	_bool			m_isAttacking = { false };
	_float3			m_PreAttackPos = {};
	
private:
	_bool			m_isBouncedBullet = { false };
	_float			m_fChangeColorTime = { 0.f };

private:
	void		Check_Collision_Player();
	void		Check_Collision_Enemy();
	void		Check_Collision_Me();

private:
	HRESULT		Ready_Component();
	


public:
	static CMonster_Bullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END