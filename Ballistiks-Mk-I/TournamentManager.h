#pragma once

#include <map>
#include <iostream>
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
		// slightly buggy in places!
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
			return !( *this > rhs );
		}
		inline bool operator>=( const TournamentStats& rhs ) const
		{
			return *this > rhs || *this == rhs;
		}
		inline bool operator<=( const TournamentStats& rhs ) const
		{
			return *this < rhs || *this == rhs;
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
			results << L"======================================================" << std::endl
					<< i->GetName() << L" VS " << j->GetName() << std::endl
					<< L"======================================================" << std::endl;
			iWins = 0;
			jWins = 0;
			ties = 0;
			iGoals = 0;
			jGoals = 0;
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

		results << L"\t" << i->GetName() << L": " << aScore << L"\t\t"
				<< j->GetName() << L": " << bScore << std::endl;
		
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
		results << L"======================================================" << std::endl
				<< L"Final Results:" << std::endl
				<< i->GetName() << L": " << iWins << std::endl
				<< j->GetName() << L": " << jWins << std::endl
				<< L"Ties: " << ties << std::endl
				<< L"======================================================" << std::endl
				<< std::endl << std::endl;
		
		auto& iStat = stats[i->GetName()];
		iStat.totalGameWins += iWins;
		iStat.totalGameLosses += jWins;
		iStat.totalGameTies += ties;

		auto& jStat = stats[j->GetName()];
		jStat.totalGameWins += jWins;
		jStat.totalGameLosses += iWins;
		jStat.totalGameTies += ties;

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
private:
	int currentGameIndex = 0;
	const int nTotalGames = 50;
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
};