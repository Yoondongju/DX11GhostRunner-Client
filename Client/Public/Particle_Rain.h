#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_Rain final : public CGameObject
{
private:
	CParticle_Rain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Rain(const CParticle_Rain& Prototype);
	virtual ~CParticle_Rain() = default;



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
	_float					  m_fDisableTime = { 3.f };


private:
	const _float4x4* m_pSocketMatrix = { nullptr };


private:
	HRESULT Ready_Components();

public:
	static CParticle_Rain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END