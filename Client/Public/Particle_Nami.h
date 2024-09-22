#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_Nami final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4*  pSocketMatrix = { nullptr };

	}PARTICLE_NAMIEFFECT;

private:
	CParticle_Nami(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Nami(const CParticle_Nami& Prototype);
	virtual ~CParticle_Nami() = default;


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
	const _float4x4*			m_pSocketMatrix = { nullptr };
	_float						m_fDisableTime = { 2.f };


private:
	HRESULT Ready_Components();

public:
	static CParticle_Nami* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END