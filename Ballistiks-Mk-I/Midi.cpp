#include "Midi.h"
#include <windows.h>
#include <mmsystem.h>
#include <assert.h>
#include <string>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#pragma comment(lib, "winmm.lib" )

std::set< unsigned int > MidiSong::usedIDs;

MidiSong::MidiSong( const std::wstring path,float start,float end )
	:
	id( ReserveID() ),
	startMilliSeconds( unsigned int( start * 1000.0f ) ),
	isPlaying( false ),
	isDying( false )
{	
	std::thread( [this,path,end]()
	{
		std::unique_lock<std::mutex> conditionLock( mutex );

		const std::wstring seekCmd = std::wstring( L"Seek " ) + GetAlias() +
			std::wstring( L" to " ) + std::to_wstring( startMilliSeconds );
		const std::wstring playCmd = std::wstring( L"Play " ) + GetAlias();
		const std::wstring openCmd = std::wstring( L"Open " ) + path + std::wstring( L" alias " ) + GetAlias();
		mciSendString( openCmd.c_str(),nullptr,0,nullptr );
		const std::wstring setFormatCmd = std::wstring( L"Set " ) + GetAlias() +
			std::wstring( L" time format milliseconds" );
		mciSendString( setFormatCmd.c_str(),nullptr,0,nullptr );
		const std::wstring setSeekCmd = std::wstring( L"Set " ) + GetAlias() +
			std::wstring( L" seek exactly on" );
		mciSendString( setFormatCmd.c_str(),nullptr,0,nullptr );

		if( end == -1.0f )
		{
			const std::wstring getLengthCmd = std::wstring( L"Status " ) + GetAlias() 
				+ std::wstring( L" length" );
			wchar_t lengthString[64];
			mciSendString( getLengthCmd.c_str(),lengthString,64,nullptr );
			endMilliSeconds = std::stoul( std::wstring( lengthString ) );
		}
		else
		{
			endMilliSeconds = unsigned int( end * 1000.0f );
		}

		while( !isDying )
		{
			cv.wait( conditionLock );
			while( isPlaying )
			{
				mciSendString( seekCmd.c_str(),nullptr,0,nullptr );
				mciSendString( playCmd.c_str(),nullptr,0,nullptr );
				cv.wait_for( conditionLock,
					std::chrono::milliseconds( endMilliSeconds - startMilliSeconds ) );
			}
			const std::wstring stopCmd = std::wstring( L"Stop " ) + GetAlias();
			mciSendString( stopCmd.c_str(),nullptr,0,nullptr );
		}
		const std::wstring closeCmd = std::wstring( L"Close " ) + GetAlias();
		mciSendString( closeCmd.c_str(),nullptr,0,nullptr );
	} ).detach();
}

void MidiSong::Play()
{
	if( !isPlaying )
	{
		std::lock_guard<std::mutex> lock( mutex );
		isPlaying = true;
		cv.notify_all();
	}
}

void MidiSong::Stop()
{
	if( isPlaying )
	{
		std::lock_guard<std::mutex> lock( mutex );
		isPlaying = false;
		cv.notify_all();
	}
}

std::wstring MidiSong::GetAlias() const
{
	return std::wstring( L"ChiliMidi" ) + std::to_wstring( id );
}

MidiSong::~MidiSong()
{
	std::lock_guard<std::mutex> lock( mutex );
	usedIDs.erase( id );
	isPlaying = false;
	isDying = true;
	cv.notify_all();
}

unsigned int MidiSong::ReserveID()
{
	for( int n = 0; n < maxSongs; n++ )
	{
		if( usedIDs.count( n ) == 0 )
		{
			usedIDs.insert( n );
			return n;
		}
	}
	assert( false );
	return maxSongs;
}