#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CVIBuffer_Cube;
END

BEGIN(Client)

class CSky final : public CGameObject
{
private:
	enum TEXTURE { TEXTURE_ENV, TEXTURE_HDR, TEXTURE_BRDF, TEXTURE_END };
	CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSky(const CSky& Prototype);
	virtual ~CSky() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShaderCom = { nullptr };
	class CTexture*				m_pTextureCom[TEXTURE_END] = { nullptr };
	CVIBuffer_Cube*				m_pModelCom = { nullptr };

	CTransform*		  m_pFreeCameraTransform = { nullptr };

private:
	_float				m_fTime = {};

private:
	HRESULT Ready_Components();

public:
	static CSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END