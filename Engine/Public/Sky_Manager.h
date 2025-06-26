#pragma once

#include "Base.h"
BEGIN(Engine)

class CGameObject;

class CSky_Manager final : public CBase
{
private:
	CSky_Manager();
	virtual ~CSky_Manager() = default;
	HRESULT Initialize();
public:
	void Set_HDRTexture(class CTexture* pTexture);
	void Set_BRDFTexture(class CTexture* pTexture);
	void Set_DissolveTexture(class CTexture* pTexture);
	void Set_NoiseTexture(class CTexture* pTexture);
	HRESULT Bind_HDRTexture(class CShader* pShader);
	HRESULT Bind_BRDFTexture(class CShader* pShader);
	HRESULT Bind_DissolveTexture(class CShader* pShader);
	HRESULT Bind_NoiseTexture(class CShader* pShader, const _char* pConstantName);

private:
	class CTexture* m_pDissolveTexture = {nullptr};
	class CTexture* m_pNoiseTexture = {nullptr};
	class CTexture* m_pHDRTexture = {nullptr};
	class CTexture* m_pBRDFTexture = {nullptr};

public:
	static CSky_Manager* Create();
	virtual void Free() override;

};

END