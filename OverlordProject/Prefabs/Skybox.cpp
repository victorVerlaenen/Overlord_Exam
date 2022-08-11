#include "stdafx.h"
#include "Skybox.h"

#include "Materials/Skybox/SkyboxMaterial.h"

Skybox::Skybox()
	: GameObject{ }
{
}

void Skybox::Initialize(const SceneContext&)
{
	float scale{ 40};

	const auto pMat0 = MaterialManager::Get()->CreateMaterial<SkyboxMaterial>();
	pMat0->SetSkyboxTexture(L"Textures/Skybox.dds");

	auto component = AddComponent(new ModelComponent(L"Meshes/Cube.ovm", false));
	component->SetMaterial(pMat0);
	GetTransform()->Scale(scale, scale, scale);
}