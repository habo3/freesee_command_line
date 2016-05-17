
#include <boost/locale.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "test_parser.hpp"
#include "CommandLinePars.h"

#include <iostream>
#include <fstream>

int testcase_idx = 0;
int testcase_passed = 0;
int testcase_failed = 0;



class test_case {
public:
    
    test_case(boost::property_tree::ptree& node) {
        parse(node);
    }
    
    void parse(boost::property_tree::ptree& node) {
        this->node = node;
    }
    
    void execute() {
        ++testcase_idx;
        std::cout << "testcase " << testcase_idx;
        
        std::string input = node.get_child("input").data();
        bool should_fail = node.get<bool>("should_fail", false);
        
        bool disabled = node.get<bool>("disabled", false);
        if (disabled) {
            std::cout << ": running with `" << input << "`" << " is disabled" << std::endl;
            return;
        }
        
        std::cout << ": running with `" << input << "`" << std::endl;
        
        bool result = false;
        bool qc_result = false;
        
        try {
            TestParser test_parser;
            test_parser.load_json(node.get_child("result"));
            
            CommandLine parser;
            parser.parse_cmdline(input);
            
            std::wstring ws = boost::locale::conv::utf_to_utf<wchar_t>(input);
            
            CCommandLinePars parser2;
            parser2.ParseCommandLineString(ws);
            
            Tester tester;
            result = tester.compareTo(&parser2, &parser);
            
            {
                std::cout << "compared: (";
                qc_result = (test_parser == parser);
                std::cout << ")" << std::endl;
            }
            
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        
        std::cout << "\tOLD vs NEW: " << (result ? "EQUAL" : "NOT EQUAL") << std::endl;
        std::cout << "\tResult: " << (qc_result == !should_fail ? "SUCCEDED" : "!!! FAILED") << std::endl;
        
        if (qc_result == !should_fail)
            ++testcase_passed;
        else
            ++testcase_failed;
    }
    
protected:
    
protected:

    boost::property_tree::ptree node;
    
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
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(argv[1], pt);
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("tests"))
        {
            test_cases.push_back(v.second);
        }
        
    }
    
    // execute
    for (auto test : test_cases) {
        test.execute();
    }
    
    std::cout << "Results: failed " << testcase_failed << " test cases of " << (testcase_failed + testcase_passed) << std::endl;
}