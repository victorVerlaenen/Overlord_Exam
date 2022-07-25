#include "stdafx.h"
#include "MenuScene.h"

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/UberMaterial/UberMaterial.h"


MenuScene::MenuScene() :
	GameScene(L"MenuScene") {}

void MenuScene::Initialize()
{
	//m_SceneContext.settings.showInfoOverlay = true;
	//m_SceneContext.settings.drawPhysXDebug = true;
	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	//Background
	/*auto pBackgroundMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pBackgroundMaterial->SetDiffuseTexture(L"Textures/Wall/Wall_BaseColor.png");
	pBackgroundMaterial->SetNormalTexture(L"Textures/Wall/Wall_Normal.png");
	pBackgroundMaterial->SetSpecularTexture(L"Textures/Wall/Wall_Roughness.png");

	const auto pBackgroundObj = new GameObject();
	const auto pBackgroundModel = new ModelComponent(L"Meshes/BackgroundWall.ovm");
	pBackgroundModel->SetMaterial(pBackgroundMaterial);

	pBackgroundObj->AddComponent(pBackgroundModel);
	pBackgroundObj->GetTransform()->Scale(25.0f, 25.0f, 25.0f);
	pBackgroundObj->GetTransform()->Translate(0, 0, 30);
	pBackgroundObj->GetTransform()->Rotate(0, 0, -90, true);

	AddChild(pBackgroundObj);*/

	auto pSprite = new GameObject();
	pSprite->AddComponent(new SpriteComponent(L"Textures/TestSprite.jpg", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }, 1));
	AddChild(pSprite);

	pSprite->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, .9f);
	pSprite->GetTransform()->Scale(1.f, 1.f, 1.f);

	//Left books pile
	m_pLeftPileOfBooks = AddChild(new GameObject());
	m_pLeftPileOfBooks->GetTransform()->Scale(1.f);
	m_pLeftPileOfBooks->GetTransform()->Translate(-7, 3, 0);
	m_pLeftPileOfBooks->GetTransform()->Rotate(0, 90, 90, true);
	auto pModelComponent = new ModelComponent(L"Meshes/PileOfBooks.ovm");

	m_pBookMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	m_pBookMaterial->SetDiffuseTexture(L"Textures/PileOfBooks/book_base.png");

	pModelComponent->SetMaterial(m_pBookMaterial);
	m_pLeftPileOfBooks->AddComponent(pModelComponent);

	//Right books pile
	m_pRightPileOfBooks = AddChild(new GameObject());
	m_pRightPileOfBooks->GetTransform()->Scale(1.f);
	m_pRightPileOfBooks->GetTransform()->Translate(13, 2, 0);
	m_pRightPileOfBooks->GetTransform()->Rotate(0, -120, 90, true);
	pModelComponent = new ModelComponent(L"Meshes/PileOfBooks.ovm");

	pModelComponent->SetMaterial(m_pBookMaterial);
	m_pRightPileOfBooks->AddComponent(pModelComponent);
	
	//Camera
	/*auto pFixedCamera = AddChild(new FixedCamera());

	pFixedCamera->GetTransform()->Translate(0, 15, -25);

	auto rotation = XMFLOAT4{ 0.15f, 0.0f, 0.0f, 1.f };
	auto cameraRotation = XMLoadFloat4(&rotation);
	pFixedCamera->GetTransform()->Rotate(cameraRotation);
	
	pFixedCamera->GetComponent<CameraComponent>()->SetActive(true);*/

}

void MenuScene::Update()
{
	//const auto time = SceneManager::Get()->GetActiveScene()->GetSceneContext().pGameTime->GetTotal();

	//m_pLeftPileOfBooks->GetComponent<ModelComponent>()->GetTransform()->Rotate(XMFLOAT3{ 0,45.f * time ,0 }, true);
}

void MenuScene::Draw()
{
	//Optional
}

void MenuScene::OnGUI()
{
	m_pBookMaterial->DrawImGui();
}
