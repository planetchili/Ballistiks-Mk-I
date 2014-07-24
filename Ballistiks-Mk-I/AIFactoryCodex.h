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
#include "ChiliBot.h"
#include "LuXAI.h"
#include "CaAI.h"
#include "SpaceAI.h"

/*************************/

class AIFactoryCodex
{
#pragma region Custom Iterator Container Interface
public:
	class iterator
	{
	public:
		typedef iterator self_type;
		typedef Controller::Factory value_type;
		typedef Controller::Factory& reference;
		typedef Controller::Factory* pointer;
		typedef std::vector< std::unique_ptr< Controller::Factory > >::iterator internal_iterator;
		typedef std::forward_iterator_tag iterator_category;
		typedef int difference_type;
		inline iterator( internal_iterator ptr ) : ptr_( ptr ) { }
		inline self_type operator++( ) {
			self_type i = *this; ptr_++; return i;
		}
		inline self_type operator++( int junk ) {
			ptr_++; return *this;
		}
		inline reference operator*( ) {
			return **ptr_;
		}
		inline pointer operator->( ) {
			return ptr_->operator->();
		}
		inline bool operator==( const self_type& rhs ) {
			return ptr_ == rhs.ptr_;
		}
		inline bool operator!=( const self_type& rhs ) {
			return ptr_ != rhs.ptr_;
		}
	private:
		internal_iterator ptr_;
	};
	class const_iterator
	{
	public:
		typedef const_iterator self_type;
		typedef Controller::Factory value_type;
		typedef Controller::Factory& reference;
		typedef Controller::Factory* pointer;
		typedef std::vector< std::unique_ptr< Controller::Factory > >::const_iterator internal_iterator;
		typedef int difference_type;
		typedef std::forward_iterator_tag iterator_category;
		inline const_iterator( internal_iterator ptr ) : ptr_( ptr ) { }
		inline self_type operator++( ) {
			self_type i = *this; ptr_++; return i;
		}
		inline self_type operator++( int junk ) {
			ptr_++; return *this;
		}
		inline const reference operator*( ) {
			return **ptr_;
		}
		inline const pointer operator->( ) {
			return ptr_->operator->();
		}
		inline bool operator==( const self_type& rhs ) {
			return ptr_ == rhs.ptr_;
		}
		inline bool operator!=( const self_type& rhs ) {
			return ptr_ != rhs.ptr_;
		}
	private:
		internal_iterator ptr_;
	};
	inline int size() const {
		return factories.size();
	}
	inline iterator begin()
	{
		return iterator( factories.begin() );
	}
	inline iterator end()
	{
		return iterator( factories.end() );
	}
	inline const_iterator begin() const
	{
		return cbegin();
	}
	inline const_iterator end() const
	{
		return cend();
	}
	inline const_iterator cbegin() const
	{
		return const_iterator( factories.cbegin() );
	}
	inline const_iterator cend() const
	{
		return const_iterator( factories.cend() );
	}
#pragma endregion
public:
	AIFactoryCodex()
	{
		/**********************************/
		/*  User AI Factory Registration  */

		//Register< TestAIFactory >();
		//Register< AssholeAIFactory >();
		Register< ChiliBotFactory >();
		Register< ChiliBotControlFactory >();
		Register< LuXAIFactory >( );
		//Register< CaAIFactory >();
		//Register< SpaceAIFactory >();

		/**********************************/
	}
	Controller::Factory& GetRandomFactory()
	{
		return *factories[rand() % factories.size()];
	}
	Controller::Factory& GetFactoryByName( const std::wstring author )
	{
		return **std::find_if( factories.begin(),factories.end(),
			[&author]( const std::unique_ptr< Controller::Factory >& f ) -> bool
		{
			return f->GetName() == author;
		} );
	}
	bool ContainsAuthor( const std::wstring author ) const
	{

		return 0 < std::count_if( factories.begin(),factories.end(),
			[&author]( const std::unique_ptr< Controller::Factory >& f ) -> bool
		{
			return f->GetName() == author;
		} );
	}
private:
	template <typename T>
	void Register()
	{
		std::unique_ptr< Controller::Factory > pFactory = std::make_unique< T >();

		if( !ContainsAuthor( pFactory->GetName() ) )
		{
			factories.push_back( std::move( pFactory ) );
		}
		else
		{
			assert( false );
		}
	}
private:
	std::vector< std::unique_ptr< Controller::Factory > > factories;
};