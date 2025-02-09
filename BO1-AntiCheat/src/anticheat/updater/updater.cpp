#include "updater.hpp"

#include <cpprest/http_client.h>
#include <cpprest/json.h>

#include <iostream>
#include <windows.h>
#include <shellapi.h>
#include <future>

#include "../../constants.h"

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace std;

namespace anticheat {
    namespace updater {

        std::string GetAvailableUpdates() {
            // Promise to return the result synchronously
            std::promise<std::string> updatePromise;
            auto updateFuture = updatePromise.get_future();

            try {
                // Configure the HTTP client with secure settings
                http_client_config config;
                config.set_validate_certificates(true);

                uri_builder builder(U("https://api.github.com/repos/BlackOpsOne/BO1-AntiCheat/releases/latest"));
                http_client client(builder.to_uri(), config);
                http_request request(methods::GET);

                // Handle the request asynchronously and fulfill the promise once
                client.request(request).then([&updatePromise](http_response response) {
                    if (response.status_code() == status_codes::OK) {
                        return response.extract_json();
                    }
                    else {
                        updatePromise.set_value("");
                        throw std::runtime_error("Failed to get updates");
                    }
                }).then([&updatePromise](pplx::task<json::value> jsonTask) {
                    try {
                        json::value json_response = jsonTask.get();

                        if (json_response.has_field(U("tag_name"))) {
                            std::string tag_name = utility::conversions::to_utf8string(json_response[U("tag_name")].as_string());
                            std::string description = utility::conversions::to_utf8string(json_response[U("body")].as_string());
                            if (tag_name != Constants::VERSION) {
                                std::string available_updates = description;
                                updatePromise.set_value("v" + tag_name + ":\n" + available_updates);
                                return;
                            }
                        }
                        updatePromise.set_value("");
                    }
                    catch (const std::exception& e) {
                        // Catch any error that occurred while processing the JSON
                        updatePromise.set_value("");
                        throw;
                    }
                }).then([&updatePromise](pplx::task<void> finalTask) {
                    try {
                        finalTask.get();
                    }
                    catch (const std::exception&) {
                        updatePromise.set_value("");
                        throw;
                    }
                }).wait();

                return updateFuture.get();
            }
            catch (const std::exception&) {
                MessageBoxA(NULL, Statuses::COULDNT_CHECK_UPDATES.c_str(), "Error", MB_OK);
                return ""; // Return empty std::string in case of failure
            }
        }

    } // updater
} // anticheat