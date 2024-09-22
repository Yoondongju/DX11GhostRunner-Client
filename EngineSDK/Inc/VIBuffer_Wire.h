#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Wire final : public CVIBuffer
{
private:
	CVIBuffer_Wire(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Wire(const CVIBuffer_Wire& Prototype);
	virtual ~CVIBuffer_Wire() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Update_VertexData(const _fvector & vStart, const _fvector & vEnd);

public:
	static CVIBuffer_Wire* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END