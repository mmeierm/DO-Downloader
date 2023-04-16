// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _DELIVERY_OPTIMIZATION_TEST_HELPERS_H
#define _DELIVERY_OPTIMIZATION_TEST_HELPERS_H

#include <string>
#include <thread>


#include "do_download.h"
#include "do_download_status.h"
#include "do_error_helpers.h"
#include "do_test_helpers.h"

namespace msdo = microsoft::deliveryoptimization;
namespace msdod = microsoft::deliveryoptimization::details;

namespace microsoft
{
namespace deliveryoptimization
{
namespace test
{

// Shim class to convert error-return API of the SDK download class to exception-throwing API.
// Avoids the need to rewrite tests to check the return code of each API.
class download
{
public:
    static std::unique_ptr<download> make(const std::string& uri, const std::string& downloadFilePath)
    {
        std::unique_ptr<msdo::download> newDownload;
        std::error_code ec = msdo::download::make(uri, downloadFilePath, newDownload);
        
        auto returnVal = std::make_unique<download>(std::move(newDownload));
        return returnVal;
    }

    download(std::unique_ptr<msdo::download>&& downloadImpl) :
        _downloadImpl(std::move(downloadImpl))
    {
    }

    void start()
    {
        std::error_code ec = _downloadImpl->start();
        
    }
    void pause()
    {
        std::error_code ec = _downloadImpl->pause();
        
    }
    void resume()
    {
        std::error_code ec = _downloadImpl->resume();
        
    }
    void finalize()
    {
        std::error_code ec = _downloadImpl->finalize();
        
    }
    void abort()
    {
        std::error_code ec = _downloadImpl->abort();
        
    }
    msdo::download_status get_status() const
    {
        msdo::download_status status;
        std::error_code ec = _downloadImpl->get_status(status);
        
        return status;
    }

    void start_and_wait_until_completion(std::chrono::seconds timeoutSecs = std::chrono::hours(24))
    {
        std::error_code ec = _downloadImpl->start_and_wait_until_completion(timeoutSecs);
        
    }
    void start_and_wait_until_completion(const std::atomic_bool& isCancelled, std::chrono::seconds timeoutSecs = std::chrono::hours(24))
    {
        std::error_code ec = _downloadImpl->start_and_wait_until_completion(isCancelled, timeoutSecs);
        
    }

    static void download_url_to_path(const std::string& uri, const std::string& downloadFilePath,
        std::chrono::seconds timeoutSecs = std::chrono::hours(24))
    {
        std::error_code ec = msdo::download::download_url_to_path(uri, downloadFilePath, timeoutSecs);
        
    }
    static void download_url_to_path(const std::string& uri, const std::string& downloadFilePath, const std::atomic_bool& isCancelled,
        std::chrono::seconds timeoutSecs = std::chrono::hours(24))
    {
        std::error_code ec = msdo::download::download_url_to_path(uri, downloadFilePath, isCancelled, timeoutSecs);
        
    }

    /*
    For devices running windows before 20H1, dosvc exposed a now-deprecated com interface for setting certain download properties.
    After 20H1, these properties were added to newer com interface, which this SDK is using.
    Attempting to set a download property on a version of windows earlier than 20H1 will not set the property and throw an exception with error code msdo::errc::do_e_unknown_property_id
    */
    void set_property(msdo::download_property key, const msdo::download_property_value& value)
    {
        std::error_code ec = _downloadImpl->set_property(key, value);
        
    }
    msdo::download_property_value get_property(msdo::download_property key)
    {
        msdo::download_property_value propValue;
        std::error_code ec = _downloadImpl->get_property(key, propValue);
        
        return propValue;
    }

private:
    std::unique_ptr<msdo::download> _downloadImpl;
};

}
}
}

class TestHelpers
{
public:


    // DoSvc creates temporary files with a unique name in the same directory as the output file
    static void DeleteDoSvcTemporaryFiles(const boost::filesystem::path& outputFilePath)
    {
        const boost::filesystem::path parentDir = outputFilePath.parent_path();
        for (boost::filesystem::directory_iterator itr(parentDir); itr != boost::filesystem::directory_iterator(); ++itr)
        {
            const boost::filesystem::directory_entry& dirEntry = *itr;
            // Remove all files with names that match DO*.tmp
            if (boost::filesystem::is_regular_file(dirEntry)
                && (dirEntry.path().filename().string().find("DO") == 0)
                && (dirEntry.path().extension() == ".tmp"))
            {
                boost::system::error_code ec;
                boost::filesystem::remove(dirEntry, ec);
                if (ec)
                {
                    std::cout << "Temp file deletion error: " << ec.message() << ", " << dirEntry.path() << '\n';
                }
                else
                {
                    std::cout << "Deleted DoSvc temp file: " << dirEntry.path() << '\n';
                }
            }
        }
    }

    // On Windows, operations are async - there may be some delay setting a state internally
    static void WaitForState(microsoft::deliveryoptimization::test::download& download, msdo::download_state expectedState,
        std::chrono::seconds waitTimeSecs = std::chrono::seconds{10})
    {
        msdo::download_status status = download.get_status();
        const auto endtime = std::chrono::steady_clock::now() + waitTimeSecs;
        while ((status.state() != expectedState) && (std::chrono::steady_clock::now() < endtime))
        {
            std::this_thread::sleep_for(std::chrono::seconds{1});
            status = download.get_status();
            std::cout << "Transferred " << status.bytes_transferred() << " / " << status.bytes_total() << "\n";
        }

        if (status.state() != expectedState)
        {
            // Throw exception instead of ASSERT* to let tests catch if needed
            const auto msg = dotest::util::FormatString("State: expected = %d, actual = %d", expectedState, status.state());
            throw std::runtime_error(msg);
        }
    }



private:

    // Disallow creating an instance of this object
    TestHelpers() {}

};

#endif // _DELIVERY_OPTIMIZATION_TEST_HELPERS_H
