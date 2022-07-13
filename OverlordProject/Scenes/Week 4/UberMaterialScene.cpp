#include "stdafx.h"
#include "UberMaterialScene.h"
#include "Materials/UberMaterial/UberMaterial.h"


UberMaterialScene::UberMaterialScene() :
	GameScene(L"UberMaterialScene") {}

void UberMaterialScene::Initialize()
{
	//m_SceneContext.settings.showInfoOverlay = true;
	//m_SceneContext.settings.drawPhysXDebug = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	m_pSphere = AddChild(new GameObject());
	m_pSphere->GetTransform()->Scale(15);
	const auto pModelComponent = new ModelComponent(L"Meshes/Sphere.ovm");

	m_pUberMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	m_pUberMaterial->SetDiffuseTexture(L"Textures/Skulls_Diffusemap.tga");
	m_pUberMaterial->SetNormalTexture(L"Textures/Skulls_Normalmap.tga");
	m_pUberMaterial->SetSpecularTexture(L"Textures/Skulls_Heightmap.tga");
	m_pUberMaterial->SetEnvironmentTexture(L"Textures/Sunol_Cubemap.dds");

	pModelComponent->SetMaterial(m_pUberMaterial);
	m_pSphere->AddComponent(pModelComponent);
}

void UberMaterialScene::Update()
{
	const auto time = SceneManager::Get()->GetActiveScene()->GetSceneContext().pGameTime->GetTotal();

	m_pSphere->GetComponent<ModelComponent>()->GetTransform()->Rotate(XMFLOAT3{ 0,45.f * time ,0 }, true);
}

void UberMaterialScene::Draw()
{
	//Optional
}

void UberMaterialScene::OnGUI()
{
	m_pUberMaterial->DrawImGui();
}
