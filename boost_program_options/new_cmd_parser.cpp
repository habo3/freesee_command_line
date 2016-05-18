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

typedef std::vector<std::string> multitoken_type;

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
    
    zeroVariables();
    
    std::string url_scheme;
    std::string cmdline = input;
    if (cmdline.find("://") != std::string::npos) {
        std::vector<std::string> url_items;
        boost::iter_split(url_items, cmdline, boost::algorithm::first_finder("://"));
        url_scheme = url_items[0];
        cmdline = url_items[1];
    }
    
    options_description desc("Options");
    
    options_description generic("Generic options");
    generic.add_options()
    ("help,h", value<bool>(&_bPrintHelp)->zero_tokens(), "Help screen");
    
    std::vector<std::string> credentials;
    std::vector<std::string> call_replacement;
    std::vector<std::string> host_access_token;
    
    auto handle_n = [this](const std::string& name){ _wstrUserName = (is_name(name) ? name : ""); };
    auto handle_e = [this](const std::string& email) { _wstrUserEmail = is_email(email) ? email : ""; };
    auto handle_updateCode = [this](int v) {
        _updateCode = v;
        _bShowUpdateBalloon = true;
        _bShowWhatsNew = (_updateCode == 10);
    };
    
    auto handle_s1 = [this](const std::string &token) {
        std::string login;
        std::string password;
        
        try {
            std::string sContent = base64_decode(token.c_str());
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
    };
    
    auto handle_s = [this](const multitoken_type& values) {
        if (values.size() == 2) {
            std::string login = values[0];
            std::string password = values[1];
            
            _wstrLogin = is_login(login) ? login : "";
            _wstrPass = is_password(password) ? password : "";
        }
    };
    
    auto handle_r = [this](const multitoken_type& values) {
        if (values.size() == 2) {
            _iSessionId = boost::lexical_cast<unsigned int>(values[0]);
            _iSessionKey = boost::lexical_cast<unsigned int>(values[1]);
        }
    };
    
    auto handle_t = [this](const std::string& value) {
        _wstrSharedConfigToken = value;
    };

    auto handle_t_no_scheme = [this](const multitoken_type& values) {
            if (values.size() == 2) {
                _wstrSubscriptionId = values[0];
                _wstrAccessToken = values[1];
            }
    };

    auto handle_join = [this](const std::string& meetingId) {
        _wstrMeetingID = is_meeting_id(meetingId) ? meetingId : "";
    };
    
    options_description session("Conferencing options");
    session.add_options()
    ("host", value<bool>()->zero_tokens(), "Host meeting")
    ("join,j", construct_value<std::string>(&_wstrMeetingID, "meetingId")->notifier(handle_join), "join a meeting")
    ("studio", "start a studio recording")
    ("s", construct_value<multitoken_type>(&credentials, "login password")->multitoken()->notifier(handle_s), "provide credentials to host meeting")
    (",n", construct_value<std::string>(&_wstrUserName, "\"firstName lastName\"")->notifier(handle_n), "sets special name\nshould be used with -s and --join commands")
    (",e", value<std::string>(&_wstrUserEmail)->notifier(handle_e), "sets email address\nshould be used with --join")
    (",l", value<bool>(&_bLeaveMeeting)->zero_tokens(), "leave meeting")
    (",p", value<bool>(&_bRememberCredentials)->zero_tokens(), "persist user related input")
    ;
    
    options_description hidden("Hidden options");
    hidden.add_options()
    ("s1", value<std::string>(&_wstrSecurityToken)->notifier(handle_s1), "provide security token to host meeting")
    (",u", value<int>(&_updateCode)->notifier(handle_updateCode), "self-update result code (provided by installer)")
    ("st", value<std::string>()->notifier(handle_t), "provide shared token")
    ("shared_token", value<std::string>()->notifier(handle_t), "provide shared token")
    (",r", construct_value<multitoken_type>(&call_replacement, "sessionId sessionKey")->multitoken()->notifier(handle_r), "provide call replacement details")
    (",b", value<bool>(&_bIsBroadcastEnabled)->zero_tokens(), "start broadcast automatically")
    ;

    if (url_scheme.empty())
        hidden.add_options()
        ("t", construct_value<multitoken_type>(&host_access_token, "subscriptionId accessToken")->multitoken()->notifier(handle_t_no_scheme), "provide account access token to host meeting");
    else
        hidden.add_options()
        ("t", value<std::string>()->notifier(handle_t), "provide sharedConfig token for meeting");
    
    options_description all("Allowed Options");
    all.add(generic).add(session).add(hidden);
    
    options_description visible;
    visible.add(generic).add(session);
    
#ifdef _WIN32
    std::vector<std::string> pars_str = split_winmain(cmdline);
#else
    std::vector<std::string> pars_str = split_unix(cmdline);
#endif
    
    variables_map vm;
    
    try {
        store(command_line_parser(pars_str)
              .options(all)
              .style(
                     command_line_style::unix_style |
                     command_line_style::allow_long_disguise).run(),
              vm);
        
        notify(vm);
        
        if (vm.count("help")) {
            std::string product;
#ifdef FCC
            product = "FCC";
#else
            product = "StartMeeting";
#endif
            std::cout << "\nUSAGE: " << product << " [option]" << std::endl;
            std::cout << visible << std::endl;
            std::cout << "\n\nEXAMPLE:\n\t" << product << " --join 2-811-111 -n \"myName myLastName\"" << std::endl;
            
            return;
        }
        
    } catch (error &e) {
        std::cerr << e.what() << std::endl;
    }
    
    if (!validate_options(vm)) {
        _eErrorCode = eWrongCommand;
        _eLoginAction = eNone;
        return;
    }
    
    auto handle_host_studio = [this, &vm](ELoginAction action) {
        
        if (vm.count("s") || vm.count("s1")) {
            if (!_wstrLogin.empty() && !_wstrPass.empty()) {
                _eCommandType = PARS_HOST;
                _eLoginAction = action;
            }
        }
        
        if (vm.count("t")) {
            _eCommandType = PARS_HOST_TOKEN;
            _eLoginAction = action;
        }
        
        if (_eLoginAction == eNone && _eCommandType != PARS_LEAVE)
            _eCommandType = PARS_SHOW_LOGIN_FORM;
        
    };
    
    if (vm.count("host")) {
        handle_host_studio(eHost);
    }
    
    if (vm.count("studio")) {
        handle_host_studio(eStudio);
    }
    
    if (vm.count("join")) {
        _eLoginAction = eJoin;
        _eCommandType = PARS_JOIN;
        
        if (vm.count("s1") || vm.count("s") || vm.count("t") || !_wstrMeetingID.empty()) {
            _eErrorCode = eWrongCommand;
        }
    }
    
}
