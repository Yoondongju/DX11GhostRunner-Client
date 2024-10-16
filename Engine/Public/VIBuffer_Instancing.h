#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct
	{
		_uint		iNumInstance;
		_float3		vCenter;
		_float3		vPivot;
		_float3		vRange;
		_float2		vSize;
		_float2		vSpeed;
		_float2		vLifeTime;
		_bool		isLoop;

		_bool		isStartFromOrigin = false;	// 원점에서 출발할거닝

	}INSTANCE_DESC;
protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& Prototype);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& Prototype, _bool isNoCopy);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC & Desc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;

public:
	virtual void Spread(_float fTimeDelta) {};
	virtual void Drop(_float fTimeDelta) {};

protected:
	ID3D11Buffer*				m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};
	D3D11_SUBRESOURCE_DATA		m_InstanceInitialData = {};

	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };
	_uint						m_iNumInstance = { 0 };
	_float3						m_vCenterPos = {};
	_float3						m_vPivotPos = {};
	_float3						m_vRange = {};
	_float2						m_vSize = {};
	_float2						m_vLifeTime = {};
	_bool						m_isLoop = {};
	_bool						m_isStartFromOrigin = { false };	// 원점에서 출발할거니 


	_float* m_pSpeed = { nullptr };


	void* m_pInstanceVertices = { nullptr };


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END