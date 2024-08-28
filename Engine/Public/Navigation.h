#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
private:
	CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _wstring & strNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override;

private:
	vector<class CCell*>				m_Cells;
	_int								m_iCurrentCellIndex = { -1 };

public:
	static CNavigation* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _wstring & strNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END