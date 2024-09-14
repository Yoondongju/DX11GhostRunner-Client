#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CWeapon_Jetpack final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _uint* pParentState = { nullptr };
		const _float4x4* pSocketBoneMatrix = { nullptr };
	}WEAPON_DESC;

private:
	CWeapon_Jetpack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Jetpack(const CWeapon_Jetpack& Prototype);
	virtual ~CWeapon_Jetpack() = default;


public:
	class CModel* Get_Model() override { return m_pModelCom; }
	
	void		  Set_Attacking(_bool b) { m_isAttacking = b; }

	const _float4x4& Get_OriginMatrix() { return m_OriginMatrix; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CCollider*      m_pColliderCom = { nullptr };

private:
	CTexture*	m_pDeadNoiseTexture = { nullptr };
	_float		m_fDiscard = { 0.f };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };

private:
	_bool		m_isAttacking = { false };
	_float4x4   m_OriginMatrix = {};

private:
	HRESULT Ready_Components();

public:
	static CWeapon_Jetpack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END