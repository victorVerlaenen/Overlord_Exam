#pragma once
class Orb;
class Player;

class PyreScene : public GameScene
{
public:
	PyreScene() :GameScene(L"PyreScene") {}
	~PyreScene() override = default;
	PyreScene(const PyreScene& other) = delete;
	PyreScene(PyreScene&& other) noexcept = delete;
	PyreScene& operator=(const PyreScene& other) = delete;
	PyreScene& operator=(PyreScene&& other) noexcept = delete;

	void GoalReset() const;

protected:
	void Initialize() override;
	void OnGUI() override;
	void Update() override;
	void PostDraw() override;
private:
	enum InputIds
	{
		Player01Jump,
		Player01PassNext,
		Player01PassPrevious,
		Player02Jump,
		Player02PassNext,
		Player02PassPrevious
	};
	
	std::vector<XMFLOAT3> m_StartingPositions{	{-10.f, 1.5f, 0.f},
												{-12.f, 1.5f, 5.f},
												{-12.f, 1.5f, -5.f} };
	Orb* m_pOrb{};
	//float m_ShadowMapScale{ 0.3f };
	//bool m_DrawShadowMap{ false };
	//GameObject* m_pSmallRock;
	//XMFLOAT3 position = { 0, 0, 0 };
	//XMFLOAT3 rotation = { 0, 0, 0 };
	//XMFLOAT3 scale = { 1, 1, 1 };
};

