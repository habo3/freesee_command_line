//
//  ParserProtocol.hpp
//  boost_program_options
//
//  Created by Oleksandr Skrypnyk on 12/05/16.
//  Copyright © 2016 Oleksandr Skrypnyk. All rights reserved.
//

#ifndef ParserProtocol_h
#define ParserProtocol_h

class Tester;

#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <sstream>

template <typename T>
class AbstractParser {
public:
    
    typedef std::basic_string<T> StringType;
    
    enum EParseLine
    {
#define ADD(n, value) n = value,
#include "parse_command_enum.h"
#undef ADD
    };
    
    enum EErrorCodes
    {
#define ADD(n, value) n = value,
#include "parse_error_enum.h"
#undef ADD
    };
    
    enum ELoginAction
    {
#define ADD(n, value) n = value,
#include "parse_action_enum.h"
#undef ADD
    };
    
    StringType GetSbcParameter() const { return _wstrSbcParameter; }
    
    StringType GetName() const { return _wstrUserName; }
    StringType GetServerIP() const { return _wstrConnectIP; }
    StringType GetMeetingID() const { return _wstrMeetingID; }
    StringType GetLogin() const { return _wstrLogin; }
    StringType GetPass() const { return _wstrPass; }
    
    const StringType& GetSubscriptionId() const { return _wstrSubscriptionId; }
    const StringType& GetAccessToken() const { return _wstrAccessToken; }
    const StringType& GetSharedConfigToken() const { return _wstrSharedConfigToken; }
    StringType GetSecurityToken() const { return _wstrSecurityToken; }
    
    unsigned int GetSessionId() const { return _iSessionId; }
    unsigned int GetSessionKey() const { return _iSessionKey; }

    bool IsBroadcastEnabled() const { return _bIsBroadcastEnabled;}
    bool IsLeaveMeeting() const {return _bLeaveMeeting; } //true - if passed parameter -l (leave meeting)
    
    StringType GetUserEmail() const { return _wstrUserEmail; }
    EParseLine GetCommandType() const { return _eCommandType; }
    
    EErrorCodes  getErrorCode() const { return _eErrorCode; }
    ELoginAction getLoginAction() const { return _eLoginAction; }
    
    int     GetUpdateCode() const { return _updateCode; }
    bool	ShowWhatsNew() const { return _bShowWhatsNew; } // true if runs immediately after update
    bool	ShowBalloon() const {return	_bShowUpdateBalloon; }	//true if show update balloon
    bool	ShouldSaveCredentials()	const { return _bRememberCredentials; } // true if we should save the successfulll credentials
    
    void zeroVariables() {
        _iConnectPort = 0;
        _iSessionId = 0;
        _iSessionKey = 0;
        _bIsBroadcastEnabled = false;
        _eCommandType = PARS_UNKNOWN;
        _eErrorCode = eNoError;
        _eLoginAction = eNone;
        _updateCode = -1;
        _bShowWhatsNew = false;
        _bShowUpdateBalloon = false;
    }
    
protected:
    friend class Tester;
    
    StringType _wstrMeetingID;
    StringType _wstrConnectIP;
    StringType _wstrUserName;
    StringType _wstrLogin;
    StringType _wstrUserEmail;
    StringType _wstrPass;
    StringType _wstrSecurityToken;
    StringType _wstrSbcParameter;
    StringType _wstrSharedConfigToken;
    StringType _wstrSubscriptionId;
    StringType _wstrAccessToken;
    
    int _updateCode;
    int  _iConnectPort;
    unsigned int _iSessionId;
    unsigned int _iSessionKey;
    
    bool _bIsBroadcastEnabled;
    bool _bShowWhatsNew;
    bool _bShowUpdateBalloon;
    bool _bRememberCredentials;
    bool _bLeaveMeeting;
    
    EParseLine _eCommandType;
    EErrorCodes _eErrorCode;
    ELoginAction _eLoginAction;
};

#endif /* ParserProtocol_h */
