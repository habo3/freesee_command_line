
#include <boost/locale.hpp>

#include "new_cmd_parser.hpp"
#include "CommandLinePars.h"

#include <iostream>
#include <fstream>

int testcase_idx = 0;

class test_case {
public:
    
    test_case(const std::string& raw) : execute_type(compare_qc) {
        parse(raw);
    }
    
    void parse(const std::string& in) {
        char str[256] = {'\0'};
        sscanf(in.c_str(), "%[^,],%d", str, &expected_result);
        input_params = str;
    }
    
    void execute() {
        ++testcase_idx;
        std::cout << "testcase " << testcase_idx;
        
        std::cout << ": running with `" << input_params << "`";
        
        std::cout << std::endl;
        
        CommandLine parser;
        parser.parse_cmdline(input_params);
        
        std::wstring ws = boost::locale::conv::utf_to_utf<wchar_t>(input_params);
        
        CCommandLinePars parser2;
        parser2.ParseCommandLineString(ws);
        
        bool result = false;
        try {
            Tester tester;
            tester.compareTo(&parser2, &parser)
        } catch (std::exce)
        
        std::cout << ( tester.compareTo(&parser2, &parser) ? "SUCCEDED" : "!!! FAILED") << std::endl;
    }
    
protected:
    
protected:
    std::string input_params;
    int expected_result;
    
    enum {
        compare_parsers,
        compare_qc,
    } execute_type;
};

int main(int argc, const char *argv[])
{
    std::vector<test_case> test_cases;
    
    std::string test_file;
    if (argc == 2) {
        std::stringstream ss;
        ss << argv[1];
        test_file = ss.str();
        
        std::ifstream myfile;
        myfile.open(test_file, std::ios::in);
        if (!myfile.is_open())
            return 1;
        
        std::string line;
        while (getline(myfile, line)) {
            test_cases.push_back(line);
        }
    } else {
        
        const char * default_tests[] = {
            "--help, 1",
            "-u 10, 1",
            "-u 4, 0", // failing
            "--host -s email passw -u 10, 1",
            "-u 10 --host -s email passw, 1",
            
            "--join --host, 1",
            "--host -s email passw, 1",
            "--studio -s email passw, 1",
            "--join meetingid -n \"1st 2nd\", 1",
            "--join -n \"1st 2nd\" -b, 0", // failing
            "--host -s1 bmFtZSBwYXNzd29yZA== -n some_name, 1",
            
            "qa-freeconferencecall://-j -s1 bmFtZSBwYXNzd29yZA==, 1",
            "qa-freeconferencecall://-s1 bmFtZSBwYXNzd29yZA==, 1",
            "qa-freeconferencecall://--host -s1 bmFtZSBwYXNzd29yZA==, 0", // failing
            
            "--join -r 123 456, 1",
            "--join -r asd 456, 1",
        };
        
        for (auto i = 0; i < sizeof(default_tests)/sizeof(void*); ++i) {
            test_case t(default_tests[i]);
            test_cases.push_back(t);
        }
    }
    
    // execute
    for (auto test : test_cases) {
        test.execute();
    }
}