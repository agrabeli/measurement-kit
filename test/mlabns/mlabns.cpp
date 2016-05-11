// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.

#define CATCH_CONFIG_MAIN
#include "src/ext/Catch/single_include/catch.hpp"

#include <measurement_kit/common.hpp>
#include <measurement_kit/mlabns.hpp>

using namespace mk;
/*
TEST_CASE("Query works as expected") {
    mlabns::Query query;
    query.address_family = "ipv4";
    query.metro = "trn";
    query.policy = "random";
    std::string tool = "neubot";
    loop_with_initial_event([&query, &tool]() {
        mlabns::query(
            tool, [](Error error, mlabns::Reply reply) {
                if (error) {
                    std::cout << "< error: " << (int)error << "\n";
                    break_loop();
                    return;
                }
                std::cout << "< city: " << reply.city << "\n";
                std::cout << "< url: " << reply.url << "\n";
                std::cout << "< ip: [\n";
                for (auto &s : reply.ip) {
                    std::cout << "<  " << s << "\n";
                }
                std::cout << "< ]\n";
                std::cout << "< fqdn: " << reply.fqdn << "\n";
                std::cout << "< site: " << reply.site << "\n";
                std::cout << "< country: " << reply.country << "\n";
                break_loop();
            }, {}, query);
    }); 
}
*/
TEST_CASE("The query pass the settings to the other levels") {
    mlabns::Query query;
    query.address_family = "ipv4";
    query.metro = "trn";
    query.policy = "random";
    std::string tool = "neubot";
    loop_with_initial_event([&query, &tool]() {
        mlabns::query(
            tool, [](Error error, mlabns::Reply reply) {
                REQUIRE(error);
            }, {{"net/timeout", 0.0000001}}, query);
    }); 
}
