#include "stdafx.h"
#include "SpikyScene.h"

#include "Materials/SpikyMaterial/SpikyMaterial.h"


SpikyScene::SpikyScene() :
	GameScene(L"SpikyScene") {}

void SpikyScene::Initialize()
{
	//m_SceneContext.settings.showInfoOverlay = true;
	//m_SceneContext.settings.drawPhysXDebug = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	m_pSphere = AddChild(new GameObject());
	m_pSphere->GetTransform()->Scale(15);
	const auto pModelComponent = new ModelComponent(L"Meshes/OctaSphere.ovm");

	m_pSpikyMaterial = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();

	pModelComponent->SetMaterial(m_pSpikyMaterial);
	m_pSphere->AddComponent(pModelComponent);
}

void SpikyScene::Update()
{
	const auto time = SceneManager::Get()->GetActiveScene()->GetSceneContext().pGameTime->GetTotal();

	m_pSphere->GetComponent<ModelComponent>()->GetTransform()->Rotate(XMFLOAT3{ 0,20.f * time ,0 }, true);
}

void SpikyScene::Draw()
{
	//Optional
}

void SpikyScene::OnGUI()
{
	//m_pUberMaterial->DrawImGui();
}
