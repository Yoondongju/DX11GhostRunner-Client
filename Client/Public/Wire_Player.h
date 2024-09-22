#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "VIBuffer_Trail_Instance.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Wire;

END

BEGIN(Client)

class CWire_Player final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };

	}WIRE_DESC;





private:
	CWire_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWire_Player(const CWire_Player& Prototype);
	virtual ~CWire_Player() = default;


public:
	void		Set_Active(_bool b) { m_bActive = b; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom[2] = {nullptr};
	CVIBuffer_Wire*     m_pVIBufferCom = { nullptr };



private:
	const _float4x4* m_pSocketMatrix = { nullptr };

private:
	_bool				m_bActive = { false };

private:
	HRESULT Ready_Components();

public:
	static CWire_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END