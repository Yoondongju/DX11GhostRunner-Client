#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CWeapon_Player : public CPartObject
{
public:
	enum WEAPON_TYPE { KATANA, SHURIKEN, WEAPON_TYPE_END };

	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _uint* pParentState = { nullptr };
		const _float4x4* pSocketBoneMatrix = { nullptr };
	}WEAPON_DESC;

protected:
	CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Player(const CWeapon_Player& Prototype);
	virtual ~CWeapon_Player() = default;

	
public:
	void			Set_CurType(WEAPON_TYPE eType) { m_eCurType = eType; }
	WEAPON_TYPE		Get_CurType() { return m_eCurType; }

public:
	CModel*			Get_Model() override { return m_pModelCom[m_eCurType]; }
	CCollider*		Get_Collider() { return m_pColliderCom[m_eCurType]; }

public:
	const _float4x4& Get_OriginMatrix() { return m_OriginMatrix; }

public:
	void			Set_Attacking(_bool b) { m_isAttacking = b; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom[WEAPON_TYPE::WEAPON_TYPE_END] = {nullptr};
	CCollider*	    m_pColliderCom[WEAPON_TYPE::WEAPON_TYPE_END] = { nullptr };

	WEAPON_TYPE		m_eCurType = { WEAPON_TYPE::KATANA };


protected:
	const _float4x4* m_pSocketMatrix = { nullptr };
	_float4x4		 m_OriginMatrix = {};

	_bool			 m_isAttacking = { false };		// 공격중이니 ?


private:
	HRESULT Ready_Components();
	HRESULT Ready_SubShuriken();



public:
	static CWeapon_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END