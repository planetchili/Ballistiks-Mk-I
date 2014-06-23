#pragma once
#include <vector>
#include <memory>
#include <algorithm>
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
	AI::Factory& GetFactoryByAuthor( const std::string author )
	{
		return **std::find_if( factories.begin(),factories.end(),
			[&author]( const std::unique_ptr< AI::Factory >& f ) -> bool
		{
			return f->GetAuthor() == author;
		} );
	}
private:
	std::vector< std::unique_ptr< AI::Factory > > factories;
};