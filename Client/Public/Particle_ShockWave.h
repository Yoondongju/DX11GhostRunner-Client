#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_ShockWave final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };

	}SHOCKWAVE_DESC;

private:
	CParticle_ShockWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_ShockWave(const CParticle_ShockWave& Prototype);
	virtual ~CParticle_ShockWave() = default;



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
	_float					  m_fDisableTime = { 3.f };


private:
	const _float4x4* m_pSocketMatrix = { nullptr };


private:
	HRESULT Ready_Components();

public:
	static CParticle_ShockWave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END