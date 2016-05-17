//
//  new_cmd_parser.cpp
//  boost_program_options
//
//  Created by Oleksandr Skrypnyk on 12/05/16.
//  Copyright © 2016 Oleksandr Skrypnyk. All rights reserved.
//

#include <boost/program_options.hpp>
#include <boost/functional.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/regex.hpp>
#include <iostream>

#include "new_cmd_parser.hpp"
#include "Base64.hxx"

using namespace boost::program_options;

static const int MAX_NAME_LENGTH = 40;

template<class T>
typed_value<T>*
construct_value(T* v, const char * value_typename)
{
    typed_value<T>* r = new typed_value<T>(v);
    r->value_name( value_typename );
    
    return r;
}

bool validate_options(variables_map& vm) {
    std::set<std::string> main_commands;
    if (vm.count("help"))
        main_commands.insert("help");
    if (vm.count("host"))
        main_commands.insert("host");
    if (vm.count("join"))
        main_commands.insert("join");
    if (vm.count("studio"))
        main_commands.insert("studio");
    
    if (main_commands.size() > 1)
        return false;
    
    if (vm.count("s") && vm.count("join"))
        return false;
    
    if (vm.count("s") && vm.count("j"))
        return false;
    
    if (vm.count("studio") && vm.count("j"))
        return false;
    
    
    
    return true;
}

#pragma mark - validation helpers
bool is_urlscheme(const std::string& string) {
    return string.find("://") != std::string::npos;
}

bool is_login(const std::string& login)
{
    const boost::regex regex("[a-zA-Z0-9_\\-\\.@]{1,}");
    
    return boost::regex_match(login, regex);
}


bool is_password(const std::string& password)
{
    return password.length() != 0;
}

bool is_email(const std::string& email)
{
    static const int MAX_EMAIL_LENGTH = 254;
    const boost::regex regex(
                             "[a-zA-Z0-9_\\-\\.]+@([a-zA-Z0-9_\\-]+\\.)+[a-zA-Z]{2,4}"
                             );
    
    return (boost::regex_match(email, regex) && MAX_EMAIL_LENGTH > email.size());
}


bool is_name(const std::string& name)
{
    static const std::string INVALID_NAME_SYMBOLS = "%\"&*";
    return (static_cast<int>(name.find_first_of(INVALID_NAME_SYMBOLS)) == -1 &&
            name.length() <= MAX_NAME_LENGTH);
}

bool is_meeting_id(const std::string& meeting)
{
    return !meeting.empty();
}

#pragma mark - parser implementaion
void CommandLine::parse_cmdline(int argc, const char *argv[]) {
    std::stringstream ss;
    for (size_t i = 1; i < argc; ++i) {
        ss << argv[i] << ' ';
    }
    
    parse_cmdline(ss.str());
}

void CommandLine::parse_cmdline(const std::string& input) {
    
    std::string cmdline = input;
    if (cmdline.find("://") != std::string::npos) {
        std::vector<std::string> url_items;
        boost::iter_split(url_items, cmdline, boost::algorithm::first_finder("://"));
        cmdline = url_items[1];
    }
    
    options_description desc("Options");
    
    options_description generic("Generic options");
    generic.add_options()
    ("help,h", "Help screen");
    
    std::vector<std::string> credentials;
    std::vector<std::string> call_replacement;
    std::vector<std::string> host_access_token;
    
    options_description session("Conferencing options");
    session.add_options()
    ("host", value<bool>()->zero_tokens(), "Host meeting")
    ("join,j", construct_value<std::string>(&_wstrMeetingID, "meetingId"), "join a meeting")
    ("studio", "start a studio recording")
    ("s", construct_value<std::vector<std::string> >(&credentials, "login password")->multitoken(), "provide credentials to host meeting")
    ("s1", value<std::string>(&_wstrSecurityToken), "provide security token to host meeting")
    ("t", construct_value<std::vector<std::string> >(&host_access_token, "subscriptionId accessToken")->multitoken(), "provide account access token to host meeting")
    (",n", construct_value<std::string>(&_wstrUserName, "\"firstName lastName\""), "sets special name\nshould be used with -s and --join commands")
    (",e", value<std::string>(&_wstrUserEmail), "sets email address\nshould be used with --join")
    (",l", value<bool>(&_bLeaveMeeting)->zero_tokens(), "leave meeting")
    (",p", value<bool>(&_bRememberCredentials)->zero_tokens(), "persist user related input")
    ;
    
    options_description hidden("Hidden options");
    hidden.add_options()
    (",u", value<int>(&_updateCode), "self-update result code (provided by installer)")
    ("st", value<std::string>(), "provide shared token")
    ("shared_token", value<std::string>(), "provide shared token")
    (",r", value<std::vector<std::string> >(&call_replacement)->multitoken(), "provide call replacement details")
    (",b", value<bool>()->zero_tokens(), "start broadcast automatically")
    ;
    
    options_description visible("Allowed Options");
    visible.add(generic).add(session).add(hidden);
    
    variables_map vm;
    
#ifdef _WIN32
    std::vector<std::string> pars_str = split_winmain(cmdline);
#else
    std::vector<std::string> pars_str = split_unix(cmdline);
#endif
    
    store(command_line_parser(pars_str)
          .options(visible)
          .style(
                 command_line_style::unix_style |
                 command_line_style::allow_long_disguise).run(),
          vm);
    notify(vm);
    
    if (vm.count("help"))
        std::cout << visible << '\n';
    
    zeroVariables();
    
    _eCommandType = PARS_SHOW_LOGIN_FORM;
    
    if (!validate_options(vm)) {
        _eErrorCode = eWrongCommand;
        _eLoginAction = eNone;
        return;
    }
    
    if (vm.count("help")) {
        return;
    }
    
    if (vm.count("-u")) {
        _updateCode = vm["-u"].as<int>();
        _bShowUpdateBalloon = true;
        _bShowWhatsNew = (_updateCode == 10);
    }
    
    if (vm.count("-n")) {
        _wstrUserName = (is_name(_wstrUserName) ? _wstrUserName : "");
    }
    
    if (vm.count("-e") ) {
        _wstrUserEmail = is_email(_wstrUserEmail) ? _wstrUserName : "";
    }
    
    if (vm.count("s1")) {
        std::string strToken = vm["s1"].as<std::string>();
        
        std::string login;
        std::string password;
        
        try {
            std::string sContent = base64_decode(strToken.c_str());
            size_t first_space = sContent.find_first_of(" ");
            
            
            if (first_space != std::string::npos)
            {
                login = sContent.substr(0, first_space);
                password = sContent.substr(first_space + 1, sContent.length());
            }
            
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        
        if (login.length() != 0 || password.length() != 0)
        {
            _wstrLogin = is_login(login) ? login : "";
            _wstrPass = is_password(password) ? password : "";
        }
    }
    
    if (vm.count("s")) {
        if (credentials.size() == 2) {
            std::string login = credentials[0];
            std::string password = credentials[1];
            
            _wstrLogin = is_login(login) ? login : "";
            _wstrPass = is_password(password) ? password : "";
        }
    }
    
    if (vm.count("host")) {
        
        if (vm.count("s")) {
            if (credentials.size() == 2) {
                _eCommandType = PARS_HOST;
                _eLoginAction = eHost;
            }
        }
        
        if (vm.count("s1")) {
            _eCommandType = PARS_HOST;
            _eLoginAction = eHost;
        }
        
        if (vm.count("t")) {
            if (host_access_token.size() == 2) {
                _wstrSubscriptionId = host_access_token[0];
                _wstrAccessToken = host_access_token[1];
                _eCommandType = PARS_HOST_TOKEN;
                _eLoginAction = eHost;
            }
        }
        
        if (_eLoginAction == eNone && _eCommandType != PARS_LEAVE)
            _eCommandType = PARS_SHOW_LOGIN_FORM;
    }
    
    while (vm.count("join")) {
        _eLoginAction = eJoin;
        
        if (vm.count("-b"))
            _bIsBroadcastEnabled = true;
        
        if (vm.count("-r")) {
            if (call_replacement.size() == 2) {
                _iSessionId = boost::lexical_cast<unsigned int>(call_replacement[0]);
                _iSessionKey = boost::lexical_cast<unsigned int>(call_replacement[1]);
            }
        }
        
        if (vm.count("s1") || vm.count("s")) {
            _eCommandType = PARS_JOIN;
            _eErrorCode = eWrongCommand;
            break;
        }
        
        _wstrMeetingID = is_meeting_id(_wstrMeetingID) ? _wstrMeetingID : "";
        
        _eCommandType = PARS_JOIN;
        break;
    }
    
    if (vm.count("studio")) {
        _eCommandType = PARS_HOST;
        _eLoginAction = eStudio;
    }
}
