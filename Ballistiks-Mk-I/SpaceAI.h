#pragma once
#include "AI.h"

class SpaceAI : public AI
{
public:
	SpaceAI(ControllablePlayer& player, const ViewableWorld& view)
		:
		AI(player, view)
	{
		goalLeft = Vec2(80.0f, 360.0f);
		goalRight = Vec2(1200.0f, 360.0f);
	}
protected:
	Vec2 goalLeft;
	Vec2 goalRight;
	virtual void _Process() override
	{
		const Player& me = player.PlayerConst();
		const Player& other = [me, this]() -> const Player&
		{
			for (const Player& p : view.GetPlayers())
			{
				if (p != me)
				{
					return p;
				}
			}
			// should not ever see this... unless <= 1 player in game :?
			assert(false);
			return me;
		}();
		Vec2 ballPos = view.GetBalls().front().GetCenter();
		Vec2 ballToOpp = ((ballPos - other.GetCenter()));
		Vec2 vecToBall =  ballPos - me.GetCenter();
		Vec2 attackOpp = other.GetCenter() - me.GetCenter();
		Vec2 pointOfAttack = ballPos + ballToOpp.Normalize() * (me.GetRadius() + view.GetBalls().front().GetRadius());
		Vec2 dirPointOfAttack = pointOfAttack - me.GetCenter();
		Vec2 dirGoalRight = goalRight - me.GetCenter();
		static Vec2 dirDown = Vec2(0, 1).Normalize();
		static Vec2 dirUp = Vec2(0, -1).Normalize();
		Vec2 dirDefendBottm = ballPos + (dirDown * me.GetRadius()*1.5) - me.GetCenter();
		Vec2 dirDefendTop = ballPos + (dirUp * me.GetRadius()*1.5) - me.GetCenter();

		player.SetThrustVector(dirPointOfAttack.Normalize());
		static float playerBallRad = (me.GetRadius() + view.GetBalls().front().GetRadius());
		static float ballRad = view.GetBalls().front().GetRadius();
		if (vecToBall.Len() < playerBallRad * 2 + ballRad * 2 && vecToBall.x > Vec2(0,0).x)
		{
			/*if (me.GetCenter().x < ballPos.x)
			{
				player.SetThrustVector(dirGoalRight.Normalize());
			}*/
			if (me.GetCenter().y < ballPos.y)
			{
				player.SetThrustVector(dirDefendBottm.Normalize());
			}
			else
			{
				player.SetThrustVector(dirDefendTop.Normalize());
			}
			player.Burst();
		}
		if (/*vecToBall.Len() < playerBallRad * 2 + ballRad * 2 && */vecToBall.x < Vec2(0, 0).x)
		{
			
			if (me.GetCenter().y > ballPos.y)
			{
				player.SetThrustVector(dirDefendBottm.Normalize());
			}
			else
			{
				player.SetThrustVector(dirDefendTop.Normalize());
			}
			player.Burst();
		}
	}
};

class SpaceAIFactory : public Controller::Factory
{
public:
	SpaceAIFactory()
		:
		Factory(L"Space")
	{}
	virtual std::unique_ptr< Controller > Make(ControllablePlayer& player, const ViewableWorld& view) override
	{
		return std::make_unique< SpaceAI >(player, view);
	}
};