#ifndef __COMMANDLINEPARS_H__
#define __COMMANDLINEPARS_H__

#include "ParserProtocol.hpp"
#include <vector>

class CCommandLinePars : public AbstractParser<wchar_t>
{
public:
	
	CCommandLinePars() {
		ZeroVariables();
	}
    
	void	ParseCommand(size_t &iIndex, std::vector<std::wstring> parsLine, EParseLine enAction);
	void	ParseCommandLineString(std::wstring wstrCommandLine);
	bool	CheckForContradictoryCommand();
	bool	CheckForIncorrectCommandCombination(std::vector<std::string> CommandInput);
	void	Clear() {ZeroVariables();}
    
	void	ParsMeetingID(std::wstring wstrMeetingID);
	size_t	ParsJoinNameEmail(const std::vector<std::wstring>& parsLine,
                              size_t &iIndex
                              );
	size_t	ParsJoinParameters(const std::vector<std::wstring>& parsLine,
                               size_t &iIndex
                               );
	std::wstring ProcessMeetingID();
	void	ParsServerIP(std::wstring wstrServerID);
	void	ParsServerPort(std::wstring wstrServerPort);
    
	void	ParsLogin(std::wstring wstrLogin);
	void	ParsPassword(std::wstring wstrPassword);
	void	ParsSubscriptionId(const std::wstring& wstrSubscriptionId);
	void	ParsAccessToken(const std::wstring& wstrAccessToken);
	void	ParsSharedConfig(const std::wstring& wstrSharedConfigToken);
    
	void	PrintHelp(const std::wstring& wstrCommandLine);
	EErrorCodes	PrintErrorCode();
    
	void	SaveCredentials(bool bSaveStartUp = true);
    
    void ClearSbcParameter() { _wstrSbcParameter.clear(); }
    
	int		GetPort()		{return _iConnectPort; }
	bool	IsCommandLine()	{return _bIsCommandLine; }		//true if from command line was passed parameters
	bool	IsParsedComm()	{return _bIsParsedComm; }		//true if all parameters from comline was parsed successfully
	bool	IsQAServer()	{return _bIsQAServer; }		//true if it is QA server used
	bool	DoHelp()		{return _bPrintHelp; };		//true if passed -h for print help information
	void	WhatsNewShown() { _bShowWhatsNew = false; }
	void	BalloonShowed()	{ _bShowUpdateBalloon = false; }
	bool	ShouldSaveCredentials()	{return _bRememberCredentials; } // true if we should save the successfulll credentials
    
	bool	IsProcessed()	{ return _bIsProcessedCommand; }
	void	Processed()		{ _bIsProcessedCommand = true; }
    
	int 	ShowForm();										//for updated program: 2 - show join; 1 - show host;
	std::wstring GetCommandLine(){return _wstrPassedCommLine; }	//return all parameters passed through command line
	bool	IsUpdateInfo()	{return _bIsUpdateInfo; }		//true if passed -r param (update server[:port])
    
	//IPC and additional commands
	bool	IsHostMeeting() {return _bHostMeeting; }		//true - if passed parameter -s (host) and -a (start meeting)
	bool	IsLeaveMeeting(){return _bLeaveMeeting; }		//true - if passed parameter -l (leave meeting)
	bool	IsBringToFront(){return _bBringToFront; }		//true - if passed parameter -b (bring app to front)
	bool	IsRCEnabled(){return _bIsRCEnabled;}			//true - if RemoteControl enabled
	
	std::wstring GetUI()		{return _wstrUI; };

#ifdef __WIN32
	void writeStringToConsole(HANDLE std_out, const std::wstring& string, DWORD &position);
#else
    void writeStringToConsole(const std::wstring& string);
#endif
    
protected:
//	friend class Singleton<CCommandLinePars>;
    
private:
    
	void ZeroVariables();
	
	
	std::wstring _parseJoinName(
                                const std::vector<std::wstring>& parsLine,
                                size_t &iIndex
                                );
	std::wstring _parseJoinEmail(
                                 const std::vector<std::wstring>& parsLine,
                                 size_t &iIndex
                                 );
    
	bool _validateEmail(const std::wstring& wstrEmail) const;
	bool _validateName(const std::wstring& wstrName) const;
    
    std::wstring _wstrPassedCommLine;
    
	bool _bIsCommandLine;
	bool _bIsParsedComm;
	bool _bIsQAServer;
	bool _bMinimizeToTray;
	bool _bPrintHelp;
	bool _bIsProcessedCommand;
	bool _bIsRCEnabled;
   
	//for updated program
	bool _bShowJoin;
	bool _bShowHost;
	bool _bIsUpdateInfo;
	bool skip_get_from_string;
    
	//IPC and additional commands 
	bool _bHostMeeting;
	bool _bLeaveMeeting;
	bool _bBringToFront;
	
	std::wstring _wstrUI;
	
};
#endif //__COMMANDLINEPARS_H__