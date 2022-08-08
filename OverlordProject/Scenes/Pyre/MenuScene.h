#pragma once
class Button;
class DiffuseMaterial_Shadow;

class MenuScene : public GameScene
{
public:
	MenuScene(bool isPause = false);
	~MenuScene() override = default;

	MenuScene(const MenuScene& other) = delete;
	MenuScene(MenuScene&& other) noexcept = delete;
	MenuScene& operator=(const MenuScene& other) = delete;
	MenuScene& operator=(MenuScene&& other) noexcept = delete;

protected:
	enum InputIds
	{
		Select,
	};

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;
private:

	void AddMainMenuButtons();
	void AddPauseButtons();

	std::vector<Button*> m_pButtons{};
	DiffuseMaterial_Shadow* m_pBookMaterial;
	GameObject* m_pLeftPileOfBooks, * m_pRightPileOfBooks;
	GameObject* m_pBackground;
	GameObject* m_pSkyBox;
	
	bool m_Navigated{ false };
	float m_NavigationTimer{ 0 };
	const float m_NavigationCooldown{ 0.2f };

	FMOD::Sound* m_pMenuTrack{};
	FMOD::Channel* m_pChannel2D{};

	bool m_IsPause{};
};
