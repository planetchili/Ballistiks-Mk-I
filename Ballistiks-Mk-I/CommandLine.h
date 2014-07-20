#include <string>
#include <sstream>
#include <iterator>

class CommandLine
{
public:
	class Exception
	{
	public:
		Exception( const std::wstring& cmd )
		{
			if( cmd == L"" )
			{
				msg = L"You didn't type any command line arguments. Fucker.\n\nSeriously, are you fucking kidding me? Jesus.";
			}
			else
			{
				msg = L"In the command line you typed the following arguments:\n\n   [ " + cmd + L" ]"
					+ L"\n\nThat was really fucking stupid of you. Don't do that again. Ever.\n   ...fucker.";
			}
		}
		operator const wchar_t*() const
		{
			return msg.c_str();
		}
	private:
		std::wstring msg;
	};
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
		try
		{
			std::wistringstream str( cmd );
			std::wstring tok = L"";
			str >> tok;

			if( tok == L"fs" )
			{
				tok = L"";
				str >> tok;
				frameskip = std::stoul( tok );
				tok = L"";
				str >> tok;
			}

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
				tok = L"";
				str >> tok;
				player0 = tok;
				tok = L"";
				str >> tok;
				player1 = tok;
				tok = L"";
				str >> tok;
				seed = tok == L"" ? rand() : std::stoul( tok );
				break;
			case L'P':
			case L'p':
				type = Play;
				tok = L"";
				str >> tok;
				player0 = tok;
				tok = L"";
				str >> tok;
				seed = tok == L"" ? rand() : std::stoul( tok );
				break;
			default:
				throw Exception( cmd );
			}
		}
		catch( std::invalid_argument )
		{
			throw Exception( cmd );
		}
		catch( std::out_of_range )
		{
			throw Exception( cmd );
		}
	}

public:
	Type type = Null;
	unsigned int seed = 0;
	std::wstring player0;
	std::wstring player1;
	unsigned int frameskip = 0;
};