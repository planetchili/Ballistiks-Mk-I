#pragma once
#include "AI.h"

class ShipHandler
{
public:
	ShipHandler(ControllablePlayer& player, const ViewableWorld& view)
		:
		player(player),
		view(view)
	{

	}
	~ShipHandler()
	{

	}
	void GoToCheckPoints(std::vector< Vec2 >& points)
	{
		bool b = false;
		auto a = points.begin();
		do
		{
			GoToPoint(*a);
			if (b = (HasReachedPoint(*a, 30)))
			{
				++a;
			}
		} while (b && a != points.end());
	}
	bool HasReachedPoint(const Vec2& point, const float leeWay)
	{
		Vec2 pos = player.PlayerConst().GetCenter();
		if (pos.x + leeWay > point.x && pos.x - leeWay < point.x)
		{
			if (pos.y + leeWay > point.y && pos.y - leeWay < point.y)
			{
				return true;
			}
		}
		return false;
	}
	void GoToPoint(const Vec2& point)
	{
		const Vec2 pos = player.PlayerConst().GetCenter();
		Vec2 curVel = player.PlayerConst().GetVel();
		Vec2 dV = point - pos;
		dV.Normalize();

		if (curVel.x == 0.0f && curVel.y == 0.0f)
			curVel = dV;
		else
			curVel.Normalize();

		Vec2 nV = { 0.0f,0.0f };

		if (sgn(curVel.x) == sgn(dV.x))
			nV.x = dV.x;
		else
			nV.x = -curVel.x + dV.x;

		if (sgn(curVel.y) == sgn(dV.y))
			nV.y = dV.y;
		else
			nV.y = -curVel.y + dV.y;
		
		player.SetThrustVector(nV);

		/*const Vec2 factor = { curVel.x / dV.x, curVel.y / dV.y };*/
		/*if (curVel.x > 0.0f && dV.x > 0.0f)
		{
			if (curVel.x > dV.x)
			{
				nV.x = curVel.x - dV.x;
			}
			else
			{
				nV.x = dV.x - curVel.x;
			}
		}
		else
		{
			nV.x = dV.x;
		}
		if (curVel.y > 0.0f && dV.y > 0.0f)
		{
			if (curVel.y > dV.y)
			{
				nV.y = curVel.y - dV.y;
			}
			else
			{
				nV.y = dV.y - curVel.y;
			}
		}
		else
		{
			nV.y = dV.y;
		}
		player.SetThrustVector(nV);*/

	}
		
	Vec2 GetDP(const Vec2& pos, const Vec2& ballPos, const float& ballRad, float distanceMultiplier, const Vec2& point)
	{
		Vec2 slope = point - pos;
		slope.Normalize();
		return Vec2(ballPos.x - (slope.x * ballRad * distanceMultiplier), ballPos.y - (slope.y * ballRad * distanceMultiplier));
	}
	float GetObjectRange(const Vec2& obj1, const Vec2& obj2)
	{
		const float xDist = sq(obj2.x - obj1.x);
		const float yDist = sq(obj2.y - obj1.y);
		return sqrt(xDist + yDist);
	}
	template <typename V1, typename V2, typename R > bool IsWithinRange(V1 v1, V2 v2, R r)
	{
		return v1 - r <= v2 && v1 + r >= v2 && v2 - r <= v1 && v2 + r >= v1;
	}
	bool HasClearShot(const Vec2& pos, const Vec2& ballPos, const float& ballRad, const Vec2& DP, const Vec2& goal)
	{
		Vec2 slope1 = goal - pos;//slope from you to goal
		slope1.Normalize();
		Vec2 slope2 = DP - pos;// solpe from you to DP
		slope2.Normalize();
		return IsWithinRange(slope1.x, slope2.x, ballRad / 100.0f) && IsWithinRange(slope1.y, slope2.y, ballRad / 100.0f);
	}
	void Stop()
	{
		player.SetThrustVector(Vec2(0.0f, 0.0f));
	}
	void Rotate(float radians)
	{
		Vec2 pos = player.PlayerConst().GetCenter();
		float radius = player.PlayerConst().GetRadius();
		GoToPoint(pos.Rotation(radians));	
	}
	//Loc GetDPLoc( const Vec2& DP, const Vec2& ballPos, const float ballRad )
	//{
	//	Loc loc;
	//	if (DP.x + ballRad < ballPos.x)
	//		loc.x = Left;
	//	else if (DP.x - ballRad > ballPos.x)
	//		loc.x = Right;
	//	else
	//		loc.x = None;
	//	if (DP.y + ballRad < ballPos.y)
	//		loc.y = Top;
	//	else if (DP.y - ballRad > ballPos.y)
	//		loc.y = Bottom;
	//	else
	//		loc.y = None;
	//}
	bool InFrontOfPuck(const Vec2& pos, const float& rad, const Vec2& ballPos, const float& ballRad, char player)
	{
		if ( player == 1 )
			return (ballPos.x - ballRad) + 0.5f >= (pos.x + rad) + 0.5f;
		else if ( player == -1 )
			return (ballPos.x + ballRad) + 0.5f <= (pos.x - rad) + 0.5f;
	}
	void PathToPoint(const Vec2& pos, const float& rad, const Vec2& ballPos, const Vec2& ballVel, const float& ballRad, const Vec2& point, char player )
	{
		const Vec2 DP = GetDP(pos, ballPos, ballRad, 3.0f, point);//increasing the distance seems to improve accuracy due to you pushing ball around
		if (!InFrontOfPuck(pos, rad, ballPos, ballRad, player))
		{
			if (DP.y > ballPos.y)//bottom
			{
				Vec2 vel = { 0.0f, 0.0f };
				float temp = 0;
				if (pos.y > DP.y)
					temp = (pos.y - DP.y) - (rad + ballVel.y);
				else
					temp = (DP.y - pos.y) - (rad + ballVel.y);

				if (temp <= -ballRad)//you are not down far enough
					GoToPoint(vel = (Vec2(pos.x, DP.y - (temp))));
				else
					GoToPoint(vel = Vec2(DP.x - (player * ( rad + ballRad )), pos.y));
				int a = 0;
			}
			else //top
			{
				Vec2 vel = { 0.0f, 0.0f };
				float temp = 0;
				if (pos.y > DP.y)
					temp = (pos.y - DP.y) - (rad + ballVel.y);
				else
					temp = (DP.y - pos.y) - (rad + ballVel.y);

				if (temp <= -ballRad)//you are not up far enough
					GoToPoint(vel = (Vec2(pos.x, DP.y + (temp))));
				else
					GoToPoint(vel = Vec2(DP.x - (player * ( rad + ballRad )), pos.y));
				int a = 0;
			}
	}		
	else
	{
		GoToPoint(DP);
		if (HasClearShot(pos, ballPos, ballRad, DP, point))
			this->player.Burst();
	}
}
private:
	ControllablePlayer& player;
	const ViewableWorld& view;
};

class CaAI : public AI
{
public:
	CaAI(ControllablePlayer& player, const ViewableWorld& view)
		:
		AI(player, view),
		shipH( player, view )
	{}
protected:
	virtual void _Process() override
	{
		Shoot({ 1250.0f, 350.0f });
		/*Launch( view.GetBalls().front().GetCenter() );*/
		/*player.Burst();*/
	}
private:
	ShipHandler shipH;
	Vec2 GetLS(const Vec2& DP, const Vec2& pos)//Get Launch Slope
	{
		return DP - pos;
	}
	Vec2 Get90R(Vec2& vect)//Calculate Opposite Resiprical
	{
		return Vec2(-vect.y, vect.x);
	}
	void Shoot(Vec2 point)
	{
		const Player& p = player.PlayerConst();
		const Vec2 pos = p.GetCenter();
		const Vec2 vel = p.GetVel();
		const float rad = p.GetRadius();

		const Ball& ball = view.GetBalls().front();
		const Vec2 ballPos = ball.GetCenter();
		const Vec2 ballVel = ball.GetVel();
		const float ballRad = ball.GetRadius();
		shipH.PathToPoint(pos, rad, ballPos, ballVel, ballRad, point, 1);
	}
	Vec2 GetLongestDist(const Vec2& p1, const Vec2& p2)// puck, disk
	{
		float xDist = abs( p2.x - p1.x );
		float yDist = abs( p2.y - p1.y );
		if (xDist > yDist)
			return Vec2(p2.x - p1.x, 0);
		else
			return Vec2(0, p2.y - p1.y);
	}
};

class CaAIFactory : public Controller::Factory
{
public:
	CaAIFactory()
		:
		Factory(L"GeorgeTheJungle")
	{}
	virtual std::unique_ptr< Controller > Make(ControllablePlayer& player, const ViewableWorld& view) override
	{
		return std::make_unique< CaAI >(player, view);
	}
};