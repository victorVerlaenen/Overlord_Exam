#pragma once
class Vignette;
class Pedestal;
class Team;
class Orb;
class Player;

class PyreScene : public GameScene
{
public:
	PyreScene() ;
	~PyreScene() override = default;
	PyreScene(const PyreScene& other) = delete;
	PyreScene(PyreScene&& other) noexcept = delete;
	PyreScene& operator=(const PyreScene& other) = delete;
	PyreScene& operator=(PyreScene&& other) noexcept = delete;

	void GoalReset(Player* pScoredPlayer = nullptr) const;

protected:
	void Initialize() override;
	void OnGUI() override;
	void Update() override;
	void Draw() override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;
private:
	enum InputIds
	{
		Player01Jump,
		Player01PassNext,
		Player01PassPrevious,
		Player02Jump,
		Player02PassNext,
		Player02PassPrevious,
		Pause
	};

	void AddTeams();
	void AddInputs() const;
	void AddLevel();
	void AddControls();

	void UpdateCamera();

	std::vector<XMFLOAT3> m_StartingPositions{	{-10.f, 1.5f, 0.f},
												{-12.f, 1.5f, 5.f},
												{-12.f, 1.5f, -5.f} };
	Orb* m_pOrb{};
	Team* m_pTeam_01{}, * m_pTeam_02{};
	FixedCamera* m_pCamera{};
	Vignette* m_pVignette{};
	Pedestal* m_pPedestal{};
	XMVECTOR m_CurrentCameraRotation{};
	SpriteFont* m_pControlsFont{};

	XMFLOAT3 m_ControlsPosition{ };

	FMOD::Sound* m_pGameTrack{};
	FMOD::Channel* m_pChannel2D{};
	//float m_ShadowMapScale{ 0.3f };
	//bool m_DrawShadowMap{ false };
	//GameObject* m_pSmallRock;
	//XMFLOAT3 position = { 0, 0, 0 };
	//XMFLOAT3 rotation = { 0, 0, 0 };
	//XMFLOAT3 scale = { 1, 1, 1 };
};

