#include "..\Public\Sky_Manager.h"
#include "GameObject.h"
#include "Shader.h"
#include "Texture.h"

CSky_Manager::CSky_Manager()
{
}

HRESULT CSky_Manager::Initialize()
{
	return S_OK;
}

void CSky_Manager::Set_HDRTexture(CTexture* pTexture)
{
	Safe_Release(m_pHDRTexture);
	m_pHDRTexture = pTexture;
	Safe_AddRef(pTexture);
}

void CSky_Manager::Set_BRDFTexture(CTexture* pTexture)
{
	Safe_Release(m_pBRDFTexture);
	m_pBRDFTexture = pTexture;
	Safe_AddRef(pTexture);
}

void CSky_Manager::Set_DissolveTexture(CTexture* pTexture)
{
	Safe_Release(m_pDissolveTexture);
	m_pDissolveTexture = pTexture;
	Safe_AddRef(pTexture);
}

void CSky_Manager::Set_NoiseTexture(CTexture* pTexture)
{
	Safe_Release(m_pNoiseTexture);
	m_pNoiseTexture = pTexture;
	Safe_AddRef(pTexture);
}

HRESULT CSky_Manager::Bind_HDRTexture(CShader* pShader)
{
	if(m_pHDRTexture != nullptr)
		return m_pHDRTexture->Bind_ShadeResources(pShader, "g_HDRTexture");

	return S_OK;
}

HRESULT CSky_Manager::Bind_BRDFTexture(CShader* pShader)
{
	if (m_pBRDFTexture != nullptr)
		return m_pBRDFTexture->Bind_ShadeResource(pShader, "g_BRDFTexture", 0);

	return S_OK;
}

HRESULT CSky_Manager::Bind_DissolveTexture(CShader* pShader)
{
	if (m_pDissolveTexture != nullptr)
		return m_pDissolveTexture->Bind_ShadeResource(pShader, "g_DissolveTexture", 0);

	return S_OK;
}

HRESULT CSky_Manager::Bind_NoiseTexture(CShader* pShader, const _char* pConstantName)
{
	if (m_pNoiseTexture != nullptr)
		return m_pNoiseTexture->Bind_ShadeResource(pShader, pConstantName, 0);

	return S_OK;
}

CSky_Manager* CSky_Manager::Create()
{
	CSky_Manager* pInstance = new CSky_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CSkyManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pHDRTexture);
	Safe_Release(m_pBRDFTexture);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pNoiseTexture);
	Safe_Release(m_pNoiseTexture);
}
