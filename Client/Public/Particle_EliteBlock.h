#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CParticle_EliteBlock final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_float3		SpwanPositionLocal = {};

	}PARTICLE_BLOCKEFFECT;

private:
	CParticle_EliteBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_EliteBlock(const CParticle_EliteBlock& Prototype);
	virtual ~CParticle_EliteBlock() = default;


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
	_float3						m_SpwanPositionLocal = {};		// 스폰되는 위치
	_bool						m_isPreActive = { false };		// 이전 프레임에 활성화 된적잇니


private:
	HRESULT Ready_Components();

public:
	static CParticle_EliteBlock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END