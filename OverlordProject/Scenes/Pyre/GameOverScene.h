
#pragma once
class GameOverScene : public GameScene
{
public:
	GameOverScene(const std::wstring& winnerText);
	~GameOverScene() override = default;

	GameOverScene(const GameOverScene& other) = delete;
	GameOverScene(GameOverScene&& other) noexcept = delete;
	GameOverScene& operator=(const GameOverScene& other) = delete;
	GameOverScene& operator=(GameOverScene&& other) noexcept = delete;

protected:
	enum InputIds
	{
		GoToMenu,
	};

	void Initialize() override;
	void Update() override;
	void Draw() override;
	
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;
private:
	GameObject* m_pSkyBox;

	std::wstring m_Text{};
	SpriteFont* m_pFont{};
	FMOD::Sound* m_pGameOverTrack{};
	FMOD::Channel* m_pChannel2D{};
};
