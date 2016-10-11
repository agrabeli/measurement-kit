// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.

#include "../common/utils.hpp"
#include "../ooni/utils.hpp"
#include "../ooni/constants.hpp"

#include <set>
#include <cctype>
#include <algorithm>

#define BODY_PROPORTION_FACTOR 0.7

namespace mk {
namespace ooni {

using namespace mk::report;

typedef std::vector<std::pair<std::string, int>> SocketList;

static void compare_http_requests(Var<Entry> entry,
                                  Entry experiment_response,
                                  Entry control, Var<Logger> logger) {

  // XXX find a way to avoid this.
  std::string exp_body = experiment_response["body"];
  int exp_length = exp_body.size();
  int ctrl_length = control["body_length"];

  // Verifiy if the body lengths match a certain proportion factor.
  float body_proportion = 0;
  if (ctrl_length == exp_length) {
    body_proportion = 1;
  } else if (ctrl_length == 0 || exp_length == 0) {
    body_proportion = 0;
  } else {
    body_proportion = (float) std::min(ctrl_length, exp_length) /
                      (float) std::max(ctrl_length, exp_length);
  }
  logger->debug("web_connectivity: body proportion %f", body_proportion);

  if (body_proportion > BODY_PROPORTION_FACTOR) {
    (*entry)["body_length_match"] = true;
  } else {
    (*entry)["body_length_match"] = false;
  }

  // Verify that the status codes match
  logger->debug("web_connectivity: comparing status codes");

  // We ignore status code matching when the server returns an error in the
  // control.
  (*entry)["status_code_match"] = true;
  if (((int) control["status_code"])/100 != 5) {
    if (((int) control["status_code"]) == ((int) experiment_response["code"])) {
      (*entry)["status_code_match"] = true;
    } else {
      (*entry)["status_code_match"] = false;
    }
  }

  // Check if the headers match
  logger->debug("web_connectivity: checking headers");
  std::set<std::string> lowercase_ctrl_headers;
  std::set<std::string> lowercase_exp_headers;

  for (Entry::iterator it = control["headers"].begin();
       it != control["headers"].end(); ++it) {
    //lowercase_ctrl_headers.insert(std::tolower(it.key()));
    lowercase_ctrl_headers.insert(it.key());
  }
  for (Entry::iterator it = experiment_response["headers"].begin();
       it != experiment_response["headers"].end(); ++it) {
    //lowercase_exp_headers.insert(std::tolower(it.key()));
    lowercase_exp_headers.insert(it.key());
  }

  if (lowercase_ctrl_headers == lowercase_exp_headers) {
    (*entry)["headers_match"] = true;
  } else {
    std::set<std::string> intersection;
    std::set<std::string> uncommon_intersection;

    std::set_intersection(lowercase_exp_headers.begin(),
                          lowercase_exp_headers.end(),
                          lowercase_ctrl_headers.begin(),
                          lowercase_ctrl_headers.end(),
                          std::inserter(intersection,
                            intersection.begin()));

    std::set_difference(intersection.begin(),
                        intersection.end(),
                        constants::COMMON_SERVER_HEADERS.begin(),
                        constants::COMMON_SERVER_HEADERS.end(),
                        std::inserter(uncommon_intersection,
                          uncommon_intersection.begin()));

    if (uncommon_intersection.size() > 0) {
      (*entry)["headers_match"] = true;
    } else {
      (*entry)["headers_match"] = false;
    }
  }

  // Check if the HTML titles match
  logger->debug("web_connectivity: checking HTML titles");
  std::string experiment_title = extract_html_title(experiment_response["body"]);
  std::vector<std::string> exp_title_words;
  std::vector<std::string> ctrl_title_words;

  exp_title_words = split<std::vector<std::string>>(experiment_title, " ");
  ctrl_title_words = split<std::vector<std::string>>(control["title"], " ");
  int idx = 0;
  (*entry)["title_match"] = (experiment_title == control["title"]);
  for (auto exp_word : exp_title_words) {
    if (exp_word.length() < 5) {
      idx++;
      continue;
    }
    (*entry)["title_match"] = (bool) (exp_word == ctrl_title_words[idx]);
    break;
  }
}

static void compare_dns_queries(Var<Entry> entry,
                         std::vector<std::string> experiment_addresses,
                         Entry control, Settings options) {

  Var<Logger> logger = Logger::global();
  // When the controls fails in the same way as the experiment we consider the
  // DNS queries to be consistent.
  // XXX ensure the failure messages are aligned between ooniprobe and MK
  if ((*entry)["dns_experiment_failure"] != nullptr) {
    std::string exp_failure = (*entry)["dns_experiment_failure"];
    std::string ctrl_failure = control["failure"];
    if (exp_failure == ctrl_failure) {
      (*entry)["dns_consistency"] = "consistent";
    } else {
      (*entry)["dns_consistency"] = "inconsistent";
    }
    return;
  }

  std::set<std::string> exp_addresses(experiment_addresses.begin(),
                                      experiment_addresses.end());
  std::set<std::string> ctrl_addresses;
  for (std::string addr : control["addrs"]) {
    ctrl_addresses.insert(addr);
  }

  if (exp_addresses == ctrl_addresses) {
    (*entry)["dns_consistency"] = "consistent";
    return;
  }

  for (auto exp_addr : exp_addresses) {
    if (is_private_ipv4_addr(exp_addr) == true) {
      (*entry)["dns_consistency"] = "inconsistent";
      return;
    }
  }

  std::set<std::string> common_addresses;
  std::set_intersection(exp_addresses.begin(),
                        exp_addresses.end(),
                        ctrl_addresses.begin(),
                        ctrl_addresses.end(),
                        std::inserter(common_addresses,
                          common_addresses.begin()));

  if (common_addresses.size() > 0) {
      (*entry)["dns_consistency"] = "consistent";
      return;
  }

  std::set<std::string> exp_asns;
  std::set<std::string> ctrl_asns;

  std::string asn_p =
      options.get("geoip_asn_path", std::string{});
  logger->debug("Creating..");
  // TODO: perhaps here we could use GeoipCache::global()
  GeoipDatabase ip_location(asn_p);
  for (auto exp_addr : exp_addresses) {
    logger->debug("expaddr");
    ErrorOr<std::string> asn = ip_location.resolve_asn(exp_addr);
    if (asn && asn.as_value() != "AS0") {
      exp_asns.insert(asn.as_value());
    }
  }
  logger->debug("control");
  for (auto ctrl_addr : ctrl_addresses) {
    ErrorOr<std::string> asn = ip_location.resolve_asn(ctrl_addr);
    if (asn && asn.as_value() != "AS0") {
      ctrl_asns.insert(asn.as_value());
    }
  }
  std::set<std::string> common_asns;
  std::set_intersection(exp_asns.begin(),
                        exp_asns.end(),
                        ctrl_asns.begin(),
                        ctrl_asns.end(),
                        std::inserter(common_asns,
                          common_asns.begin()));

  if (common_asns.size() > 0) {
      (*entry)["dns_consistency"] = "consistent";
      return;
  }

  (*entry)["dns_consistency"] = "inconsistent";
}

static bool compare_tcp_connect(Var<Entry> entry, Entry control) {
  bool success = true;
  int idx = 0;
  for (auto result : (*entry)["tcp_connect"]) {
    bool ctrl_status;
    bool exp_status = result["status"]["success"];
    // XXX this is wasteful
    std::string ip = result["ip"];
    std::ostringstream ss;
    ss << ip;
    ss << ":";
    ss << result["port"];
    try {
      ctrl_status = control.at(ss.str())["status"];
    } catch (const std::out_of_range &) {
      continue;
    }
    if (ctrl_status == true && exp_status == false) {
      (*entry)["tcp_connect"][idx]["status"]["blocked"] = true;
      success = false;
    } else {
      (*entry)["tcp_connect"][idx]["status"]["blocked"] = false;
    }
    idx++;
  }
  return success;
}

static void compare_control_experiment(
    std::string input,
    Var<Entry> entry, std::vector<std::string> addresses,
    Settings options, Var<Logger> logger) {
    if ((*entry)["control_failure"] != nullptr) {
      logger->warn("web_connectivity: skipping control comparison due to failure");
      return;
    }

    logger->debug("web_connectivity: control is like %s",
                  (*entry)["control"].dump().c_str());

    if ((*entry)["http_experiment_failure"] == nullptr &&
        (*entry)["control"]["http_request"]["failure"] == nullptr) {
      logger->debug("web_connectivity: comparing http_requests");
      compare_http_requests(
              entry,
              (*entry)["requests"][0]["response"],
              (*entry)["control"]["http_request"],
              logger
      );
    }

    logger->debug("web_connectivity: comparing dns_queries");
    compare_dns_queries(
      entry,
      addresses,
      (*entry)["control"]["dns"],
      options
    );

    logger->debug("web_connectivity: comparing tcp_connect");
    bool tcp_connect_success = compare_tcp_connect(
        entry,
        (*entry)["control"]["tcp_connect"]
    );

    std::string exp_http_failure;
    std::string ctrl_http_failure;
    std::string dns_consistency = (*entry)["dns_consistency"];

    logger->debug("web_connectivity: exp,ctrl http failure determination");
    if ((*entry)["http_experiment_failure"] != nullptr) {
      exp_http_failure = split((*entry)["http_experiment_failure"]).front();
    }
    if ((*entry)["control"]["http_request"]["failure"] != nullptr) {
      ctrl_http_failure = split((*entry)["control"]["http_request"]["failure"]).front();
    }

    logger->debug("web_connectivity: checking if we got expected web page");
    logger->debug("%s", (*entry).dump().c_str());

    bool got_expected_web_page = false;
    if (exp_http_failure == "" &&
        ctrl_http_failure == "") {
      got_expected_web_page = (
        (((bool) (*entry)["body_length_match"]) == true ||
         ((bool) (*entry)["headers_match"]) == true ||
         ((bool) (*entry)["title_match"]) == true) &&
         ((bool) (*entry)["status_code_match"]) != false
      );
    }

    logger->debug("web_connectivity: determining blocking reason");

    if (dns_consistency == "consistent" &&
        tcp_connect_success == false &&
        exp_http_failure != "") {
      (*entry)["blocking"] = "tcp_ip";
    } else if (dns_consistency == "consistent" &&
        tcp_connect_success == true &&
        got_expected_web_page == false &&
        exp_http_failure == "" &&
        ctrl_http_failure == "") {
      (*entry)["blocking"] = "http-diff";
    } else if (dns_consistency == "consistent" &&
               tcp_connect_success == true &&
               exp_http_failure != "" &&
               ctrl_http_failure == "") {
      if (exp_http_failure == "dns_lookup_error") {
        (*entry)["blocking"] = "dns";
      } else {
        (*entry)["blocking"] = "http-failure";
      }
    } else if (dns_consistency == "inconsistent" &&
        got_expected_web_page == false) { // Note this slightly differs from the OONI logic
                                          // because the got_expected_page is more strict in MK
      (*entry)["blocking"] = "dns";
    } else if (dns_consistency == "consistent" &&
        got_expected_web_page == false &&
        (exp_http_failure == "" && ctrl_http_failure == "") &&
        ((*entry)["control"]["dns"]["failure"] != nullptr ||
         ctrl_http_failure != exp_http_failure)) {
      (*entry)["blocking"] = "dns";
    }
    if ((*entry)["blocking"] != nullptr) {
      std::string blocking = (*entry)["blocking"];
      logger->info("web_connectivity: BLOCKING detected due to: %s on %s",
                   blocking.c_str(), input.c_str());
    } else {
      logger->info("web_connectivity: no blocking detected");
    }
}

static void control_request(Var<Entry> entry,
        SocketList socket_list, std::string url,
        Callback<Error> callback,
        Settings options, Var<Logger> logger) {
    Settings request_settings;
    http::Headers headers;
    Entry request;
    request["tcp_connect"] = Entry::array();
    for (auto socket : socket_list) {
      // Formats the sockets as IP:PORT
      std::ostringstream ss;
      if (is_private_ipv4_addr(socket.first) == true) {
         continue;
      }
      ss << socket.first;
      ss << ":";
      ss << socket.second;
      request["tcp_connect"].push_back(ss.str());
    }
    request["http_request"] = url;
    std::string body = request.dump();

    request_settings["http/url"] = options["backend"];
    request_settings["http/method"] = "POST";
    headers["Content-Type"] = "application/json";

    if (options["backend/type"] == "cloudfront") {
      // TODO set the appropriate headers to support cloud-fronting.
    }

    logger->debug("web_connectivity: performing control request to %s",
                  options["backend"].c_str());
    http::request(request_settings, headers, body,
            [=](Error error, Var<http::Response> response) {
      if (!error) {
        try {
          (*entry)["control"] = Entry::parse(response->body);
          callback(NoError());
          return;
        } catch (const std::invalid_argument &) {
          (*entry)["control_failure"] = "json_parse_error";
          callback(JsonParseError());
          return;
        }
      }
      (*entry)["control_failure"] = error.as_ooni_error();
      callback(error);
      return;
    });
}

static void experiment_http_request(Var<Entry> entry,
        std::string url, Callback<Error> cb,
        Settings options,
        Var<Reactor> reactor, Var<Logger> logger) {

  http::Headers headers = constants::COMMON_CLIENT_HEADERS;
  std::string body;
  options["http/url"] = url;

  logger->debug("Requesting url %s", url.c_str());
  templates::http_request(entry, options, headers, body, [=](Error err,
              Var<http::Response> response) {
    // TODO: here we are ignoring the response... does it make sense?
    if (err) {
      (*entry)["http_experiment_failure"] = err.as_ooni_error();
      cb(err);
      return;
    }
    cb(NoError());
  }, reactor, logger);
}

static void experiment_tcp_connect(Var<Entry> entry,
        SocketList sockets, Callback<Error> cb,
        Var<Reactor> reactor, Var<Logger> logger) {

  int socket_count = sockets.size();
  Var<int> sockets_tested(new int(0));
  // XXX this is very ghetto
  if (socket_count == 0) {
    cb(NoError());
    return;
  }

  auto handle_connect = [=](std::string ip, int port) {
    return [=](Error err, Var<net::Transport> txp) {
      *sockets_tested += 1;
      bool close_txp = true;
      Entry result = {
        {"ip", ip},
        {"port", port},
        {"status", {
            {"success", nullptr},
            {"failure", nullptr},
            {"blocked", nullptr}
          }
        },
      };

      if (!!err) {
          logger->info("web_connectivity: failed to connect to %s:%d",
                       ip.c_str(), port);
          result["status"]["success"] = false;
          result["status"]["failure"] = err.as_ooni_error();
          close_txp = false;
      } else {
          logger->info("web_connectivity: success to connect to %s:%d",
                       ip.c_str(), port);
          result["status"]["success"] = true;
          result["status"]["blocked"] = false;
      }
      (*entry)["tcp_connect"].push_back(result);
      if (socket_count == *sockets_tested) {
        if (close_txp == true) {
          txp->close([=]{
            cb(NoError());
          });
        } else {
          cb(NoError());
        }
      } else {
        if (close_txp == true) {
          // XXX optimistic closure
          txp->close([=]{});
        }
      }
    };
  };

  for (auto socket : sockets) {
      std::string address = socket.first;
      int port = socket.second;
      Settings connect_options;
      connect_options["host"] = address;
      connect_options["port"] = port;
      connect_options["net/timeout"] = 10.0;
      templates::tcp_connect(connect_options,
                             handle_connect(address, port),
                             reactor, logger);
  }
}

static void experiment_dns_query(
        Var<Entry> entry,
        std::string hostname,
        std::string nameserver,
        Callback<Error, std::vector<std::string>> callback,
        Settings options,
        Var<Reactor> reactor, Var<Logger> logger) {

  if (is_ip_addr(hostname)) {
    // Don't perform DNS resolutions if it's an IP address
    std::vector<std::string> addresses;
    addresses.push_back(hostname);
    callback(NoError(), addresses);
    return;
  }

  templates::dns_query(entry, "A", "IN", hostname, nameserver,
        [=](Error err, Var<dns::Message> message) {
          logger->debug("web_connectivity: experiment_dns_query got response");
          std::vector<std::string> addresses;
          if (err) {
            callback(err, addresses);
            return;
          }
          for (auto answer : message->answers) {
            if (answer.ipv4 != "") {
              addresses.push_back(answer.ipv4);
            } else if (answer.hostname != "") {
              addresses.push_back(answer.ipv4);
            }
          }
          callback(NoError(), addresses);
        }, options, reactor, logger);
}

void web_connectivity(std::string input, Settings options,
                      Callback<Var<Entry>> callback,
                      Var<Reactor> reactor, Var<Logger> logger) {
    options["http/max_redirects"] = 20;
    Var<Entry> entry(new Entry);
    // This is set from ooni test
    // (*entry)["client_resolver"] = nullptr;
    (*entry)["retries"] = nullptr;

    (*entry)["dns_consistency"] = nullptr;
    (*entry)["body_length_match"] = nullptr;
    (*entry)["headers_match"] = nullptr;
    (*entry)["status_code_match"] = nullptr;
    (*entry)["title_match"] = nullptr;

    (*entry)["accessible"] = nullptr;
    (*entry)["blocking"] = nullptr;

    (*entry)["control_failure"] = nullptr;
    (*entry)["http_experiment_failure"] = nullptr;
    (*entry)["dns_experiment_failure"] = nullptr;

    (*entry)["tcp_connect"] = Entry::array();
    (*entry)["control"] = Entry({});

    ErrorOr<http::Url> url =
      mk::http::parse_url_noexcept(input);

    if (!url) {
      logger->debug("Invalid test url.");
      callback(entry);
      return;
    }

    std::string hostname = url->address;
    std::string nameserver = options["nameserver"];

    logger->info("web_connectivity: starting dns_query for %s", hostname.c_str());

    experiment_dns_query(entry, hostname, nameserver, [=](Error err,
              std::vector<std::string> addresses) {

      logger->info("web_connectivity: starting tcp_connect");

      SocketList socket_list;
      for (auto addr : addresses) {
        socket_list.push_back(std::make_pair(addr, url->port));
      }

      // TODO: is this correct here to ignore errors? Not changing the
      // code until it's clear it make sense to ignore them

      experiment_tcp_connect(entry, socket_list, [=](Error err){

        logger->info("web_connectivity: starting http_request to %s",
                     input.c_str());
        experiment_http_request(entry, input, [=](Error err){

          logger->info("web_connectivity: doing control request");
          control_request(entry, socket_list, input, [=](Error err) {

            logger->info("web_connectivity: comparing control with experiment");
            compare_control_experiment(input, entry, addresses, options, logger);
            callback(entry);

          }, options, logger); // end control_request

        }, options, reactor, logger); // end http_request

      }, reactor, logger); // end tcp_connect

    }, options, reactor, logger); // end dns_query
}

Var<NetTest> WebConnectivity::create_test_() {
  WebConnectivity *test = new WebConnectivity(input_filepath, options);
  test->logger = logger;
  test->reactor = reactor;
  test->output_filepath = output_filepath;
  test->entry_cb = entry_cb;
  test->begin_cb = begin_cb;
  test->end_cb = end_cb;
  return Var<NetTest>(test);
}

} // namespace ooni
} // namespace mk
