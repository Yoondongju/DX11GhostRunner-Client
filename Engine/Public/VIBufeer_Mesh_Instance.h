#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Mesh_Instance final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Mesh_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype);
	virtual ~CVIBuffer_Mesh_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC & Desc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Spread(_float fTimeDelta) override;
	virtual void Drop(_float fTimeDelta) override;

public:
	static CVIBuffer_Mesh_Instance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const CVIBuffer_Instancing::INSTANCE_DESC & Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END