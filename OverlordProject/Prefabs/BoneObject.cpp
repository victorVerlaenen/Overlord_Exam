#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* pMaterial, const float length)
	:GameObject()
	, m_Length(length)
	, m_pMaterial(pMaterial)
{
	
}

void BoneObject::Initialize(const SceneContext& /*sceneContext*/)
{
	GameObject* pEmpty{ new GameObject() };
	AddChild(pEmpty);
	ModelComponent* pModel = pEmpty->AddComponent(new ModelComponent(L"Meshes/Bone.ovm"));
	pModel->SetMaterial(m_pMaterial);
	pEmpty->GetTransform()->Rotate(0, -90, 0, true);
	pEmpty->GetTransform()->Scale(m_Length);
}

void BoneObject::AddBone(BoneObject* pBone)
{
	//locate incoming bone next to parent (this) bone
	pBone->GetTransform()->Translate(m_Length, 0, 0);
	//Add pBone as a child to parent bone (this)
	AddChild(pBone);
}

//PART_2
void BoneObject::CalculateBindPose()
{
	//Store inverse world matrix in m_BindPose
	const XMFLOAT4X4 worldMatrix{ GetTransform()->GetWorld() };
	XMVECTOR worldDeterminant{ XMMatrixDeterminant(XMLoadFloat4x4(&worldMatrix)) };
	const XMMATRIX inverseWorldMatrix{ XMMatrixInverse(&worldDeterminant, XMLoadFloat4x4(&worldMatrix)) };
	XMStoreFloat4x4(&m_BindPose, inverseWorldMatrix);

	//CalculateBindPose for all children
	for(const auto pChildBoneObject : GetChildren<BoneObject>())
	{
		pChildBoneObject->CalculateBindPose();
	}
}
