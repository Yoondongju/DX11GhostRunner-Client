#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
public:


private:
	CVIBuffer_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& Prototype);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;



public:
	virtual void		 Update_Trail(_float fTimeDelta, const _fvector & vPreStart, const _fvector & vPreEnd , const _fvector& vCurStart,  const _fvector& vCurEnd);
	_float3				 CatmullRom(const _fvector & vPreStart, const _fvector & vPreEnd, const _fvector & vCurStart, const _fvector & vCurEnd, float t);


private:
	_uint				 m_iNumPanel = {};



public:
	static CVIBuffer_Trail* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END