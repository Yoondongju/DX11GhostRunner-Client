#include "..\Public\PartObject.h"
#include "Shader.h"

CPartObject::CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPartObject::CPartObject(const CPartObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPartObject::Initialize(void* pArg)
{
	PARTOBJ_DESC* pDesc = static_cast<PARTOBJ_DESC*>(pArg);

	m_pParentMatrix = pDesc->pParentWorldMatrix;

	if (pDesc->pOwner)
	{
		m_pOwner = pDesc->pOwner;
		//Safe_AddRef(m_pOwner);
	}
	else
	{
		MSG_BOX(L"Owner가 없는 PartObj가 존재할수가있나?");
		assert(nullptr);
	}



	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CPartObject::Priority_Update(_float fTimeDelta)
{
}

void CPartObject::Update(_float fTimeDelta)
{

}

void CPartObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
	return S_OK;
}

HRESULT CPartObject::Bind_WorldMatrix(CShader* pShader, const _char* pContantName)
{
	return pShader->Bind_Matrix(pContantName, &m_WorldMatrix);
}

void CPartObject::Free()
{
	__super::Free();

	//Safe_Release(m_pOwner);
}
