#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_Swirl final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };

	}PARTICLE_EFFECT;

private:
	CParticle_Swirl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Swirl(const CParticle_Swirl& Prototype);
	virtual ~CParticle_Swirl() = default;

public:
	virtual void			SetActiveMyParticle(_bool b, _bool isOtherFlag = false) override;


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
	CTexture* m_pFlowMapTextureCom = { nullptr };

	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };


private:
	const _float4x4*			m_pSocketMatrix = { nullptr };
	_float						m_fDisableTime = { 2.f };

private:
	_float3						m_test = {};
	_bool						m_isFirstActive = { false };

private:
	HRESULT Ready_Components();

public:
	static CParticle_Swirl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END