#include "stdafx.h"
#include "MenuScene.h"

#include "PyreScene.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Prefabs/Skybox.h"
#include "Prefabs/Button/Button.h"


MenuScene::MenuScene(bool isPause) :
	GameScene(L"MenuScene")
	, m_IsPause(isPause)
{}

void MenuScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = false;

	//Light
	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//Sound
	SoundManager::Get()->GetSystem()->createStream("Resources/Audio/MenuTrack.mp3", FMOD_LOOP_NORMAL, nullptr, &m_pMenuTrack);
	SoundManager::Get()->GetSystem()->playSound(m_pMenuTrack, nullptr, true, &m_pChannel2D);
	m_pChannel2D->setVolume(.1f);

	//Background
	m_pSkyBox = AddChild(new Skybox());

	//MenuUI
	const auto pMenuSprite = AddChild(new GameObject());
	pMenuSprite->AddComponent(new SpriteComponent(L"Textures/MenuScene/MenuPiece.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));

	pMenuSprite->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, .8f);
	pMenuSprite->GetTransform()->Scale(.7f);

	const auto pNavigateStickSprite = AddChild(new GameObject());
	pNavigateStickSprite->AddComponent(new SpriteComponent(L"Textures/MenuScene/NavigateStick.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));

	pNavigateStickSprite->GetTransform()->Translate(m_SceneContext.windowWidth / 2 - 200, m_SceneContext.windowHeight - 50, 0);

	const auto pSelectButtonSprite = AddChild(new GameObject());
	pSelectButtonSprite->AddComponent(new SpriteComponent(L"Textures/MenuScene/SelectButton.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));

	pSelectButtonSprite->GetTransform()->Translate(m_SceneContext.windowWidth / 2 + 150, m_SceneContext.windowHeight - 50, 0);

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

	//Buttons
	const auto inputAction = InputAction(Select, InputState::pressed, -1, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);

	if (m_IsPause == false)
	{
		AddMainMenuButtons();
	}
	else
	{
		AddPauseButtons();
	}
}

void MenuScene::Update()
{
	//Rotate sky
	const auto time = SceneManager::Get()->GetActiveScene()->GetSceneContext().pGameTime->GetTotal();
	m_pSkyBox->GetComponent<ModelComponent>()->GetTransform()->Rotate(XMFLOAT3{ 0,1.f * time ,0 }, true);

	//Navigate between buttons
	if (m_Navigated == true)
	{
		m_NavigationTimer += m_SceneContext.pGameTime->GetElapsed();
		if (m_NavigationTimer >= m_NavigationCooldown || std::abs(m_SceneContext.pInput->GetThumbstickPosition().y) < 0.01f)
		{
			m_NavigationTimer = 0;
			m_Navigated = false;
		}
	}
	float leftStickPos{ 0 };
	constexpr float epsilon{ 0.01f };
	if (std::abs(leftStickPos) < epsilon && m_Navigated == false)
	{
		leftStickPos = m_SceneContext.pInput->GetThumbstickPosition().y;
		int selectedButton{};
		for (int i{}; i < m_pButtons.size(); ++i)
		{
			if (m_pButtons[i]->GetSelected() == true)
			{
				selectedButton = i;
				break;
			}
		}
		if (leftStickPos < 0)
		{
			if (m_pButtons[selectedButton]->GetSelected() == true)
			{
				m_pButtons[selectedButton]->SetSelected(false);
				m_pButtons[(selectedButton == m_pButtons.size() - 1 ? 0 : selectedButton + 1)]->SetSelected(true);
				m_Navigated = true;
			}
		}
		else if (leftStickPos > 0)
		{
			if (m_pButtons[selectedButton]->GetSelected() == true)
			{
				m_pButtons[selectedButton]->SetSelected(false);
				m_pButtons[(selectedButton == 0 ? m_pButtons.size() : selectedButton) - 1]->SetSelected(true);
				m_Navigated = true;
			}
		}
	}
}

void MenuScene::Draw()
{
	TextRenderer::Get()->DrawText(ContentManager::Load<SpriteFont>(L"SpriteFonts/Nightwarrior_32.fnt"),
		L"Navigate",
		{ m_SceneContext.windowWidth / 2 - 170, m_SceneContext.windowHeight - 66 },
		XMFLOAT4{ Colors::White });

	TextRenderer::Get()->DrawText(ContentManager::Load<SpriteFont>(L"SpriteFonts/Nightwarrior_32.fnt"),
		L"Select",
		{ m_SceneContext.windowWidth / 2 + 180, m_SceneContext.windowHeight - 66 },
		XMFLOAT4{ Colors::White });
}

void MenuScene::OnGUI()
{

}

void MenuScene::OnSceneActivated()
{
	m_pChannel2D->setPaused(false);
}

void MenuScene::OnSceneDeactivated()
{
	m_pChannel2D->setPaused(true);

	const auto pSceneManager = SceneManager::Get();
	const auto pThisScene = static_cast<GameScene*>(this);
	pSceneManager->RemoveGameScene(pThisScene, true);
}

void MenuScene::AddPauseButtons()
{
	m_pButtons.emplace_back(AddChild(new Button(L"CONTINUE", XMFLOAT2{ m_SceneContext.windowWidth / 2 - 90, m_SceneContext.windowHeight / 2 - 140 }, XMFLOAT4{ Colors::White }, Select)));
	m_pButtons[0]->SetPressedFunction([]()
		{
			const auto pSceneManager = SceneManager::Get();
			pSceneManager->SetActiveGameScene(L"PyreScene");
		});
	m_pButtons[0]->SetSelected(true);

	m_pButtons.emplace_back(AddChild(new Button(L"RESTART", XMFLOAT2{ m_SceneContext.windowWidth / 2 - 80, m_SceneContext.windowHeight / 2 - 70 }, XMFLOAT4{ Colors::White }, Select)));
	m_pButtons[1]->SetPressedFunction([]()
		{
			const auto pSceneManager = SceneManager::Get();
			//pSceneManager->PreviousScene();
			const auto pPyreScene = pSceneManager->GetScene(L"PyreScene");
			pSceneManager->RemoveGameScene(pPyreScene, true);
			pSceneManager->AddGameScene(new PyreScene());
			pSceneManager->SetActiveGameScene(L"PyreScene");
		});

	m_pButtons.emplace_back(AddChild(new Button(L"MAIN MENU", XMFLOAT2{ m_SceneContext.windowWidth / 2 - 110, m_SceneContext.windowHeight / 2 }, XMFLOAT4{ Colors::White }, Select)));
	m_pButtons[2]->SetPressedFunction([]()
		{
			const auto pSceneManager = SceneManager::Get();
			const auto pPyreScene = pSceneManager->GetScene(L"PyreScene");
			pSceneManager->RemoveGameScene(pPyreScene, true);
			pSceneManager->AddGameScene(new MenuScene());
			pSceneManager->NextScene();
		});

	m_pButtons.emplace_back(AddChild(new Button(L"QUIT", XMFLOAT2{ m_SceneContext.windowWidth / 2 - 45, m_SceneContext.windowHeight / 2 + 70 }, XMFLOAT4{ Colors::White }, Select)));
	m_pButtons[3]->SetPressedFunction([]()
		{
			PostQuitMessage(0);
		});
}

void MenuScene::AddMainMenuButtons()
{
	m_pButtons.emplace_back(AddChild(new Button(L"PLAY", XMFLOAT2{ m_SceneContext.windowWidth / 2 - 40, m_SceneContext.windowHeight / 2 - 110 }, XMFLOAT4{ Colors::White }, Select)));
	m_pButtons[0]->SetPressedFunction([]()
		{
			const auto pSceneManager = SceneManager::Get();
			pSceneManager->AddGameScene(new PyreScene());
			pSceneManager->SetActiveGameScene(L"PyreScene");
		});
	m_pButtons[0]->SetSelected(true);

	m_pButtons.emplace_back(AddChild(new Button(L"ABOUT", XMFLOAT2{ m_SceneContext.windowWidth / 2 - 60, m_SceneContext.windowHeight / 2 - 30 }, XMFLOAT4{ Colors::White }, Select)));
	m_pButtons[1]->SetPressedFunction([]()
		{
			//Go to about scene
		});

	m_pButtons.emplace_back(AddChild(new Button(L"QUIT", XMFLOAT2{ m_SceneContext.windowWidth / 2 - 45, m_SceneContext.windowHeight / 2 + 40 }, XMFLOAT4{ Colors::White }, Select)));
	m_pButtons[2]->SetPressedFunction([]()
		{
			PostQuitMessage(0);
		});
}
