#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
public:
	typedef struct 
	{
		_float3				CurStart = {};
		_float3				CurEnd = {};
		
		_float				fLifeTime = {};

	}TRAIL_INFO;


private:
	CVIBuffer_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& Prototype);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumSegment);
	virtual HRESULT Initialize(void* pArg) override;



public:
	virtual void		 Update_SwordTrail(_float fTimeDelta, deque<TRAIL_INFO>& TrailInfo );
	virtual void		 Update_ShurikenTrail(_float fTimeDelta, deque<TRAIL_INFO>& TrailInfo);




private:
	_uint				 m_iNumSegment = {};



public:
	static CVIBuffer_Trail* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext , _uint iNumSegment);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END