#include <codecvt>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/program_options.hpp>
#include <boost/program_options/config.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/detail/cmdline.hpp>
#include <boost/program_options/detail/parsers.hpp>

namespace po = boost::program_options;

#include "Base64.hxx"
#include "CommandLinePars.h"

static const int MAX_NAME_LENGTH = 40;
static const int MAX_EMAIL_LENGTH = 254;

using namespace boost;
using namespace std;

static const wstring NAME_FLAG = L"-n";
static const wstring EMAIL_FLAG = L"-e";
static const wstring SESSION_ID_AND_KEY_FLAG = L"-r";
static const wstring BROADCAST_FLAG = L"-b";
static const wstring TOKEN_FLAG = L"-t";
static const wstring SHORT_SHARED_TOKEN_FLAG = L"-st";
static const wstring SHARED_TOKEN_FLAG = L"--shared_token";
static const wstring JOIN_FLAG = L"-j";
static const wstring PERSIST_FLAG = L"-p";
static const wstring SBC_FLAG = L"-sbc";
static const wstring HOST_COMMAND = L"--host";
static const wstring JOIN_COMMAND = L"--join";
static const wstring STUDIO_COMMAND = L"--studio";
static const wstring INVALID_NAME_SYMBOSL = L"%\"&*";
static const wstring EMPTY_LINE;
static const wstring SPACE = L" ";
static const wchar_t COMMAND_MARKER = L'-';
static const wstring UI_FLAG = L"-ui";

#define FSS_STARTMEETING

void 
CCommandLinePars::ParsMeetingID(wstring wstrMeetingID)
{
	_wstrMeetingID = wstrMeetingID;
}


wstring 
CCommandLinePars::ProcessMeetingID()
{
	wstring separator = L"-", wstrMeetingID(_wstrMeetingID);
	int iSeparatorPos = wstrMeetingID.find(separator);
	
	while (iSeparatorPos != wstring::npos)
	{
		wstrMeetingID = wstrMeetingID.replace(iSeparatorPos, 1, EMPTY_LINE);
		iSeparatorPos = wstrMeetingID.find(separator);
	}

	return wstrMeetingID;
}


void 
CCommandLinePars::ParsServerIP(wstring wstrServerID)
{
	_wstrConnectIP = wstrServerID;
	const wregex regex( 
		L"([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})" 
	);
	
	if(!regex_match(_wstrConnectIP, regex)) 
	{
		_wstrConnectIP = L"";
		_bIsParsedComm = false;
		_bIsUpdateInfo = false;		
	}	
}


void 
CCommandLinePars::ParsServerPort(wstring wstrServerPort)
{
	const wregex regex(L"([0-9]{1,5})");
	
	if(!regex_match(wstrServerPort, regex)) 
	{
		_iConnectPort = -1;
		_bIsParsedComm = false;
	}
	else 
	{
        _iConnectPort = boost::lexical_cast<int>(wstrServerPort.c_str());
	}
}


void 
CCommandLinePars::ParsLogin(wstring wstrLogin)
{
	_wstrLogin = wstrLogin;
	const wregex regex(L"[a-zA-Z0-9_\\-\\.@]{1,}");
	
	if(!regex_match(_wstrLogin, regex)) 
	{
		_bIsParsedComm = false;
		_wstrLogin = EMPTY_LINE;
	}
}


void 
CCommandLinePars::ParsPassword(wstring wstrPassword)
{
	_wstrPass = wstrPassword;
	if(_wstrPass.length() == 0) 
	{									
		_bIsParsedComm = false;			
		_wstrPass = EMPTY_LINE;				
	}
}

void CCommandLinePars::ParsSubscriptionId(const std::wstring& wstrSubscriptionId)
{
	_wstrSubscriptionId = wstrSubscriptionId;

	if(_wstrSubscriptionId.length() == 0) 
	{									
		_bIsParsedComm = false;			
		_wstrSubscriptionId = EMPTY_LINE;				
	}
}

void CCommandLinePars::ParsAccessToken(const std::wstring& wstrAccessToken)
{
	_wstrAccessToken = wstrAccessToken;

	if(_wstrAccessToken.length() == 0) 
	{									
		_bIsParsedComm = false;			
		_wstrAccessToken = EMPTY_LINE;				
	}
}

void CCommandLinePars::ParsSharedConfig(const std::wstring& wstrSharedConfigToken)
{
/*
	ConnectionManager::Proxy proxySettings = ConnectionManager::instance().GetProxy();

	if (_bIsQAServer)
	{
		APIV3::SharedConfig::instance().Load(BASE_SERVER_QA, wstrSharedConfigToken, proxySettings.GetProxyURL(), proxySettings.Login, proxySettings.Password); 
	}
	else
	{
		APIV3::SharedConfig::instance().Load(BASE_SERVER, wstrSharedConfigToken, proxySettings.GetProxyURL(), proxySettings.Login, proxySettings.Password); 
	}

	if (APIV3::SharedConfig::instance().ErrorCode() != APIV3::eServerRetcodeNoError)
	{
		_bIsParsedComm = false;
	}
	else
	{
		wstring subscriptionId = CA2W(APIV3::SharedConfig::instance().SubscriptionId().c_str(), CP_UTF8);
		wstring accessToken = CA2W(APIV3::SharedConfig::instance().AccessToken().c_str(), CP_UTF8);
		wstring meetingId = CA2W(APIV3::SharedConfig::instance().MeetingId().c_str(), CP_UTF8);

		if (!subscriptionId.empty() && !accessToken.empty())
		{
			wstring newCmdString = TOKEN_FLAG + L" " + subscriptionId + L" " + accessToken;

			bool rememberMe = APIV3::SharedConfig::instance().RememberMe();

			if (rememberMe)
			{
				newCmdString += (L" " + PERSIST_FLAG);
			}

			ParseCommandLineString(newCmdString);
		}
		else if (!meetingId.empty())
		{
			wstring newCmdString = JOIN_COMMAND + L" " + meetingId;

			string escapedName = APIV3::SharedConfig::instance().Name();
			wstring name = CA2W(WinHTTP::URLUnescape(escapedName).c_str(), CP_UTF8);

			if (!name.empty())
			{
				newCmdString += L" " + NAME_FLAG + L" " + name;
			}

			string escapedEmail = APIV3::SharedConfig::instance().EMail();
			wstring email = CA2W(WinHTTP::URLUnescape(escapedEmail).c_str(), CP_UTF8);

			if (!email.empty())
			{
				newCmdString += L" " + EMAIL_FLAG + L" " + email;
			}

			wstring sessionId = CA2W(APIV3::SharedConfig::instance().SessionId().c_str(), CP_UTF8);
			wstring sessionKey = CA2W(APIV3::SharedConfig::instance().SessionKey().c_str(), CP_UTF8);

			if (!sessionId.empty() && !sessionKey.empty())
			{
				newCmdString += L" " + SESSION_ID_AND_KEY_FLAG + L" " + sessionId + L" " + sessionKey;
			}

			if (APIV3::SharedConfig::instance().Broadcast())
			{
				newCmdString += L" " + BROADCAST_FLAG;
			}

			bool rememberMe = APIV3::SharedConfig::instance().RememberMe();

			if (rememberMe)
			{
				newCmdString += (L" " + PERSIST_FLAG);
			}

			string sbcURL = APIV3::SharedConfig::instance().SBCURL();

			if (!sbcURL.empty())
			{
				newCmdString += (L" " + SBC_FLAG + L" " + wstring(CA2W(sbcURL.c_str(), CP_UTF8)));
			}

			ParseCommandLineString(newCmdString);
		}
		else
		{
			_bIsParsedComm = false;
		}
	} */
}

size_t 
CCommandLinePars::ParsJoinNameEmail(
		const vector<wstring>& parsLine,
		size_t &iIndex
	)
{
	while (iIndex < parsLine.size())
	{
		if(parsLine[iIndex] == NAME_FLAG)
		{
			_wstrUserName = _parseJoinName(parsLine, iIndex);
		}
		else if (parsLine[iIndex] == EMAIL_FLAG)
		{
			_wstrUserEmail = _parseJoinEmail(parsLine, iIndex);
		}
		else
		{
			iIndex++;
		}
	}
 	return iIndex;
}

size_t CCommandLinePars::ParsJoinParameters(const std::vector<std::wstring>& parsLine, size_t &iIndex)
{
    size_t idx = iIndex;
    
	while (idx < parsLine.size())
	{
        bool notParsed = false;
        
		if(parsLine[idx] == NAME_FLAG)
		{
			_wstrUserName = _parseJoinName(parsLine, idx);
		}
		else if (parsLine[idx] == EMAIL_FLAG)
		{
			_wstrUserEmail = _parseJoinEmail(parsLine, idx);
		}
		else if (parsLine[idx] == BROADCAST_FLAG)
		{
			_bIsBroadcastEnabled = true;

			idx++;
		}
		else if (parsLine[idx] == SESSION_ID_AND_KEY_FLAG)
		{
			idx++;

			vector<wstring> params;
				
			while ((idx < parsLine.size()) && (parsLine[idx].at(0) != COMMAND_MARKER))
			{
				params.push_back(parsLine[idx]);
                idx++;
			}
					
			if (params.size() == 2)
			{
				try
				{
					_iSessionId = lexical_cast<unsigned int>(params[0]);
					_iSessionKey = lexical_cast<unsigned int>(params[1]);
				}
                catch (boost::bad_lexical_cast const& e)
				{
					_iSessionId = 0;
					_iSessionKey = 0;

					//FAULT( e.what() );
				}
			}
		}
		else if (parsLine[idx] == PERSIST_FLAG)
		{
			idx++;
			
			_bRememberCredentials = true;
		}
		else if (parsLine[idx] == SBC_FLAG)
		{
			ParseCommand(idx, parsLine, PARS_SBC_PARAMETER);

			idx++;
		}
		else
		{
			idx++;
            notParsed = true;
		}
        
        if (!notParsed) {
            iIndex = idx;
		}
	}

 	return iIndex;
}

wstring 
CCommandLinePars::_parseJoinName(
		const vector<wstring>& parsLine,
		size_t &iIndex
	)
{
	wstring name;
	iIndex++;

	while(iIndex < parsLine.size()
					&&
		parsLine[iIndex].at(0) != COMMAND_MARKER )
	{
		name.append(parsLine[iIndex] +SPACE);
		iIndex++;
	}

	if(!name.empty())
	{
		name.erase(name.size() -1);
	}
	if(_validateName(name))
	{
		return name;
	}
	return EMPTY_LINE;
}


wstring 
CCommandLinePars::_parseJoinEmail(
		const vector<wstring>& parsLine, 
		size_t &iIndex
	)
{
	wstring email;
	iIndex++;
	
	if(parsLine.size() <= iIndex)
	{
		return EMPTY_LINE;
	}
	
	email = parsLine[iIndex];
	iIndex++;
	
	if(_validateEmail(email))
	{	
		return email;
	}
	return EMPTY_LINE;
}

bool replace(std::wstring& str, const std::wstring& from, const std::wstring& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::wstring::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

wstring s2ws(const std::string& str)
{
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    
    return converterX.from_bytes(str);
}

string ws2s(const std::wstring& wstr)
{
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    
    return converterX.to_bytes(wstr);
}

void 
CCommandLinePars::ParseCommand(
		size_t &iIndex, 
		vector<wstring> parsLine, 
		EParseLine enAction
	)
{
	switch(enAction)
	{
	case PARS_HOST_LOCAL:
		_eCommandType = enAction;
		if(parsLine.size() > 2 + iIndex)	// At least 3 params should be
		{
			_bIsParsedComm = true;
			// Get MeetingID
			ParsMeetingID(parsLine[iIndex]);
			++iIndex;

			// Get Server IP
			ParsServerIP(parsLine[iIndex]);
			++iIndex;

			// Get Server Port
			ParsServerPort(parsLine[iIndex]);
			++iIndex;
		}
		else
		{
			_bIsParsedComm = false;
		}
		break;
	case PARS_JOIN_LOCAL:
		_eCommandType = enAction;
		if(parsLine.size() > 2 + iIndex)	// At least 3 params should be
		{
			_bIsParsedComm = true;
			// Get MeetingID
			ParsMeetingID(parsLine[iIndex]);
			++iIndex;

			// Get Server IP
			ParsServerIP(parsLine[iIndex]);
			++iIndex;

			// Get Server Port
			ParsServerPort(parsLine[iIndex]);
			++iIndex;
		}
		else
		{
			_bIsParsedComm = false;
		}
		break;
	case PARS_HOST:
		_eCommandType = enAction;
		if( parsLine.size() > 1 + iIndex )	// At least 2 params should be
		{
			_bIsParsedComm = true;
			//Get Login/E-mail
			ParsLogin(parsLine[iIndex]);
			++iIndex;

			//Get Password
			ParsPassword(parsLine[iIndex]);
			++iIndex;
		}
		else
		{
			_bIsParsedComm = false;
		}
		break;
	case PARS_HOST_TOKEN:
		_eCommandType = enAction;
		if( parsLine.size() > 1 + iIndex )	// At least 2 params should be
		{
			_bIsParsedComm = true;
			//Get Subscription Id
			ParsSubscriptionId(parsLine[iIndex]);
			++iIndex;

			//Get Access Token
			ParsAccessToken(parsLine[iIndex]);
			++iIndex;
		}
		else
		{
			_bIsParsedComm = false;
		}
		break;
	case PARS_JOIN:
		_eCommandType = enAction;
		if(parsLine.size() > iIndex)	// At least 1 param should be
		{
			_bIsParsedComm = true;
			// Get MeetingID
			ParsMeetingID(parsLine[iIndex]);
			++iIndex;
			iIndex = ParsJoinParameters( parsLine, iIndex);
		}
		else
		{
			_bIsParsedComm = false;
		}
		break;
	case PARS_STUDIO:
		_eCommandType = enAction;
		if( parsLine.size() > 1 + iIndex )	// At least 2 params should be
		{
			_bIsParsedComm = true;
			//Get Login/E-mail
			ParsLogin(parsLine[iIndex]);
			++iIndex;

			//Get Password
			ParsPassword(parsLine[iIndex]);
			++iIndex;
		}
		else
		{
			_bIsParsedComm = false;
		}
		break;
            
        case PARS_UPDATE: {
            
            _bIsParsedComm = true;
            _bShowWhatsNew = true;
            _bShowUpdateBalloon = true;
            
            if (_eCommandType == PARS_UNKNOWN) {
                _eCommandType = PARS_SHOW_LOGIN_FORM;
            }
            
            int updateCode = -1;
            
            while(true)
            {
                if(iIndex >= parsLine.size())
                    break;
                
                if(parsLine[iIndex][0] != L'e')
                {
                    updateCode = boost::lexical_cast<int>(parsLine[iIndex]);
                    iIndex++;
                    _bShowUpdateBalloon = true; _bShowWhatsNew = true;
                    
                    if(iIndex >= parsLine.size())
                        break;
                    
                    if(parsLine[iIndex] == L"-join") {
                        _bShowJoin = true;
                        _bShowHost = false;
                    }
                    else if(parsLine[iIndex] == L"-host") {
                        _bShowJoin = false;
                        _bShowHost = true;
                    }
                    else
                    {
                        wstring wstr;
                        while(true)
                        {
                            wstr.append(parsLine[iIndex]);
                            iIndex++;
                            if(iIndex >= parsLine.size())
                                break;
                            wstr.append(SPACE);
                        }
                        if (wstr.find(L"%'") != wstring::npos)
                        {
                            replace(wstr, L"%'", L"\"");
                            replace(wstr, L"%% ", L"%");
                        }
                        ParseCommandLineString(wstr);
                        
                    }
                    _bShowUpdateBalloon = true; _bShowWhatsNew = true;
                }
                else break;
            }
            
            _updateCode = updateCode;
            
        } break;
            
	case PARS_SECURITY_TOKEN:
		if(parsLine.size() > 1 + iIndex)	// At least 2 param should be
		{
			_bIsParsedComm = true;
			
			// Get security token
			++iIndex;
			_wstrSecurityToken = parsLine[iIndex];
            
            wstring string_to_convert;
            
            std::string strToken = ws2s(_wstrSecurityToken);
			string  sContent   = base64_decode(strToken.c_str());
			wstring wstrUniLine = s2ws(sContent.c_str());

			wstring wstrLogin;
			wstring wstrPassword;

			size_t first_space = wstrUniLine.find_first_of(SPACE);

			if (first_space != wstring::npos)
			{
				wstrLogin = wstrUniLine.substr(0, first_space);
				wstrPassword = wstrUniLine.substr(first_space + 1, wstrUniLine.length());
			}

			if (wstrLogin.length() != 0 || wstrPassword.length() != 0)
			{
				//Get Login
				ParsLogin(wstrLogin);
				//Get Password
				ParsPassword(wstrPassword);
			}
		}
		else
		{
			_bIsParsedComm = false;
		}

		_eCommandType = PARS_HOST;
		break;
	case PARS_SBC_PARAMETER:
		if(parsLine.size() > 1 + iIndex)	// At least 2 param should be
		{
			// Get SBC from command line
			_bIsParsedComm = true;
			++iIndex;
			_wstrSbcParameter = parsLine[iIndex];
		}
		else
		{
			_bIsParsedComm = false;
		}

		break;
	case PARS_EMAIL_PARAMETER:
		if(parsLine.size() >= 1 + iIndex)	// At least 2 param should be
		{
			_bIsParsedComm = true;
			_wstrUserEmail = parsLine[iIndex];
			++iIndex;			
		}
		else
		{
			_bIsParsedComm = false;
		}

		break;
	case PARS_NAME:
		if(parsLine.size() >= 1 + iIndex)	// At least 2 param should be
		{
			// Get username
			_bIsParsedComm = true;
			_wstrUserName = parsLine[iIndex];
			++iIndex;
		}
		else
		{
			_bIsParsedComm = false;
		}
		break;
	case PARS_SHARED_CONFIG_TOKEN:

		if(parsLine.size() > 1 + iIndex)	// At least 2 param should be
		{
			_bIsParsedComm = true;
			
			// Get token
			++iIndex;
			_wstrSharedConfigToken = parsLine[iIndex];

			ParsSharedConfig(_wstrSharedConfigToken);
		}
		else
		{
			_bIsParsedComm = false;
		}
		break;
	default: 
		break;
	}	
}


bool 
CCommandLinePars::_validateEmail(const wstring& wstrEmail) const
{
	const wregex regex(
		L"[a-zA-Z0-9_\\-\\.]+@([a-zA-Z0-9_\\-]+\\.)+[a-zA-Z]{2,4}"
	);
	
	if(regex_match(wstrEmail, regex)
			&&
		MAX_EMAIL_LENGTH > wstrEmail.size())
	{
		return true;
	}
	return false;
}


bool
CCommandLinePars::_validateName(const wstring& wstrName) const
{
	if(static_cast<int>(wstrName.find_first_of(INVALID_NAME_SYMBOSL)) == -1 
			&& 
		wstrName.length() <= MAX_NAME_LENGTH)
	{
		return true;
	}
	return false;
}

void
CCommandLinePars::PrintHelp(const wstring& cmdline)
{
    wstring wstrCommandLine = cmdline;
    if (!wstrCommandLine.length()) {
        wstrCommandLine = _wstrPassedCommLine;
    }
    
#ifdef __WIN32__
	wchar_t dir[MAX_PATH*10+1];
	GetModuleFileName(NULL, dir, sizeof(dir));
	wstring str(dir);

	DWORD position = 0;
	AttachConsole(ATTACH_PARENT_PROCESS);

	HANDLE std_out = GetStdHandle(STD_OUTPUT_HANDLE);
    
#  define _writeStringToConsole(console, string, position) writeStringToConsole(console, string, position)
#else
    void *std_out = NULL;
#  define UNUSED(expr) (void)(expr)
#  define _writeStringToConsole(console, string, position) UNUSED(console); writeStringToConsole(string)
#endif

#ifdef FCC
	_writeStringToConsole(std_out, L"\n USAGE: FCC [option]", position);
#else
	_writeStringToConsole(std_out, L"\n USAGE: StartMeeting [option]", position);
#endif

	_writeStringToConsole(std_out, L"\n OPTIONS:", position);

	_writeStringToConsole(std_out, L"\n  [--host]                             "
			L"- host a meeting", position);
	_writeStringToConsole(std_out, L"\n  [--studio]                           "
			L"- start a studio recording", position);
	_writeStringToConsole(std_out, L"\n  [-s login password]                  "
			L"- provide credentials to host meeting", position);	
	_writeStringToConsole(std_out, L"\n  [-s1 securityToken]                  "
			L"- provide security token to host meeting", position);	
	_writeStringToConsole(std_out, L"\n  [-n \"FirstName LastName\"]          "
			L"- sets special name\n                                          "
			L"should be used with -s command", position);
	_writeStringToConsole(std_out, L"\n  [--join meetingID]                   "
			L"- join a meeting", position);
	_writeStringToConsole(std_out, L"\n  [-n \"FirstName LastName\"]          "
			L"- sets special name\n                                              "
			L"should be used with --join command", position);
	_writeStringToConsole(std_out, L"\n  [-e \"Email Address\"]               "
			L"- sets email address \n                                        "     
			L"should be used with --join command", position);
	_writeStringToConsole(std_out, L"\n  [-l]                                 "
			L"- leave meeting", position);
	_writeStringToConsole(std_out, L"\n  [-p]                                 "
			L"- persist user related input", position);
	_writeStringToConsole(std_out, L"\n  [-h][--help]                         "
			L"- show this message", position);
	_writeStringToConsole(std_out, L"\n\n EXAMPLE:", position);

#ifdef FCC
	_writeStringToConsole(std_out, L"\n           FCC --join 2-811-111 "
			L"-n \"myName myLastName\"\n\r", position);
#else
	_writeStringToConsole(std_out, L"\n           StartMeeting --join 2-811-111 "
			L"-n \"myName myLastName\"\n\r", position);
#endif

#ifdef __WIN32__
	int found = str.find_last_of(L"\\");
	_writeStringToConsole(std_out, L"\n", position);
	_writeStringToConsole(std_out, str.substr(0, found), position);
	_writeStringToConsole(std_out, L">", position);
#endif
}

#ifdef __WIN32__
void 
CCommandLinePars::writeStringToConsole(HANDLE std_out, const wstring& string, DWORD &position)
{
	WriteConsole(std_out, string.c_str(), string.length(), &position, NULL);
}
#else
void
CCommandLinePars::writeStringToConsole(const wstring& string)
{
    printf("%S", string.c_str());
}
#endif

CCommandLinePars::EErrorCodes CCommandLinePars::PrintErrorCode()
{
	return _eErrorCode;
}

bool 
CCommandLinePars::CheckForContradictoryCommand()
{
	if (_bIsParsedComm)
	{
		_bIsParsedComm = false;
		_eErrorCode = eContradictionaryCommands;
		return true;
	}
	return false;
}


bool 
CCommandLinePars::CheckForIncorrectCommandCombination(vector<string> CommandInput)
{
	bool _result = true;

	bool _studio = false;
	bool _host   = false;
	bool _join   = false;
	bool _host_set_up = false;
	bool _join_set_up = false;

	// action command
	vector<string>::iterator _studio_acc_it   = find(CommandInput.begin(), CommandInput.end(), "--studio");
	vector<string>::iterator _host_acc_it     = find(CommandInput.begin(), CommandInput.end(), "--host");
	vector<string>::iterator _join_acc_it     = find(CommandInput.begin(), CommandInput.end(), "--join");
	// cred set up command
	vector<string>::iterator _host_set_up_it  = find(CommandInput.begin(), CommandInput.end(), "-s");
	vector<string>::iterator _join_set_up_it  = find(CommandInput.begin(), CommandInput.end(), "-j");


	if (_studio_acc_it != CommandInput.end())
		_studio = true;

	if (_host_acc_it != CommandInput.end())
		_host = true;

	if (_join_acc_it != CommandInput.end())
		_join = true;

	if (_join_set_up_it != CommandInput.end())
		_join_set_up = true;

	if (_host_set_up_it != CommandInput.end())
		_host_set_up = true;

	// handle incorrect command combination
	if (_studio && _host && _join)
		_result = false;

	if (_studio && _join)
		_result = false;

	if (_host && _join)
		_result = false;

	if (_host_set_up && _join_set_up)
		_result = false;

	if (_studio && _join_set_up)
		_result = false;

	if (_host_set_up && _join)
		_result = false;

	return _result;
}

void
CCommandLinePars::ParseCommandLineString(wstring wstrCommandLine)
{
	ZeroVariables();

	_wstrPassedCommLine = wstrCommandLine;

#ifdef FSS_STARTMEETING
    
#ifdef WIN32
	//Browser plugin/web scheme start
	if (!_wstrPassedCommLine.empty() &&
		(_wstrPassedCommLine.find(APP_WEB_SCHEME) != wstring::npos))
	{
		//Trim right spaces
		boost::algorithm::trim_right(_wstrPassedCommLine);
		
		//Unescape URL
		DWORD unescapedCommandLineSize = wstrCommandLine.size();
		TCHAR* unescapedCommandLine = new TCHAR[unescapedCommandLineSize]; 	
	
		if (S_OK == UrlUnescape(const_cast<TCHAR*>(_wstrPassedCommLine.c_str()),
								unescapedCommandLine,
								&unescapedCommandLineSize, 0))
		{
			_wstrPassedCommLine = unescapedCommandLine;
		}

		delete[] unescapedCommandLine;
		unescapedCommandLine = NULL;
		
		//Browser plugin sends '/' symbol at the end of URL
		if (*_wstrPassedCommLine.rbegin() == L'/')
		{
			_wstrPassedCommLine.erase(_wstrPassedCommLine.length() - 1);
		}
	}
#else
    
    std::wstring APP_WEB_SCHEME;
    
    //Browser plugin/web scheme start
    if (!_wstrPassedCommLine.empty() &&
        (_wstrPassedCommLine.find(L"://") != wstring::npos))
    {
        APP_WEB_SCHEME = _wstrPassedCommLine;
        //Trim right spaces
        boost::algorithm::trim_right(APP_WEB_SCHEME);
        
        size_t pos = APP_WEB_SCHEME.rfind(L"://");
        if (pos != wstring::npos) {
            APP_WEB_SCHEME.erase(pos+3);
        }
    }
#endif

#endif

	string strCommandLine = ws2s(_wstrPassedCommLine.c_str());

//	INFO("Parse command line: " << strCommandLine.c_str());

#ifdef _WIN32
	vector<string> pars_str = po::split_winmain(strCommandLine);
#else
    vector<string> pars_str = po::split_unix(strCommandLine);
#endif


	if (!CheckForIncorrectCommandCombination(pars_str))
	{
		_eErrorCode = eWrongCommand;
		_eLoginAction = eNone;
		_eCommandType = PARS_SHOW_LOGIN_FORM;	

		return;
	}

	if(pars_str.size() > 0)
	{
		vector<wstring> pars;
		for (size_t i = 0; i < pars_str.size(); ++i)
		{
			wstring tmp = s2ws(pars_str[i].c_str());
			pars.insert(pars.end(), tmp );
		}

		size_t iIndex = 0;
		do 
		{
			if ( (pars[iIndex].length() == 2)
					&& 
				(pars[iIndex][0] == COMMAND_MARKER) )
			{
				_bIsCommandLine = true;
				bool bDoFinalBreak = false;
				wchar_t cmd = pars[iIndex][1];
				++iIndex;
				switch(cmd)
				{
	
				case L'e':
					ParseCommand(iIndex, pars, PARS_EMAIL_PARAMETER);
					break;

					//parse command line to join local meeting
				case L'c':
					ParseCommand(iIndex, pars, PARS_JOIN_LOCAL);
					break;

					//parse command line for host meeting  
				case L's':
					if (!CheckForContradictoryCommand())
					{
						ParseCommand(iIndex, pars, PARS_HOST);
						if (!_bIsParsedComm/* && _eErrorCode == eNoError*/)
						{
							_eErrorCode = eWrongHostParameters;
						}
					}
					break;
				//parse command line for host meeting by access token 
				case L't':
					if (!CheckForContradictoryCommand())
					{
						ParseCommand(iIndex, pars, PARS_HOST_TOKEN);

						if (!_bIsParsedComm)
						{
							_eErrorCode = eWrongHostParameters;
						}
					}
					break;
					//parse command line for join meeting
				case L'j':
					if (!CheckForContradictoryCommand() && _eErrorCode == eNoError)
					{
						ParseCommand(iIndex, pars, PARS_JOIN);

						_eLoginAction = eJoin;

						if (!_bIsParsedComm/* && _eErrorCode == eNoError*/)
							_eErrorCode = eWrongJoinParameters;						
					}
					break;

					//print help
				case L'h':
					if (!CheckForContradictoryCommand())
					{
						_eCommandType = PARS_HELP;
						_bPrintHelp = true;
						bDoFinalBreak = true;
						break;
					}

				case L'p':
					_bRememberCredentials = true;
					break;

				case L'u':
					ParseCommand(iIndex, pars, PARS_UPDATE);
					bDoFinalBreak = true;
					break;

				case L'l':
					if (!CheckForContradictoryCommand())
					{
						_eCommandType = PARS_LEAVE;
						_bIsParsedComm = true;
						_bLeaveMeeting = true;
					}
					break;

				case L'b':
					if (!CheckForContradictoryCommand())
					{
						_eCommandType = PARS_SHOW;
						_bIsParsedComm = true;
						_bBringToFront = true;
					}
					break;

				case L'q':
					_bIsQAServer = true;
					break;

					//parse command line for username
				case L'n':
						ParseCommand(iIndex, pars, PARS_NAME);
					break;

				default: break;
				}
				if (bDoFinalBreak)
				{
					break;
				}
			}
			//#47652. Win: command line options revise
			else if (pars[iIndex] == STUDIO_COMMAND)
			{
				if (iIndex + 1 > pars.size())
				{
					if ( pars[iIndex + 1] != L"-s")
					{
						_eLoginAction = eStudio;
						++iIndex;

						ParseCommand(iIndex, pars, PARS_HOST);

						if (!_bIsParsedComm)
							_eErrorCode = eWrongHostParameters;
					}
				}
				else
				{
					_eLoginAction = eStudio;
					++iIndex;
				}
			}
			else if (pars[iIndex] == HOST_COMMAND)
			{
				if (iIndex + 1 > pars.size())
				{
					if ( pars[iIndex + 1] != L"-s")
					{
						_eLoginAction = eHost;
						++iIndex;

						ParseCommand(iIndex, pars, PARS_HOST);

						if (!_bIsParsedComm)
							_eErrorCode = eWrongHostParameters;
					}
				}
				else
				{
					_eLoginAction = eHost;
					++iIndex;
				}
			}
			else if (pars[iIndex] == JOIN_COMMAND)
			{
				++iIndex;
				_eLoginAction = eJoin;

				ParseCommand(iIndex, pars, PARS_JOIN);

				if (!_bIsParsedComm)
					_eErrorCode = eWrongJoinParameters;
			}
#ifdef FSS_STARTMEETING
			else if (pars[iIndex] == (wstring(APP_WEB_SCHEME) + JOIN_FLAG))
			{
					++iIndex;
					_eLoginAction = eJoin;

					ParseCommand(iIndex, pars, PARS_JOIN);
					if (!_bIsParsedComm)
					{
						_eErrorCode = eWrongJoinParameters;
					}
			}
#endif
            else if (pars[iIndex] == JOIN_FLAG)
            {
                ++iIndex;
                _eLoginAction = eJoin;
                
                ParseCommand(iIndex, pars, PARS_JOIN);
                if (!_bIsParsedComm)
                {
                    _eErrorCode = eWrongJoinParameters;
                }
            }
			// request: override sbc parameter from config.xml 		
			else if (pars[iIndex] == SBC_FLAG)
			{
				if (!CheckForContradictoryCommand())
				{
					ParseCommand(iIndex, pars, PARS_SBC_PARAMETER);
					if (!_bIsParsedComm)
					{
						_eErrorCode = eWrongCommand;
					}
				}
			}
			// US: CommandLine security token
			else if (pars[iIndex] == L"-s1")
			{
				if (!CheckForContradictoryCommand())
				{
					ParseCommand(iIndex, pars, PARS_SECURITY_TOKEN);
					if (!_bIsParsedComm)
					{
						_eErrorCode = eWrongHostParameters;
					}
				}
			}
#ifdef FSS_STARTMEETING
//#ifdef WIN32
			else if (pars[iIndex] == (wstring(APP_WEB_SCHEME) + L"-s1"))
			{
				if (!CheckForContradictoryCommand())
				{
					ParseCommand(iIndex, pars, PARS_SECURITY_TOKEN);
					if (!_bIsParsedComm)
					{
						_eErrorCode = eWrongHostParameters;
					}
				}
			}
			else if (pars[iIndex] == (wstring(APP_WEB_SCHEME) + TOKEN_FLAG))
			{
				if (!CheckForContradictoryCommand())
				{
					ParseCommand(iIndex, pars, PARS_SHARED_CONFIG_TOKEN);

					if (!_bIsParsedComm)
					{
						_eErrorCode = eWrongCommand;
					}
				}
			}
//#endif
#endif
            else if (pars[iIndex] == TOKEN_FLAG ||
                     pars[iIndex] == SHORT_SHARED_TOKEN_FLAG ||
                     pars[iIndex] == SHARED_TOKEN_FLAG)
            {
                if (!CheckForContradictoryCommand())
                {
                    ParseCommand(iIndex, pars, PARS_SHARED_CONFIG_TOKEN);
                    
                    if (!_bIsParsedComm)
                    {
                        _eErrorCode = eWrongCommand;
                    }
                }
            }
			else if (pars[iIndex] == L"--help")
			{
				if (!CheckForContradictoryCommand())
				{
					_bIsParsedComm = true;
					_eCommandType = PARS_HELP;
					_bPrintHelp = true;
					++iIndex;
				}
			}
			// US: CommandLine security token
			else if (pars[iIndex] == L"-rc_enable")
			{
				++iIndex;
				_bIsRCEnabled = true;
			}
			else if(pars[iIndex] == UI_FLAG ) {
				++iIndex;
				if(pars.size() > iIndex ) {
					_wstrUI = pars[iIndex];
					++iIndex;
				}
			}
			else
			{
				++iIndex;	// Just ignore it
			}
		} while ( iIndex < pars.size() && _eErrorCode == eNoError);

	}
	else
	{
		_eCommandType = PARS_SHOW_LOGIN_FORM;
		_bIsParsedComm = true;
	}

	if (!_bIsParsedComm && _eErrorCode == eNoError)
		_eErrorCode = eWrongCommand;


		if (_eLoginAction == eNone && _eCommandType != PARS_LEAVE)
			_eCommandType = PARS_SHOW_LOGIN_FORM;	
}


int	
CCommandLinePars::ShowForm()
{
	if(_bShowJoin)
	{
		return 2;
	}

	if(_bShowHost)
	{
		return 1;
	}

	return 0;
}


void 
CCommandLinePars::ZeroVariables()
{
	_wstrMeetingID		= L"";
	_wstrConnectIP		= L"";
	_wstrUserName		= L"";
	_wstrLogin			= L"";
	_wstrPass			= L"";
	_wstrPassedCommLine = L"";
	_wstrSecurityToken  = L"";
	_wstrUserEmail      = L"";
	_wstrSbcParameter   = L"";

	_iConnectPort		= -1;
	_iSessionId			= 0;
	_iSessionKey		= 0;
	_bIsCommandLine		= false;
	_bIsParsedComm		= false;
	_bIsQAServer		= false;
	_bMinimizeToTray	= false;
	_bPrintHelp			= false;
	_bShowUpdateBalloon	= false;
        _updateCode          = -1;
	_bShowWhatsNew	= false;
	_bIsProcessedCommand = false;
	_bRememberCredentials = false;
	_bIsRCEnabled		= true;
	_bIsBroadcastEnabled = false;

	//IPC and additional commands 
	_bHostMeeting		= false;
	_bLeaveMeeting		= false;
	_bBringToFront		= false;

	//for updated program
	_bShowJoin			= false;
	_bShowHost			= false;
	_bIsUpdateInfo		= false;

	_eCommandType      = PARS_UNKNOWN;
	_eErrorCode        = eNoError;
	_eLoginAction      = eNone;

// In case it is Release and is not Production _bIsQAServer - by default true
#ifndef FSS_PRODUCTION
	_bIsQAServer = true;
#endif
    
    zeroVariables();

};


