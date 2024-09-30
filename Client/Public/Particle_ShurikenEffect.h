#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Point_Instance.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CParticle_ShurikenEffect final : public CGameObject
{
public:
	typedef struct : public CVIBuffer_Instancing::INSTANCE_DESC
	{
		CGameObject*		pShuriken = { nullptr };

	}PARTICLE_SHURIKENEFFECT;

private:
	CParticle_ShurikenEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_ShurikenEffect(const CParticle_ShurikenEffect& Prototype);
	virtual ~CParticle_ShurikenEffect() = default;

public:
	void		Set_Active(_bool b) { m_bActive = b; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	
private:
	CShader*							m_pShaderCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instance*	        m_pVIBufferCom = { nullptr };

private:
	CGameObject*				m_pShuriken = { nullptr };

private:
	_bool						m_bActive = { false };


private:
	HRESULT Ready_Components();

public:
	static CParticle_ShurikenEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END