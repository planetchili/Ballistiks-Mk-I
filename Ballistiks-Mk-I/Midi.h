#pragma once
#include <string>
#include <set>
#include <mutex>
#include <condition_variable>

class MidiSong
{
public:
	MidiSong( const std::wstring path,float start = 0.0f,float end = -1.0f );
	void Play();
	void Stop();
	~MidiSong();
private:
	std::wstring GetAlias() const;
	static unsigned int ReserveID();
private:
	static unsigned int nextID;
	static std::set< unsigned int > usedIDs;
private:
	std::mutex mutex;
	std::condition_variable cv;
	const unsigned int id;
	bool isPlaying;
	bool isDying;
	unsigned int startMilliSeconds;
	unsigned int endMilliSeconds;
};