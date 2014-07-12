#include "Presentator.h"


#pragma region GameOnState
void Presentator::GameOnState::Observe()
{
	if( parent.perObs->IsSet() )
	{
		if( parent.manager->GetPeriod() < 1 )
		{
			parent.Transition( std::make_unique<PeriodEndState>( parent ) );
		}
		else
		{
			parent.Transition( std::make_unique<GameEndState>( parent ) );
		}
	}
	else if( parent.ptObs->IsSet() )
	{
		parent.ptObs->Reset();
		parent.Transition( std::make_unique<GoalScoredState>( parent ) );
	}
}

void Presentator::GameOnState::Step( float dt )
{
	parent.manager->Step( dt );
}

void Presentator::GameOnState::Draw( DrawTarget& tgt )
{
	parent.manager->RenderWorld( tgt );
}
#pragma endregion

void Presentator::BeginTitleState::OnCountdownFinished()
{
	parent.Transition( std::make_unique< GameOnState >( parent ) );
}

void Presentator::BeginTitleState::Draw( DrawTarget& tgt )
{
	parent.manager->RenderWorld( tgt );
	tgt.Draw( txtA );
	tgt.Draw( txtB );
	tgt.Draw( vs );
}

void Presentator::GoalScoredState::Draw( DrawTarget& tgt )
{
	parent.manager->RenderWorld( tgt );
	tgt.Draw( scoreText );
}

void Presentator::GoalScoredState::OnCountdownFinished()
{
	parent.manager->StartNewPoint();
	parent.Transition( std::make_unique< GameOnState >( parent ) );
}

void Presentator::GoalScoredState::_Step( float dt )
{
	parent.manager->Step( dt );
}

void Presentator::PeriodEndState::Draw( DrawTarget& tgt )
{
	parent.manager->RenderWorld( tgt );
	tgt.Draw( endText );
}

void Presentator::PeriodEndState::OnCountdownFinished()
{
	parent.manager->StartNewPeriod();
	parent.perObs->Reset();
	parent.Transition( std::make_unique< GameOnState >( parent ) );
}

void Presentator::PeriodEndState::_Step( float dt )
{
	parent.manager->Step( dt );
}

void Presentator::GameEndState::Draw( DrawTarget& tgt )
{
	parent.manager->RenderWorld( tgt );
	tgt.Draw( endText );
}

void Presentator::GameEndState::Step( float dt )
{
	parent.manager->Step( dt );
}