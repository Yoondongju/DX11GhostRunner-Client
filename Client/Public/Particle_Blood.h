#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_Blood final : public CGameObject
{
private:
	CParticle_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Blood(const CParticle_Blood& Prototype);
	virtual ~CParticle_Blood() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instance*	m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CParticle_Blood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END