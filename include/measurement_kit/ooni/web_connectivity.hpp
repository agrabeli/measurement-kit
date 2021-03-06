// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.
#ifndef MEASUREMENT_KIT_OONI_WEB_CONNECTIVITY_HPP
#define MEASUREMENT_KIT_OONI_WEB_CONNECTIVITY_HPP

#include <measurement_kit/ooni/ooni_test.hpp>
#include <measurement_kit/report.hpp>

namespace mk {
namespace ooni {

using namespace mk::report;

void web_connectivity(std::string input, Settings,
                      Callback<Var<Entry>>,
                      Var<Reactor> = Reactor::global(),
                      Var<Logger> = Logger::global());

class WebConnectivity : public OoniTest {
  public:
    WebConnectivity() : WebConnectivity("", {}) {}

    WebConnectivity(std::string f, Settings o) : OoniTest(f, o) {
        test_name = "web_connectivity";
        test_version = "0.0.1";
        needs_input = true;
    }

    void main(std::string input, Settings options,
              Callback<Entry> cb) override {
        web_connectivity(input, options, [=](Var<Entry> e) {
             cb(*e);
        }, reactor, logger);
    }

    Var<NetTest> create_test_() override;
};

} // namespace ooni
} // namespace mk
#endif
