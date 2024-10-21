#pragma once
#include "Client_Defines.h"
#include "UIObject.h"



BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CKillCountUI final : public CUIObject
{
private:
	CKillCountUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKillCountUI(const CKillCountUI& Prototype);
	virtual ~CKillCountUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float			m_fUnActiveTime = { 0.f };
	_float2			m_vTargetSize = {};
	_float4			m_clickedColor = _float4(1.f, 0.f, 0.f, 1.0f);
	_float			m_fDiffTime = { 0.f };
	
private:
	HRESULT Ready_Components();



public:
	static CKillCountUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

