#pragma once

#include "Client_Defines.h"
#include "PartObject.h"


BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CTexture;
END


BEGIN(Client)

class CSubShuriken final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_float3		MyOffSet = {};
		_uint       iNumSubShuriken = {};		// 몇번째 서브수리켄이니 ?  0,1,.. 

	}SUBSHURIKEN_DESC;


private:
	CSubShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSubShuriken(const CSubShuriken& Prototype);
	virtual ~CSubShuriken() = default;

public:
	CCollider*	Get_Collider() { return m_pColliderCom; }


	class CShurikenTrail* Get_ShurikenTrail()
	{
		return m_pShurikenTrail;
	}


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float4x4* pParentWMatrix , _bool isAttacking, _float3 GoDir, _bool isLeft, _float3 InitPlayerPos ,_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CCollider*  m_pColliderCom = { nullptr };
	CTexture*	m_pCreateNoiseTexture = { nullptr };
	_float		m_fDissolveAmount = { 0.f };



	_float			m_fShurikenAccTime = { };

	_float		m_fEmissiveEmphasize = { 0.f };
	_bool		m_isEmissiveIncreasing = { true };


private:
	_float3		m_MyOffSet = {};

private:
	class CShurikenTrail* m_pShurikenTrail = { nullptr };




private:
	HRESULT Ready_Components();
	HRESULT	Ready_Trail(_uint iNumSubShuriken);

public:
	static CSubShuriken* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END