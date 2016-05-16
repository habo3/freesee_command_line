//
//  new_cmd_parser.hpp
//  boost_program_options
//
//  Created by Oleksandr Skrypnyk on 12/05/16.
//  Copyright © 2016 Oleksandr Skrypnyk. All rights reserved.
//

#ifndef new_cmd_parser_hpp
#define new_cmd_parser_hpp

#include <string>
#include "ParserProtocol.hpp"

class CommandLine : public AbstractParser<char> {
    
public:
    void parse_cmdline(int argc, const char *argv[]);
    void parse_cmdline(const std::string& cmdline);
};

#endif /* new_cmd_parser_hpp */
