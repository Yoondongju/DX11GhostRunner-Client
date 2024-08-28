#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
public:
	typedef struct INIT_BONE_DESC
	{
		_int		iParentBoneIndex = {};
		_uint       iNumChildBone = { }; // �ڽ��� ��� ?

		_char		szName[MAX_PATH] = { "" };
		_float4x4   TransformationMatrix = {};
		_float4x4   CombinedTransformaionMatrix = {};

		vector<INIT_BONE_DESC> vecChildBones; // �ڽ� ���鿡 ���� ����

	}INIT_BONE_DESC;


private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}

	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	const _float4x4* Get_CombinedTransformationMatrix_Ptr() const {
		return &m_CombinedTransformationMatrix;
	}

	_matrix Get_TransformationMatrix() const {
		return  XMLoadFloat4x4(&m_TransformationMatrix);
	}

	void Set_TransformationMatrix(_fmatrix TransformationMatrix)
	{
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	_int	Get_ParentBoneIndex() { return m_iParentBoneIndex; }


	void	Set_NumChildBone(_uint iNum) { m_iNumChildBone = iNum; }
	_uint	Get_NumChildBone() { return m_iNumChildBone; }


public:
	HRESULT Initialize(_uint iNumChildBone, _char* pName , _float4x4 TransformationMarix, _int iParentBoneIndex);
	void Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix);


private:
	_char				m_szName[MAX_PATH] = {};

	/* �θ�������� ǥ���� �� ������ �������. */
	_float4x4			m_TransformationMatrix = {};

	/* �� ���� �������� ������� ( �� ������� * �θ��� ����������� ) */
	/* m_TransformationMatrix * Parent`s m_CombinedTransformationMatrix */
	_float4x4			m_CombinedTransformationMatrix = {};
	_int				m_iParentBoneIndex = { -1 };			// �� �θ��� �� �ε���

	_uint				m_iNumChildBone = {};




public:
	static CBone* Create(_uint iNumChildBone, _char* pName , _float4x4 TransformationMarix  ,_int iParentBoneIndex);
	CBone* Clone();
	virtual void Free() override;
};

END