#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_Snow final : public CGameObject
{
private:
	CParticle_Snow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Snow(const CParticle_Snow& Prototype);
	virtual ~CParticle_Snow() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CParticle_Snow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END