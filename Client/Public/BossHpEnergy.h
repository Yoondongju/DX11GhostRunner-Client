#pragma once
#include "Client_Defines.h"
#include "UIObject.h"



BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CBossHpEnergy final : public CUIObject
{
public:
	typedef struct : public CUIObject::UI_DESC
	{		
		class CEnemy* pOwner = { nullptr };

	}BOSSHP_DESC;

private:
	CBossHpEnergy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossHpEnergy(const CBossHpEnergy& Prototype);
	virtual ~CBossHpEnergy() = default;


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

	class CEnemy*	m_pOwner = { nullptr };

private:
	_float			m_fOriginalSizeX = {};
	_float			m_fOriginalfX = {};

	_float			m_fPreRatio = { };
	_bool			m_isStartDecrease = { false };

private:
	HRESULT Ready_Components();



public:
	static CBossHpEnergy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

