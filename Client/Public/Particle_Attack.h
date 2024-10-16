#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_Attack final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };

	}PARTICLE_EFFECT;

private:
	CParticle_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Attack(const CParticle_Attack& Prototype);
	virtual ~CParticle_Attack() = default;


public:
	void		Set_FirstActive(_bool b);
	void		Set_Active(_bool b) { m_isActive = b; }

	_bool		IsFirstActive() { return m_isFirstActive; }
	_bool		IsActive() { return m_isActive; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };


private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	_float						m_fDisableTime = { 1.f };

private:
	_bool						m_isFirstActive = { false };
	_bool						m_isActive = { false };

private:
	HRESULT Ready_Components();

public:
	static CParticle_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END