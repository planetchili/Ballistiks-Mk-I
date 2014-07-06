#pragma once
#include <stdlib.h>
#include <time.h>

class Randomizer
{
public:
	Randomizer()
	{
		srand( (unsigned int)time( nullptr ) );
	}
};