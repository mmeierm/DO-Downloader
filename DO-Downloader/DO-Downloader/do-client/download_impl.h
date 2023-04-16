// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.


#define _DELIVERY_OPTIMIZATION_DOWNLOAD_IMPL_H

#include <string>

#include "download_interface.h"
#include "do_download_property.h"
#include "do_download_status.h"
#include "do_errors.h"
#include <wrl.h>
#include <deliveryoptimization.h>

namespace microsoft
{
namespace deliveryoptimization
{
namespace details
{
class CDownloadImpl : public IDownload
{
public:
    CDownloadImpl() = default;

    std::error_code Init(const std::string& uri, const std::string& downloadFilePath) noexcept override;

    std::error_code Start() noexcept override;
    std::error_code Pause() noexcept override;
    std::error_code Resume() noexcept override;
    std::error_code Finalize() noexcept override;
    std::error_code Abort() noexcept override;

    std::error_code GetStatus(download_status& status) noexcept override;
    std::error_code GetProperty(download_property key, download_property_value& value) noexcept override;
    std::error_code SetProperty(download_property key, const download_property_value& val) noexcept override;
    std::error_code SetCallback(const download_property_value::status_callback_t& callback, download& download) noexcept override;

private:


    static std::error_code _SetPropertyHelper(IDODownload& download, download_property key, const download_property_value& val) noexcept;
    std::error_code _GetPropertyHelper(download_property key, download_property_value& value) noexcept;

    Microsoft::WRL::ComPtr<IDODownload> _spDownload;

};

} // namespace details
} // namespace deliveryoptimization
} // namespace microsoft
