#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail_Instance final : public CVIBuffer_Instancing
{
public:


private:
	CVIBuffer_Trail_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Trail_Instance(const CVIBuffer_Trail_Instance& Prototype);
	virtual ~CVIBuffer_Trail_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC & Desc) override;
	virtual HRESULT Initialize(void* pArg) override;



public:
	virtual void Update_Trail(_float fTimeDelta, const _fvector & vPreStart, const _fvector & vPreEnd , const _fvector& vCurStart,  const _fvector& vCurEnd);
	virtual void Drop(_float fTimeDelta) {}




public:
	static CVIBuffer_Trail_Instance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const CVIBuffer_Instancing::INSTANCE_DESC & Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END