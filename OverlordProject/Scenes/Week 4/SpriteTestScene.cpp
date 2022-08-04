#include "stdafx.h"
#include "SpriteTestScene.h"

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"

void SpriteTestScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.enableOnGUI = true;

	m_pSprite = new GameObject();
	m_pSprite->AddComponent(new SpriteComponent(L"Textures/TestSprite.jpg", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	AddChild(m_pSprite);

	m_pSprite->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, 0.f);
	m_pSprite->GetTransform()->Scale(1.f, 1.f, 1.f);

	//Simple Level
	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Meshes/MenuScene.ovm"));

	const auto pBeamsMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pBeamsMaterial->SetDiffuseTexture(L"Textures/MenuScene/Beams_diffuse.png");

	const auto pFloorMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pFloorMaterial->SetDiffuseTexture(L"Textures/MenuScene/floor_diffuse.png");

	const auto pPropsMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pPropsMaterial->SetDiffuseTexture(L"Textures/MenuScene/Building_diffuse.png");

	const auto pWoodMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pWoodMaterial->SetDiffuseTexture(L"Textures/MenuScene/Wood_diffuse.png");

	const auto pWallMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pWallMaterial->SetDiffuseTexture(L"Textures/MenuScene/Wall_diffuse.png");

	const auto pPillarMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pPillarMaterial->SetDiffuseTexture(L"Textures/MenuScene/Pillars_diffuse.png");

	const auto pDoorMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pDoorMaterial->SetDiffuseTexture(L"Textures/MenuScene/Door_diffuse.png");

	const auto pChainsMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pChainsMaterial->SetDiffuseTexture(L"Textures/MenuScene/Door_diffuse.png");

	pLevelMesh->SetMaterial(pFloorMaterial, 0);
	pLevelMesh->SetMaterial(pPropsMaterial, 1);
	pLevelMesh->SetMaterial(pWoodMaterial, 2);
	pLevelMesh->SetMaterial(pWallMaterial, 3);
	pLevelMesh->SetMaterial(pBeamsMaterial, 4);
	pLevelMesh->SetMaterial(pDoorMaterial, 5);
	pLevelMesh->SetMaterial(pPillarMaterial, 6);
	pLevelMesh->SetMaterial(pChainsMaterial, 7);
	pLevelMesh->SetMaterial(pPropsMaterial, 8);
	pLevelObject->GetTransform()->Scale(.01f, .01f, .01f);

	//Camera
	const auto pFixedCamera = AddChild(new FixedCamera());

	pFixedCamera->GetTransform()->Translate(10, 1.8f, 4.4f);

	constexpr auto rotation = XMFLOAT4{ -0.00764465f, -0.474536f, -0.00412144f, 1.f };
	const auto cameraRotation = XMLoadFloat4(&rotation);
	pFixedCamera->GetTransform()->Rotate(cameraRotation);

	pFixedCamera->GetComponent<CameraComponent>()->SetActive(true);
}

void SpriteTestScene::Update()
{
	m_pSprite->GetTransform()->Rotate(0.f, 0.f, m_TotalRotation);

	if (m_AutoRotate)
	{
		m_TotalRotation += 30.f * m_SceneContext.pGameTime->GetElapsed();
		m_TotalRotation = fmod(m_TotalRotation, 360.f);
	}
}

void SpriteTestScene::OnGUI()
{
	const auto pSpriteComponent = m_pSprite->GetComponent<SpriteComponent>();

	ImGui::SliderFloat2("Position", ConvertUtil::ToImFloatPtr(pSpriteComponent->GetTransform()->GetPosition()), 0, m_SceneContext.windowWidth);
	ImGui::SliderFloat2("Scale", ConvertUtil::ToImFloatPtr(pSpriteComponent->GetTransform()->GetScale()), 0, 3);
	ImGui::SliderFloat2("Pivot", ConvertUtil::ToImFloatPtr(pSpriteComponent->GetPivot()), 0, 1);
	ImGui::ColorEdit4("Color", ConvertUtil::ToImFloatPtr(pSpriteComponent->GetColor()));
	ImGui::Checkbox("AutoRotate", &m_AutoRotate);

	ImGui::BeginDisabled(m_AutoRotate);
	ImGui::SliderFloat("Rotation", &m_TotalRotation, 0, 360);
	ImGui::EndDisabled();
}
