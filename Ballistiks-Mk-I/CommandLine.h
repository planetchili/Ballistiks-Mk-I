#include <string>
#include <sstream>
#include <iterator>

class CommandLine
{
public:
	enum Type
	{
		Simulate,
		Watch,
		Play,
		Null
	};
public:
	CommandLine( const std::wstring& cmd )
	{
		std::wistringstream str( cmd );
		std::wstring tok;
		str >> tok;
		switch( tok[0] )
		{
		case L'S':
		case L's':
			type = Simulate;
			tok = L"";
			str >> tok;
			seed = tok == L"" ? rand() : std::stoul( tok );
			break;
		case L'W':
		case L'w':
			type = Watch;
			str >> tok;
			player0 = tok;
			str >> tok;
			player1 = tok;
			tok = L"";
			str >> tok;
			seed = tok == L"" ? rand() : std::stoul( tok );
			break;
		case L'P':
		case L'p':
			type = Play;
			str >> tok;
			player0 = tok;
			tok = L"";
			str >> tok;
			seed = tok == L"" ? rand() : std::stoul( tok );
			break;
		default:
			throw;
		}
	}

public:
	Type type = Null;
	unsigned int seed = 0;
	std::wstring player0;
	std::wstring player1;
};