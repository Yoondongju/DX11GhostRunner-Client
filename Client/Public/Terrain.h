#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumX, _uint iNumZ  , _uint iTextureIndex);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CShader*				m_pShaderCom = { nullptr };
	class CTexture*				m_pTextureCom = { nullptr };
	class CVIBuffer_Terrain*	m_pVIBufferCom = { nullptr };

	_uint						m_iNumX = {};
	_uint						m_iNumZ = {};

	_uint						m_iTextureIndex = {};

private:
	HRESULT Ready_Components();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumX , _uint iNumZ ,_uint iTextureIndex );
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END