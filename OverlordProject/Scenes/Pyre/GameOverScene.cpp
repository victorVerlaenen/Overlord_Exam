#include "stdafx.h"
#include "GameOverScene.h"

#include "MenuScene.h"
#include "Prefabs/Skybox.h"

GameOverScene::GameOverScene(const std::wstring& winnerText)
	:GameScene(L"GameOverScene")
	, m_Text(winnerText)
	, m_pFont(ContentManager::Load<SpriteFont>(L"SpriteFonts/Nightwarrior_64.fnt"))
{

}

void GameOverScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = false;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;

	//Sound
	SoundManager::Get()->GetSystem()->createStream("Resources/Audio/MenuTrack.mp3", FMOD_LOOP_NORMAL, nullptr, &m_pGameOverTrack);
	SoundManager::Get()->GetSystem()->playSound(m_pGameOverTrack, nullptr, true, &m_pChannel2D);
	m_pChannel2D->setVolume(.1f);

	m_pSkyBox = AddChild(new Skybox());

	//button
	const auto pSelectButtonSprite = AddChild(new GameObject());
	pSelectButtonSprite->AddComponent(new SpriteComponent(L"Textures/MenuScene/SelectButton.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));

	pSelectButtonSprite->GetTransform()->Translate(m_SceneContext.windowWidth / 2 -50, m_SceneContext.windowHeight - 50, 0);

	//Camera
	const auto pFixedCamera = AddChild(new FixedCamera());
	pFixedCamera->GetTransform()->Translate(0, 0, 0);
	pFixedCamera->GetComponent<CameraComponent>()->SetActive(true);

	//input
	auto inputAction = InputAction(GoToMenu, InputState::pressed, -1, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);

}

void GameOverScene::Update()
{
	//Rotate sky
	const auto time = SceneManager::Get()->GetActiveScene()->GetSceneContext().pGameTime->GetTotal();
	m_pSkyBox->GetComponent<ModelComponent>()->GetTransform()->Rotate(XMFLOAT3{ 0,1.f * time ,0 }, true);

	if (m_SceneContext.pInput->IsActionTriggered(GoToMenu))
	{
		const auto pSceneManager = SceneManager::Get();
		pSceneManager->AddGameScene(new MenuScene());
		pSceneManager->SetActiveGameScene(L"MenuScene");
	}
}

void GameOverScene::Draw()
{
	TextRenderer::Get()->DrawText(ContentManager::Load<SpriteFont>(L"SpriteFonts/Nightwarrior_32.fnt"),
		L"Main Menu",
		{ m_SceneContext.windowWidth / 2 -20, m_SceneContext.windowHeight - 66 },
		XMFLOAT4{ Colors::White });

	TextRenderer::Get()->DrawText(m_pFont, m_Text, { m_SceneContext.windowWidth / 2 - 120, m_SceneContext.windowHeight / 2-50 }, XMFLOAT4{ 1.f, 0.604f, 0.078f, 1.f });
}

void GameOverScene::OnSceneActivated()
{
	m_pChannel2D->setPaused(false);

	const auto pSceneManager = SceneManager::Get();
	const auto pPyreScene = pSceneManager->GetScene(L"PyreScene");
	pSceneManager->RemoveGameScene(pPyreScene, true);
}

void GameOverScene::OnSceneDeactivated()
{
	m_pChannel2D->setPaused(true);

	const auto pSceneManager = SceneManager::Get();
	const auto pThisScene = static_cast<GameScene*>(this);
	pSceneManager->RemoveGameScene(pThisScene, true);
}
