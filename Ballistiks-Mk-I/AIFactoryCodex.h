#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "AI.h"

/*************************/
/*  User AI Headers      */

#include "TestAI.h"
#include "AssholeAI.h"
#include "ChiliBot.h"

/*************************/

class AIFactoryCodex
{
public:
	AIFactoryCodex()
	{
		/**********************************/
		/*  User AI Factory Registration  */

		Register< TestAIFactory >();
		Register< AssholeAIFactory >();

		/**********************************/
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
	template <typename T>
	void Register()
	{
		factories.push_back( std::make_unique< T >() );
	}
private:
	std::vector< std::unique_ptr< AI::Factory > > factories;
};