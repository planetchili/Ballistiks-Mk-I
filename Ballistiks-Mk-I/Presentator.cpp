#include "Presentator.h"

/*
void Presentator::BeginTitleState::OnCountdownFinished()
{
	parent.Transition( std::make_unique< Presentator::GameOn >( parent ) );
}*/

void Presentator::GameOn::OnGoal()
{
	parent.manager->StartNewPoint();
	parent.Transition( std::make_unique< GameOn >( parent ) );
}

void Presentator::GameOn::OnPeriod()
{
	parent.manager->StartNewPeriod();
	parent.Transition( std::make_unique< GameOn >( parent ) );
}

void Presentator::GameOn::Draw( DrawTarget& tgt )
{
	parent.manager->RenderWorld( tgt );
}