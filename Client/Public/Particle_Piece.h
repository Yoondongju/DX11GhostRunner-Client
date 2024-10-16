#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Mesh_Instance;
END

BEGIN(Client)

class CParticle_Piece final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_float3		     SpwanPositionLocal = {};

	}PARTICLE_DESC;

private:
	CParticle_Piece(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Piece(const CParticle_Piece& Prototype);
	virtual ~CParticle_Piece() = default;



public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				 m_pShaderCom = { nullptr };
	CTexture*			     m_pTextureCom = { nullptr };
	CVIBuffer_Mesh_Instance* m_pVIBufferCom = { nullptr };


private:
	_int					 m_iRandomSpread = { 0 };

private:
	_float3						m_SpwanPositionLocal = {};
	_float						m_fDisableTime = { 1.f };

private:
	HRESULT Ready_Components();

public:
	static CParticle_Piece* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END