#include "..\Public\Bone.h"

CBone::CBone()
{

}

HRESULT CBone::Initialize(_uint iNumChildBone, _char* pName , _float4x4 TransformationMarix ,_int iParentBoneIndex)
{
	m_iNumChildBone = iNumChildBone;		// �� �ڽ��� ���

	m_iParentBoneIndex = iParentBoneIndex;	// �� �θ� ���� �ε����� ���̴� 

	strcpy_s(m_szName, pName);				// �̸� �������ְ� 

	memcpy(&m_TransformationMatrix, &TransformationMarix, sizeof(_float4x4));

	// ���⼱ ��ġ�����ʿ䰡���� ai�� �����Ծƴϴϱ� !
	//XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
	//  ���� �� �Ĺ��ε� == �� TransformMatrix * �θ��� �Ĺ��ε� (�̶� PreTransformMatrix �Ȱ����ִ������� �̹� �ֻ��� ��Ʈ �θ𿡼� ����)

}

CBone* CBone::Create(_uint iNumChildBone, _char* pName , _float4x4 TransformationMarix , _int iParentBoneIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(iNumChildBone, pName , TransformationMarix ,iParentBoneIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}


void CBone::Free()
{
	__super::Free();

}
