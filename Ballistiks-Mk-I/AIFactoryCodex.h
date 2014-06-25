#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <assert.h>
#include "AI.h"

/*************************/
/*  User AI Headers      */

#include "TestAI.h"
#include "AssholeAI.h"

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
	bool ContainsAuthor( const std::string author ) const
	{

		return 0 < std::count_if( factories.begin(),factories.end(),
			[&author]( const std::unique_ptr< AI::Factory >& f ) -> bool
		{
			return f->GetAuthor() == author;
		} );
	}
private:
	template <typename T>
	void Register()
	{
		std::unique_ptr< AI::Factory > pFactory = std::make_unique< T >();

		if( !ContainsAuthor( pFactory->GetAuthor() ) )
		{
			factories.push_back( std::move( pFactory ) );
		}
		else
		{
			assert( false );
		}
	}
private:
	std::vector< std::unique_ptr< AI::Factory > > factories;
};