#include "stdafx.h"
#include "Team.h"

Team::Team()
{
}

void Team::AddPlayer(Player* pNewPlayer)
{
}

void Team::SetPedestal(Pedestal* pPedestal)
{
}

void Team::PassToNextPlayer()
{
}

void Team::PassToPreviousPlayer()
{
}

void Team::Initialize(const SceneContext& sceneContext)
{
	GameObject::Initialize(sceneContext);
}

void Team::Update(const SceneContext& sceneContext)
{
	GameObject::Update(sceneContext);
}

void Team::Draw(const SceneContext& sceneContext)
{
	GameObject::Draw(sceneContext);
}
