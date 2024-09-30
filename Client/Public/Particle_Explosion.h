#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_Explosion final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		

	}PARTICLE_EXPLOSION;

private:
	CParticle_Explosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Explosion(const CParticle_Explosion& Prototype);
	virtual ~CParticle_Explosion() = default;


public:
	void		Set_SpwanPos(const _float3& SpwanPos) { m_pSpwanPos = SpwanPos; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				  m_pShaderCom = { nullptr };
	CTexture*				  m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };


private:
	_float3					  m_pSpwanPos = {};

	_float					  m_fDisableTime = { 3.f };

private:
	HRESULT Ready_Components();

public:
	static CParticle_Explosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END