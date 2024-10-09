#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CWeapon_Hel final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _uint* pParentState = { nullptr };
		const _float4x4* pSocketBoneMatrix = { nullptr };
	}WEAPON_DESC;


public:
	class CHelSwordTrail* Get_SwordTrail() { return m_pSwordTrail; }


private:
	CWeapon_Hel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Hel(const CWeapon_Hel& Prototype);
	virtual ~CWeapon_Hel() = default;


public:
	virtual CModel* Get_Model() override { return m_pModelCom; }
	CCollider* Get_Collider() { return m_pColliderCom; }


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
	CShader*			  m_pShaderCom = { nullptr };
	CModel*				  m_pModelCom = { nullptr };
	CCollider*			  m_pColliderCom = { nullptr };

private:
	const _float4x4*	  m_pSocketMatrix = { nullptr };
	_float				  m_fCollisionCoolTime = {};

private:
	class CHelSwordTrail* m_pSwordTrail = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Reday_Trail();


public:
	static CWeapon_Hel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END