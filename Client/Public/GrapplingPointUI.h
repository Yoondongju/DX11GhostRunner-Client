#pragma once
#include "Client_Defines.h"
#include "UIObject.h"



BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CGrapplingPointUI final : public CUIObject
{
public:
	enum GRAPPLINGPOINT_TYPE { TYPE_DOT , TYPE_FRAME , TYPE_OUTRING, TYPE_END};

	typedef struct
	{
		GRAPPLINGPOINT_TYPE		eType = { TYPE_END };
		class CStatic_Object*	pCrane = { nullptr };

	}GRAPPLINGPOINT_DESC;



private:
	CGrapplingPointUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGrapplingPointUI(const CGrapplingPointUI& Prototype);
	virtual ~CGrapplingPointUI() = default;


public:
	class CStatic_Object*		Get_Crane()  { return m_pCrane; } 
	_bool						Get_FindPlayer() { return m_bFindPlayer; }

	_float						Get_Alpha() { return m_fAlpha; }

public:
	_bool						IsActive() { return m_bActivate; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = {nullptr};
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

	GRAPPLINGPOINT_TYPE		m_eType = { TYPE_END };


	class CStatic_Object*				m_pCrane = { nullptr };
	_bool								m_bFindPlayer = { false };

	_float								m_fAlpha = { 0.f };
					

private:
	HRESULT Ready_Components();


private:
	_bool	Find_Player(_float fTimeDelta);


public:
	static CGrapplingPointUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

