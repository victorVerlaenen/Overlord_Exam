#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"

#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_1::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	ColorMaterial* pMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial>() };
	pMaterial->SetColor(XMFLOAT4(.5f, .5f, .5f, 1));

	//Create root object
	GameObject* pRoot{ new GameObject() };

	//Initialze first bone
	m_pBone0 = new BoneObject(pMaterial, 15.f);
	pRoot->AddChild(m_pBone0);

	//Initialize second bone
	m_pBone1 = new BoneObject(pMaterial, 15.f);
	m_pBone0->AddBone(m_pBone1);

	//Add pRoot to scenegraph
	AddChild(pRoot);
}

void SoftwareSkinningScene_1::Update()
{
	if(m_AutoRotate == false)
	{
		return;
	}

	//Update m_BoneRotation: fluctuate between -45 and 45 degrees at speed of 45 degrees per sec
	m_BoneRotation += 45.f * m_SceneContext.pGameTime->GetElapsed() * m_RotationSign;
	if (std::abs(m_BoneRotation) >= 45.f)
	{
		m_RotationSign = -m_RotationSign;
	}
	m_pBone0->GetTransform()->Rotate(0.f, 0.f, m_BoneRotation);
	m_Bone0Rotation[0] = 0;
	m_Bone0Rotation[1] = 0;
	m_Bone0Rotation[2] = m_BoneRotation;
	m_pBone1->GetTransform()->Rotate(0.f, 0.f, -m_BoneRotation * 2.f);
	m_Bone1Rotation[0] = 0;
	m_Bone1Rotation[1] = 0;
	m_Bone1Rotation[2] = -m_BoneRotation * 2.f;
}

void SoftwareSkinningScene_1::OnGUI()
{
	ImGui::Checkbox("Auto Rotate", &m_AutoRotate);

	if (ImGui::DragFloat3("Bone 0 : ROT", m_Bone0Rotation, 1.f, -360.0f, 360.f))
	{
		m_pBone0->GetTransform()->Rotate(m_Bone0Rotation[0], m_Bone0Rotation[1], m_Bone0Rotation[2], true);
	}
	if (ImGui::DragFloat3("Bone 1 : ROT", m_Bone1Rotation, 1.f, -360.0f, 360.f))
	{
		m_pBone1->GetTransform()->Rotate(m_Bone1Rotation[0], m_Bone1Rotation[1], m_Bone1Rotation[2], true);
	}
}
