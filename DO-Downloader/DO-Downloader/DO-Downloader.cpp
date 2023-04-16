// DO-Downloader.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>
#include <deliveryoptimization.h>
#include <deliveryoptimizationerrors.h>
#include "do-client/download_impl.h"
#include "do-client/do_download.h"
#include <boost/filesystem.hpp>
#include <chrono>
#include "do-client/test_helpers.h"
#include <combaseapi.h>




namespace msdo = microsoft::deliveryoptimization;
namespace msdod = microsoft::deliveryoptimization::details;
namespace msdot = microsoft::deliveryoptimization::test;
using namespace std;

//std::string URL = "http://dl.delivery.mp.microsoft.com/filestreamingservice/files/52fa8751-747d-479d-8f22-e32730cc0eb1"; //MCC aware link

std::chrono::seconds Timeout = 10h;

static std::unique_ptr<msdo::download> g_MakeDownload(const std::string& url, const std::string& destPath)
{
    std::unique_ptr<msdo::download> downloadObj;
    auto ec = msdo::download::make(url, destPath, downloadObj);
    if (ec) throw std::exception();

    return downloadObj;
}

template <typename T>
static msdo::download_property_value g_MakePropertyValue(T value)
{
    msdo::download_property_value propValue;
    auto ec = msdo::download_property_value::make(value, propValue);
    if (ec)
    {
        throw std::exception();
    }
    return propValue;
}

class InputParser {
public:
    InputParser(int& argc, char** argv) {
        for (int i = 1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
    }
    /// @author iain
    const std::string& getCmdOption(const std::string& option) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }
    /// @author iain
    bool cmdOptionExists(const std::string& option) const {
        return std::find(this->tokens.begin(), this->tokens.end(), option)
            != this->tokens.end();
    }
private:
    std::vector <std::string> tokens;
};


int main(int argc, char** argv)
{
    //Get Command Line Parameters

    InputParser input(argc, argv);
    
    std::string Target = input.getCmdOption("-Target");
    std::string URL = input.getCmdOption("-URL");
    if (!Target.empty() && !URL.empty()) {

        if (input.cmdOptionExists("-s")) {

        }
        else
        {
            //Show Logo and Text
            std::cout << R"(

  _____   ____             _____                      _                 _           
 |  __ \ / __ \           |  __ \                    | |               | |          
 | |  | | |  | |  ______  | |  | | _____      ___ __ | | ___   __ _  __| | ___ _ __ 
 | |  | | |  | | |______| | |  | |/ _ \ \ /\ / / '_ \| |/ _ \ / _` |/ _` |/ _ \ '__|
 | |__| | |__| |          | |__| | (_) \ V  V /| | | | | (_) | (_| | (_| |  __/ |   
 |_____/ \____/           |_____/ \___/ \_/\_/ |_| |_|_|\___/ \__,_|\__,_|\___|_|   
                                                                                                                         
                                                                                                                     
 )" << '\n';
            cout << endl << "Downloading File from following URL: " << URL << endl;
            cout << endl << "Saving File to following Location: " << Target << endl;

        }

        //Initializing

        const auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr))
        {
            std::cout << "CoInitializeEx failed with " << hr << std::endl;
            return hr;
        }

        //Checking if File already exists

        if (!boost::filesystem::exists(Target))
        {

            //Starting actual DO Downlaod

            auto simpleDownload = msdot::download::make(URL, Target);

            msdo::download_property_value foregroundPriority = g_MakePropertyValue(true);
            simpleDownload->set_property(msdo::download_property::use_foreground_priority, foregroundPriority),1;
            
            msdo::download_status status = simpleDownload->get_status();
            simpleDownload->start();
            TestHelpers::WaitForState(*simpleDownload, msdo::download_state::transferred, Timeout);
            status = simpleDownload->get_status();
            simpleDownload->finalize();


        }
        else
        {
            std::cout << endl << "File already exists, skip downlaod!" << std::endl;
        }

    }
    else
    {
        if (input.cmdOptionExists("-s")) {

        }
        else
        {
            //Show Logo and Text
            std::cout << R"(

  _____   ____             _____                      _                 _           
 |  __ \ / __ \           |  __ \                    | |               | |          
 | |  | | |  | |  ______  | |  | | _____      ___ __ | | ___   __ _  __| | ___ _ __ 
 | |  | | |  | | |______| | |  | |/ _ \ \ /\ / / '_ \| |/ _ \ / _` |/ _` |/ _ \ '__|
 | |__| | |__| |          | |__| | (_) \ V  V /| | | | | (_) | (_| | (_| |  __/ |   
 |_____/ \____/           |_____/ \___/ \_/\_/ |_| |_|_|\___/ \__,_|\__,_|\___|_|   
                                                                                                                           
                                                                                                                     
 )" << '\n';
            cout << endl << "Invalid Commandline Parameters" << endl << "Reqired -URL and -Target, -s is optional to hide Logo" << endl << "(Parameters are case-sensitive!)" << endl << "working expample: DO-Downloader.exe -Target \"C:\\01_DATA\\1.test\" -URL \"http://dl.delivery.mp.microsoft.com/filestreamingservice/files/52fa8751-747d-479d-8f22-e32730cc0eb1\"" <<endl;
        }
    }

}

