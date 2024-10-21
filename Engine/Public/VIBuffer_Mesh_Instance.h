#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Mesh_Instance final : public CVIBuffer_Instancing
{
public:
	typedef struct : public  CVIBuffer_Instancing::INSTANCE_DESC
	{
		ID3D11Buffer* pVB = nullptr;
		ID3D11Buffer* pIB = nullptr;

				
		DXGI_FORMAT					eIndexFormat;
		D3D11_PRIMITIVE_TOPOLOGY	eTopology;
		
		_uint						iIndexCountPerInstance = 6;   // 인스턴스 하나당 인덱스 몇개를 쓰니?						


	}MESHINSTANCE_DESC;


private:
	CVIBuffer_Mesh_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype);
	virtual ~CVIBuffer_Mesh_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Mesh_Instance::MESHINSTANCE_DESC& Desc);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Spread(_float fTimeDelta) override;
	void		 Spread2(_float fTimeDelta);
	void		 Spread3(_float fTimeDelta);

	void		 Spread_Side_XZ(_float fTimeDelta);
	void		 Spread_Side_XY(_float fTimeDelta);


	virtual void Drop(_float fTimeDelta) override;

public:
	void		 ResetTranslation();


public:
	static CVIBuffer_Mesh_Instance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const CVIBuffer_Mesh_Instance::MESHINSTANCE_DESC& Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END