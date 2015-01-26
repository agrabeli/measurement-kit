#ifndef LIBIGHT_OONI_HTTP_TEST_HPP
# define LIBIGHT_OONI_HTTP_TEST_HPP

#include "common/settings.hpp"
#include "protocols/http.hpp"
#include "ooni/net_test.hpp"

namespace ight {
namespace ooni {
namespace http_test {

class HTTPTest : public net_test::NetTest {
    using net_test::NetTest::NetTest;

    protocols::http::Client http_client;

public:
    HTTPTest(std::string input_filepath_, ight::common::Settings options_) : 
      net_test::NetTest(input_filepath_, options_) {
        test_name = "tcp_test";
        test_version = "0.0.1";
    };

    HTTPTest(ight::common::Settings options_) : 
      HTTPTest("", options_)  {};


    void request(ight::common::Settings settings, protocols::http::Headers headers,
                 std::string body,
                 protocols::http::RequestCallback&& callback) {

        http_client.request(settings, headers, body,
                            [=](IghtError error, protocols::http::Response&& response) {

            YAML::Node rr;
            rr["request"]["headers"] = std::map<std::string, std::string>(headers);
            rr["request"]["body"] = body;
            rr["request"]["url"] = settings.at("url");
            rr["request"]["http_version"] = settings.at("http_version");
            rr["request"]["method"] = settings.at("method");

            // XXX we should probably update the OONI data format to remove
            // this.
            rr["method"] = settings.at("method");

            if (error.error == 0) {
                rr["response"]["headers"] = std::map<std::string, std::string>(response.headers);
                rr["response"]["body"] = response.body.read<char>();
                rr["response"]["response_line"] = response.response_line;
                rr["response"]["code"] = response.status_code;
            } else {
                rr["failure"] = "unknown_failure ight_error";
                rr["error_code"] = error.error;
            }

            entry["requests"].push_back(rr);
            entry["agent"] = "agent";
            entry["socksproxy"] = "";
            callback(error, std::move(response));
        });
    };
    
};

}}}

#endif
