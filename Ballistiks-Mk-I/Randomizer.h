#pragma once
#include <stdlib.h>
#include <time.h>

class Randomizer
{
public:
	Randomizer()
	{
		srand( time( nullptr ) );
	}
};