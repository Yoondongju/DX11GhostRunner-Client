#pragma once
#include "Client_Defines.h"
#include "UIObject.h"



BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CBossHpPanel final : public CUIObject
{
public:
	enum BOSSTYPE { ELITE , TYPE_END};
	typedef struct : public CUIObject::UI_DESC
	{
		BOSSTYPE	  eBossType = {};
		class CEnemy* pOwner = { nullptr };

	}BOSSHP_DESC;

private:
	CBossHpPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossHpPanel(const CBossHpPanel& Prototype);
	virtual ~CBossHpPanel() = default;

	
public:
	BOSSTYPE	Get_BossType() { return m_eBossType; }


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


	BOSSTYPE		m_eBossType = {};
	class CEnemy*	m_pOwner = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT	Ready_Child();


public:
	static CBossHpPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

