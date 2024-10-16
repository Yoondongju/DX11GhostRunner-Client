#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Point_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC & Desc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Spread(_float fTimeDelta) override;

	virtual void Drop(_float fTimeDelta) override;


public:
	void		 DirectionSpread(_float fTimeDelta, _fvector vDir);
	void		 Spread_Side_X(_float fTimeDelta);	// 양 옆으로 퍼지는 스프레드
	void		 Spread_Side_XZ(_float fTimeDelta);
	void		 Spread_Side_XY(_float fTimeDelta);

	


	void		 ForTrailLifeTimeMul(_float fTimeDelta, _fvector vDir, _float fTrailDuration);		// 궤적을 남길 방향  // 트레일을 위한 라이프타임 증식

	void		 ResetTranslation();



public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const CVIBuffer_Instancing::INSTANCE_DESC & Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END