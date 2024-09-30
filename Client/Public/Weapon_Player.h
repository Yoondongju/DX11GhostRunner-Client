#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "SubShuriken.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CTexture;
END

BEGIN(Client)

class CWeapon_Player final : public CPartObject
{
public:
	enum WEAPON_TYPE { KATANA, SHURIKEN, WEAPON_TYPE_END };

	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };

		class CSubShuriken* pSubShuriSken[2] = {nullptr};

	}WEAPON_DESC;

private:
	CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Player(const CWeapon_Player& Prototype);
	virtual ~CWeapon_Player() = default;

	
public:
	void			Set_CurType(WEAPON_TYPE eType)	
	{ 
		m_isChangeWeapon = true;
		m_eCurType = eType;
	}

public:
	WEAPON_TYPE		Get_CurType() { return m_eCurType; }
	WEAPON_TYPE		Get_PreType() { return m_ePreType; }

public:
	CModel*			Get_Model() override { return m_pModelCom[m_eCurType]; }
	CCollider*		Get_Collider() { return m_pColliderCom[m_eCurType]; }

public:
	const _float4x4& Get_ShurikenOriginMatrix() { return m_ShurikenOriginMatrix; }


public:
	_bool					IsAttacking() { return m_isAttacking; }
	void					Set_Attacking(_bool b) 
	{ 
		m_isPreAttaciong = m_isAttacking;
		m_isAttacking = b;
	}
	class CSubShuriken**	Get_SubShuriken() { return m_pSubShuriken; }


public:
	class CSwordTrail* Get_SwordTrail()
	{
		if (KATANA != m_eCurType)
			return nullptr;

		return m_pSwordTrail;
	}

	class CShurikenTrail* Get_ShurikenTrail()
	{
		if (SHURIKEN != m_eCurType)
			return nullptr;

		return m_pShurikenTrail;
	}

public:
	_bool		IsHomingActive() { return m_isHomingShurikenActive; }
	void		Set_HomingShurikenActive(_bool b) { m_isHomingShurikenActive = b; }

	_float4x4*   Get_RotationMatrix_Ptr() { return &m_RotationMatrix; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom[WEAPON_TYPE::WEAPON_TYPE_END] = {nullptr};
	CCollider*	    m_pColliderCom[WEAPON_TYPE::WEAPON_TYPE_END] = { nullptr };
	CTexture*		m_pCreateNoiseTexture = { nullptr };
	_float			m_fDissolveAmount = { 0.f };


	


private:
	_float			m_fShurikenAccTime = { };

	_float			m_fEmissiveEmphasize = { 0.f };
	_bool			m_isEmissiveIncreasing = { true };


	_bool			m_isHomingShurikenActive = { false };
	_float4x4		m_RotationMatrix = {};



private:
	WEAPON_TYPE		m_eCurType = { WEAPON_TYPE::KATANA };
	WEAPON_TYPE		m_ePreType = { WEAPON_TYPE::KATANA };
	_bool			m_isChangeWeapon = { false };

private:
	_float3     m_InitGoDir = { };
	_float3		m_InitPos = {};


	_float3		m_InitPlayerPos = {};



private:
	const _float4x4* m_pSocketMatrix = { nullptr };

	_float4x4		 m_KatanaOriginMatrix = {};
	_float4x4		 m_ShurikenOriginMatrix = {};



	_bool			 m_isAttacking = { false };		// 공격중이니 ?
	_bool			 m_isPreAttaciong = { false };  // 이전 프레임에 공격했었니


private:
	class CSubShuriken*			m_pSubShuriken[2] = { nullptr };
	

private:
	class CSwordTrail*			m_pSwordTrail = { nullptr };
	class CShurikenTrail*		m_pShurikenTrail = { nullptr };

	

private:
	void			LockTransformHoming(_float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Reday_Trail();


public:
	static CWeapon_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END