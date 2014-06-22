#pragma once
#include <vector>
#include <memory>
#include "AI.h"

/********************************/
/*  User AI Headers             */

#include "TestAI.h"
#include "AssholeAI.h"

/********************************/

class AIFactoryCodex
{
public:
	AIFactoryCodex()
	{
		/********************************/
		/*  User AI Factory Pushes      */

		factories.push_back( std::make_unique< TestAIFactory >() );
		factories.push_back( std::make_unique< AssholeAIFactory >() );

		/********************************/
	}
	AI::Factory& GetRandomFactory()
	{
		return *factories[rand() % factories.size()];
	}
private:
	std::vector< std::unique_ptr< AI::Factory > > factories;
};