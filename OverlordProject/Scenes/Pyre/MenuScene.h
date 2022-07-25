#pragma once
class DiffuseMaterial_Shadow;

class MenuScene final : public GameScene
{
public:
	MenuScene();
	~MenuScene() override = default;

	MenuScene(const MenuScene& other) = delete;
	MenuScene(MenuScene&& other) noexcept = delete;
	MenuScene& operator=(const MenuScene& other) = delete;
	MenuScene& operator=(MenuScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	DiffuseMaterial_Shadow* m_pBookMaterial;
	GameObject* m_pLeftPileOfBooks, * m_pRightPileOfBooks;
	GameObject* m_pBackground;
};
