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

template <typename T>
class AbstractParser {
public:
    
    typedef std::basic_string<T> StringType;
    
    enum EParseLine
    {
        PARS_UNKNOWN = -1,
        PARS_JOIN_LOCAL = 1,
        PARS_HOST_LOCAL = 2,
        PARS_JOIN = 3,
        PARS_HOST = 4,
        PARS_UPDATE = 5,
        PARS_NAME = 6,
        PARS_LEAVE = 7,
        PARS_SHOW = 8,
        PARS_SHOW_LOGIN_FORM = 9,
        PARS_HELP = 10,
        PARS_SECURITY_TOKEN = 11,
        PARS_SBC_PARAMETER = 12,
        PARS_EMAIL_PARAMETER = 13,
        PARS_ENABLE_RC = 14,
        PARS_NONE = 15,
        PARS_STUDIO = 16,
        PARS_SHARED_CONFIG_TOKEN = 17,
        PARS_HOST_TOKEN = 18,
        PARS_UI = 19
    };
    
    enum EErrorCodes
    {
        eWrongHostParameters = 2,
        eWrongJoinParameters = 2,
        eWrongCommand = 3,
        eContradictionaryCommands = 3,
        eNoError = 5
    };
    
    enum ELoginAction
    {
        eNone,
        eHost = 1,
        eStudio = 2,
        eJoin = 3
    };
    
    StringType GetSbcParameter() { return _wstrSbcParameter; }
    StringType GetName() { return _wstrUserName; }
    StringType GetServerIP() { return _wstrConnectIP; }
    StringType GetMeetingID() { return _wstrMeetingID; }
    StringType GetLogin() { return _wstrLogin; }
    StringType GetPass() { return _wstrPass; }
    
    const StringType& GetSubscriptionId() { return _wstrSubscriptionId; }
    const StringType& GetAccessToken() { return _wstrAccessToken; }
    const StringType& GetSharedConfigToken() { return _wstrSharedConfigToken; }
    
    unsigned int GetSessionId() { return _iSessionId; }
    unsigned int GetSessionKey() { return _iSessionKey; }
    bool IsBroadcastEnabled() { return _bIsBroadcastEnabled;}
    
    StringType GetUserEmail() { return _wstrUserEmail; }
    EParseLine GetCommandType() { return _eCommandType; }
    
    EErrorCodes  getErrorCode() { return _eErrorCode; }
    ELoginAction getLoginAction() { return _eLoginAction; }
    
    int     GetUpdateCode() { return _updateCode; }
    bool	ShowWhatsNew() { return _bShowWhatsNew; } // true if runs immediately after update
    bool	ShowBalloon()	{return	_bShowUpdateBalloon; }	//true if show update balloon
    
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
    
    EParseLine _eCommandType;
    EErrorCodes _eErrorCode;
    ELoginAction _eLoginAction;
};

class Tester {
public:
    bool compareTo(const AbstractParser<wchar_t>* n1, const AbstractParser<char>* n2) {
        
        //    assert (_wstrMeetingID == n->_wstrMeetingID );
        //    assert (_wstrConnectIP == n->_wstrConnectIP );
        //    assert (_wstrUserName  == n->_wstrUserName );
        //    assert (_wstrLogin     == n->_wstrLogin );
        //    assert (_wstrUserEmail == n->_wstrUserEmail );
        //    assert (_wstrPass      == n->_wstrPass );
        //    assert (_wstrSecurityToken == n->_wstrSecurityToken );
        //    assert (_wstrSbcParameter == n->_wstrSbcParameter );
        //    assert (_wstrSharedConfigToken == n->_wstrSharedConfigToken );
        //    assert (_wstrSubscriptionId  == n->_wstrSubscriptionId );
        //    assert (_wstrAccessToken     == n->_wstrAccessToken );
        //
//        assert (n1->_iSessionId          == n2->_iSessionId );
//        assert (n1->_iSessionKey         == n2->_iSessionKey);
//        assert (n1->_bIsBroadcastEnabled == n2->_bIsBroadcastEnabled);
//        assert (n1->_eCommandType        == n2->_eCommandType);
//        assert (n1->_eErrorCode          == n2->_eErrorCode);
//        assert (n1->_eLoginAction        == n2->_eLoginAction);
//        assert (n1->_updateCode          == n2->_updateCode);
//        assert (n1->_bShowWhatsNew      == n2->_bShowWhatsNew);
//        assert (n1->_bShowUpdateBalloon == n2->_bShowUpdateBalloon);
        
        return /*_wstrMeetingID       == n->_wstrMeetingID &&
                _wstrConnectIP       == n->_wstrConnectIP &&
                _wstrUserName        == n->_wstrUserName  &&
                _wstrLogin           == n->_wstrLogin     &&
                _wstrUserEmail       == n->_wstrUserEmail &&
                _wstrPass            == n->_wstrPass      &&
                _wstrSecurityToken   == n->_wstrSecurityToken &&
                _wstrSbcParameter    == n->_wstrSbcParameter  &&
                _wstrSharedConfigToken == n->_wstrSharedConfigToken &&
                _wstrSubscriptionId  == n->_wstrSubscriptionId &&
                _wstrAccessToken     == n->_wstrAccessToken &&*/
        
        n1->_iSessionId          == n2->_iSessionId &&
        n1->_iSessionKey         == n2->_iSessionKey &&
        n1->_bIsBroadcastEnabled == n2->_bIsBroadcastEnabled &&
        n1->_eCommandType        == n2->_eCommandType &&
        n1->_eErrorCode          == n2->_eErrorCode &&
        n1->_eLoginAction        == n2->_eLoginAction &&
        n1->_updateCode          == n2->_updateCode &&
        n1->_bShowWhatsNew      == n2->_bShowWhatsNew &&
        n1->_bShowUpdateBalloon == n2->_bShowUpdateBalloon &&
        n1->_iSessionId == n2->_iSessionId &&
        n1->_iSessionKey == n2->_iSessionKey
        ;
        
        return true;
    }
};

#endif /* ParserProtocol_h */
