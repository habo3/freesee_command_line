#pragma once

#include "new_cmd_parser.hpp"

class TestParser : public CommandLine {
    
public:
    
    void load_json(boost::property_tree::ptree& node) {
        zeroVariables();
        
        _updateCode = node.get<int>("updateCode", -1);
        _bShowWhatsNew = node.get<bool>("showWhatsNew", false);
        _bShowUpdateBalloon = node.get<bool>("showUpdateBalloon", false);
        _bIsBroadcastEnabled = node.get<bool>("isBroadcastEnabled", false);
        _bRememberCredentials = node.get<bool>("rememberCredentials", false);

        _iSessionId = node.get<int>("sessionId", 0);
        _iSessionKey = node.get<int>("sessionKey", 0);
        _iConnectPort = node.get<int>("connectPort", 0);
        
        _eCommandType = jsonToCommand(node.get<std::string>("commandType" , "PARS_UNKNOWN"));
        _eLoginAction = jsonToAction(node.get<std::string>("loginAction", "eNone"));
        _eErrorCode   = jsonToErrorCode(node.get<std::string>("errorCode", "eNoError"));
        
        _wstrMeetingID = node.get<std::string>("meetingId", "");
        _wstrConnectIP = node.get<std::string>("connectIP", "");
        _wstrUserName = node.get<std::string>("userName", "");
        _wstrLogin = node.get<std::string>("login", "");
        _wstrPass = node.get<std::string>("password", "");
        _wstrUserEmail = node.get<std::string>("email", "");
        _wstrSecurityToken = node.get<std::string>("securityToken", "");
        _wstrSbcParameter = node.get<std::string>("sbc", "");
        _wstrSharedConfigToken = node.get<std::string>("sharedConfigToken", "");
        _wstrSubscriptionId = node.get<std::string>("subscriptionId", "");
        _wstrAccessToken = node.get<std::string>("accessToken", "");
        _wstrPhone = node.get<std::string>("phone", "");
        _bLeaveMeeting = node.get<bool>("leaveMeeting", false);
        _bPrintHelp = node.get<bool>("help", false);
        
        this->node = node;
    }
    
    bool has(const std::string& v) const {
        bool found = node.find(v) != node.not_found();
        if (found) std::cout << v << " ";
        return found;
    }
    
    bool operator == (const CommandLine& n) const {
        
        return (
          (!has("meetingId") || _wstrMeetingID == n.GetMeetingID())
        && (!has("userName") || _wstrUserName == n.GetName())
        && (!has("login") || _wstrLogin == n.GetLogin())
        && (!has("password") || _wstrPass == n.GetPass())
        && (!has("email") || _wstrUserEmail == n.GetUserEmail())
        && (!has("securityToken") || _wstrSecurityToken == n.GetSecurityToken())
        && (!has("sbc") || _wstrSbcParameter == n.GetSbcParameter())
                
        && (!has("sharedConfigToken") || _wstrSharedConfigToken == n.GetSharedConfigToken())
        && (!has("subscriptionId") || _wstrSubscriptionId == n.GetSubscriptionId())
        && (!has("accessToken") || _wstrAccessToken == n.GetAccessToken())

        && (!has("connectIP") || _wstrConnectIP == n.GetServerIP())

        && (!has("sessionId") || _iSessionId == n.GetSessionId())
        && (!has("sessionKey") || _iSessionKey == n.GetSessionKey())
        && (!has("isBroadcastEnabled") || _bIsBroadcastEnabled == n.IsBroadcastEnabled())
        && (!has("commandType") || _eCommandType == n.GetCommandType())
        && (!has("errorCode") || _eErrorCode == n.getErrorCode())
        && (!has("loginAction") || _eLoginAction == n.getLoginAction())
        && (!has("updateCode") || _updateCode == n.GetUpdateCode())
        && (!has("showWhatsNew") || _bShowWhatsNew == n.ShowWhatsNew())
        && (!has("showUpdateBalloon") || _bShowUpdateBalloon == n.ShowBalloon())
        && (!has("rememberCredentials") || _bRememberCredentials == n.ShouldSaveCredentials())
        && (!has("leaveMeeting") || _bLeaveMeeting == n.IsLeaveMeeting())
        && (!has("phone") || _wstrPhone == n.GetPhoneNumber())
        && (!has("help") || _bPrintHelp == n.DoHelp())
        );
        
    }
    
protected:
    EParseLine jsonToCommand(const std::string &s) {
#define ADD(n, value) if (#n == s) return n;
#include "parse_command_enum.h"
#undef ADD
        assert( 0 );
        return PARS_UNKNOWN;
    }
    
    ELoginAction jsonToAction(const std::string &s) {
#define ADD(n, value) if (#n == s) return n;
#include "parse_action_enum.h"
#undef ADD
        assert( 0 );
        return eNone;
    }
    
    EErrorCodes jsonToErrorCode(const std::string &s) {
#define ADD(n, value) if (#n == s) return n;
#include "parse_error_enum.h"
#undef ADD
        assert( 0 );
        return eNoError;
    }

    boost::property_tree::ptree node;
};

class Tester {
public:
    bool compareTo(const AbstractParser<wchar_t>* n1, const AbstractParser<char>* n2) {
        
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
    
    template <typename T>
    bool compareTo(const AbstractParser<T>& p, const AbstractParser<T>& test) {
        return
        p._wstrMeetingID       == test._wstrMeetingID &&
        p._wstrConnectIP       == test._wstrConnectIP &&
        p._wstrUserName        == test._wstrUserName  &&
        p._wstrLogin           == test._wstrLogin     &&
        p._wstrUserEmail       == test._wstrUserEmail &&
        p._wstrPass            == test._wstrPass      &&
        p._wstrSecurityToken   == test._wstrSecurityToken &&
        p._wstrSbcParameter    == test._wstrSbcParameter  &&
        p._wstrSharedConfigToken == test._wstrSharedConfigToken &&
        p._wstrSubscriptionId  == test._wstrSubscriptionId &&
        p._wstrAccessToken     == test._wstrAccessToken &&
        
        p._iSessionId          == test._iSessionId &&
        p._iSessionKey         == test._iSessionKey &&
        p._bIsBroadcastEnabled == test._bIsBroadcastEnabled &&
        p._eCommandType        == test._eCommandType &&
        p._eErrorCode          == test._eErrorCode &&
        p._eLoginAction        == test._eLoginAction &&
        p._updateCode          == test._updateCode &&
        p._bShowWhatsNew      == test._bShowWhatsNew &&
        p._bShowUpdateBalloon == test._bShowUpdateBalloon &&
        p._iSessionId == test._iSessionId &&
        p._iSessionKey == test._iSessionKey
        ;
    }
};