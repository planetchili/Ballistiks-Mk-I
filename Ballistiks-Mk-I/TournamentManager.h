#pragma once

#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "AIFactoryCodex.h"
#include "Presentator.h"

class TournamentManager
{
private:
	class TournamentStats
	{
	public:
		void WriteStats( std::wofstream& res ) const
		{
			res << L"Match results (W/L/T): " << matchWins << L"/" << matchLosses << L"/" << matchTies << std::endl
				<< L"Total results (W/L/T): " << totalGameWins << L"/" << totalGameLosses << L"/" << totalGameTies << std::endl
				<< L"Total scored: " << totalPointsScored << L"  Total Allowed: " << totalPointsAllowed << std::endl;
		}
	public:
		inline bool operator>( const TournamentStats& rhs ) const
		{
			if( matchWins > rhs.matchWins )
			{
				return true;
			}
			else if( matchWins < rhs.matchWins )
			{
				return false;
			}
			else
			{
				return matchLosses < rhs.matchLosses;
			}
		}
		inline bool operator==( const TournamentStats& rhs ) const
		{
			return matchWins == rhs.matchWins && matchTies == rhs.matchTies;
		}
		inline bool operator<( const TournamentStats& rhs ) const
		{
			return !( *this > rhs ) && !(*this == rhs );
		}
		inline bool operator>=( const TournamentStats& rhs ) const
		{
			return *this > rhs || *this == rhs;
		}
		inline bool operator<=( const TournamentStats& rhs ) const
		{
			return !( *this > rhs );
		}
		inline bool operator!=( const TournamentStats& rhs ) const
		{
			return !(*this == rhs);
		}
	public:
		int matchWins = 0;
		int matchLosses = 0;
		int matchTies = 0;
		int totalGameWins = 0;
		int totalGameLosses = 0;
		int totalGameTies = 0;
		int totalPointsScored = 0;
		int totalPointsAllowed = 0;
	};
public:
	TournamentManager( AIFactoryCodex& codex,Presentator& pres )
		:
		TournamentManager( codex,pres,rand() )
	{}
	TournamentManager( AIFactoryCodex& codex,Presentator& pres,unsigned int masterSeed )
		:
		codex( codex ),
		pres( pres ),
		i(codex.begin()),
		j(codex.begin())
	{
		results.open( L"results.txt",std::ofstream::out );
		for( const Controller::Factory& f : codex )
		{
			stats.insert( std::pair<std::wstring,TournamentStats>(f.GetName(),TournamentStats()) );
		}

		const int competitors = codex.size();
		nMatches = ( competitors * ( competitors - 1 ) ) / 2;
		srand( masterSeed );
		for( int i = 0; i < nMatches; i++ )
		{
			matchSeeds.push_back( rand() );
		}
		for( unsigned int s : matchSeeds )
		{
			std::vector< unsigned int > gameSeedsTemp;
			srand( s );
			for( int i = 0; i < nTotalGames; i++ )
			{
				gameSeedsTemp.push_back( rand() );
			}
			gameSeeds.push_back( std::move( gameSeedsTemp ) );
		}

		results << L"=============================================================" << std::endl
				<< L"  Master Seed: [ " << masterSeed << L" ]" << std::endl
				<< L"=============================================================" << std::endl << std::endl;
	}
	~TournamentManager()
	{
		results.close();
	}
	bool StartNextMatch()
	{
		j++;
		if( j != codex.end() )
		{
			currentMatchIndex++;
			results << L"=============================================================" << std::endl
					<< i->GetName() << L" VS " << j->GetName() << std::endl
					<< L"  Match Seed: [ " << matchSeeds[ currentMatchIndex] << L" ]" << std::endl
					<< L"=============================================================" << std::endl;
			iWins = 0;
			jWins = 0;
			ties = 0;
			iGoals = 0;
			jGoals = 0;
			currentGameIndex = 0;
			return true;
		}
		else
		{
			i++;
			if( i != codex.end() )
			{
				j = i;
				return StartNextMatch();
			}
			else
			{
				return false;
			}
		}
	}
	bool StartNextGame()
	{
		if( currentGameIndex < nTotalGames )
		{
			srand( gameSeeds[currentMatchIndex][currentGameIndex] );
			currentGameIndex++;
			pres.StartSimulation( *i,*j );
			return true;
		}
		else
		{
			return false;
		}
	}
	void RecordGameResults()
	{
		const unsigned int aScore = pres.GetManager().GetTeamA().GetScore();
		const unsigned int bScore = pres.GetManager().GetTeamB().GetScore();

		results << L"\t" << i->GetName() << L": " << aScore << L"\t"
				<< j->GetName() << L": " << bScore 
				<< L"\t[ " << std::setfill( L' ' ) << std::setw( 5 )
				<< gameSeeds[currentMatchIndex][currentGameIndex - 1] << L" ]" << std::endl;
		
		iGoals += aScore;
		jGoals += bScore;

		if( aScore > bScore )
		{
			iWins++;
		}
		else if( bScore > aScore )
		{
			jWins++;
		}
		else
		{
			ties++;
		}
	}
	void RecordMatchResults()
	{
		results << L"=============================================================" << std::endl
				<< L"Final Results:" << std::endl
				<< i->GetName() << L": " << iWins << std::endl
				<< j->GetName() << L": " << jWins << std::endl
				<< L"Ties: " << ties << std::endl
				<< L"=============================================================" << std::endl
				<< std::endl << std::endl;
		
		auto& iStat = stats[i->GetName()];
		iStat.totalGameWins += iWins;
		iStat.totalGameLosses += jWins;
		iStat.totalGameTies += ties;
		iStat.totalPointsScored += iGoals;
		iStat.totalPointsAllowed += jGoals;

		auto& jStat = stats[j->GetName()];
		jStat.totalGameWins += jWins;
		jStat.totalGameLosses += iWins;
		jStat.totalGameTies += ties;
		jStat.totalPointsScored += jGoals;
		jStat.totalPointsAllowed += iGoals;

		if( iWins > jWins )
		{
			iStat.matchWins++;
			jStat.matchLosses++;
		}
		else if( jWins > iWins )
		{
			jStat.matchWins++;
			iStat.matchLosses++;
		}
		else
		{
			iStat.matchTies++;
			jStat.matchTies++;
		}
	}
	void RecordTotalResults()
	{
		std::multiset<std::pair<TournamentStats,const std::wstring>,
			std::greater<std::pair<TournamentStats,const std::wstring>>> sortedStats;
		for( auto& s : stats )
		{
			sortedStats.insert( { s.second,s.first } );
		}
		int rank = 1;
		for( auto& s : sortedStats )
		{
			results << L"=============================================================" << std::endl
					<< L"             #" << rank << L" " << s.second << std::endl
					<< L"=============================================================" << std::endl;
			s.first.WriteStats( results );
			results << std::endl;
			rank++;
		}
	}
	float GetCompletionPercent() const
	{
		return (float( currentMatchIndex * GAMES_PER_MATCH + currentGameIndex ) 
			/ float( GAMES_PER_MATCH * nMatches )) * 100.0f;
	}
private:
	int currentMatchIndex = -1;
	int nMatches;
	int currentGameIndex = 0;
	const int nTotalGames = GAMES_PER_MATCH;
	int iWins = 0;
	int jWins = 0;
	int ties = 0;
	int iGoals = 0;
	int jGoals = 0;
	AIFactoryCodex& codex;
	Presentator& pres;
	AIFactoryCodex::iterator i;
	AIFactoryCodex::iterator j;
	std::map< std::wstring,TournamentStats > stats;
	std::wofstream results;
	unsigned int masterSeed;
	std::vector< unsigned int > matchSeeds;
	std::vector< std::vector< unsigned int > > gameSeeds;
};