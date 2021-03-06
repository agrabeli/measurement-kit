// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.

#include "../ooni/utils_impl.hpp"
#include "../common/utils.hpp"

namespace mk {
namespace ooni {

void ip_lookup(Callback<Error, std::string> callback, Settings settings,
               Var<Reactor> reactor, Var<Logger> logger) {
    ip_lookup_impl(callback, settings, reactor, logger);
}

void resolver_lookup(Callback<Error, std::string> callback, Settings settings,
                     Var<Reactor> reactor, Var<Logger> logger) {
    resolver_lookup_impl(callback, settings, reactor, logger);
}

/* static */ Var<GeoipCache> GeoipCache::global() {
    static Var<GeoipCache> singleton(new GeoipCache);
    return singleton;
}

Var<GeoipDatabase> GeoipCache::get(std::string path, bool &did_open) {
    if (instances.find(path) != instances.end()) {
        did_open = false;
        return instances.at(path);
    }
    did_open = true;
    if (instances.size() > max_size) {
        instances.erase(std::prev(instances.end()));
    }
    instances[path] = Var<GeoipDatabase>(new GeoipDatabase(path));
    return instances[path];
}

ErrorOr<std::string> GeoipDatabase::with_open_database_do(
        std::function<ErrorOr<std::string>()> action,
        Var<Logger> logger) {
    if (!db) {
        db.reset(GeoIP_open(path.c_str(), GEOIP_MEMORY_CACHE),
                 [](GeoIP *pointer) {
                    if (pointer) {
                        GeoIP_delete(pointer);
                    }
                 });
        if (!db) {
            logger->warn("cannot open geoip database: %s", path.c_str());
            return GeoipDatabaseOpenError();
        }
        // FALLTHROUGH
    }
    return action();
}

ErrorOr<std::string> GeoipDatabase::resolve_country_code(
            std::string ip, Var<Logger> logger) {
    return with_open_database_do([=]() -> ErrorOr<std::string> {
        GeoIPLookup gl;
        memset(&gl, 0, sizeof(gl));
        const char *result;
        result = GeoIP_country_code_by_name_gl(db.get(), ip.c_str(), &gl);
        if (result == nullptr) {
            return GeoipCountryCodeLookupError();
        }
        std::string country_code = result;
        return country_code;
    }, logger);
}

ErrorOr<std::string> GeoipDatabase::resolve_country_name(
            std::string ip, Var<Logger> logger) {
    return with_open_database_do([=]() -> ErrorOr<std::string> {
        GeoIPLookup gl;
        memset(&gl, 0, sizeof(gl));
        const char *result;
        result = GeoIP_country_name_by_name_gl(db.get(), ip.c_str(), &gl);
        if (result == nullptr) {
            return GeoipCountryNameLookupError();
        }
        std::string country_name = result;
        return country_name;
    }, logger);
}

ErrorOr<std::string> GeoipDatabase::resolve_city_name(
            std::string ip, Var<Logger> logger) {
    return with_open_database_do([=]() -> ErrorOr<std::string> {
        GeoIPRecord *gir = GeoIP_record_by_name(db.get(), ip.c_str());
        if (gir == nullptr) {
            return GeoipCityLookupError();
        }
        std::string result;
        if (gir->city != nullptr) {
            result = gir->city;
        }
        GeoIPRecord_delete(gir);
        return result;
    }, logger);
}

ErrorOr<std::string> GeoipDatabase::resolve_asn(std::string ip,
                                             Var<Logger> logger) {
    return with_open_database_do([=]() -> ErrorOr<std::string> {
        GeoIPLookup gl;
        memset(&gl, 0, sizeof(gl));
        char *res = GeoIP_name_by_name_gl(db.get(), ip.c_str(), &gl);
        if (res == nullptr) {
            return GeoipAsnLookupError();
        }
        std::string asn = res;
        asn = split(asn).front(); // We only want ASXX
        free(res);
        return asn;
    }, logger);
}

std::string extract_html_title(std::string body) {
  std::regex TITLE_REGEXP("<title>([\\s\\S]*?)</title>", std::regex::icase);
  std::smatch match;

  if (std::regex_search(body, match, TITLE_REGEXP) && match.size() > 1) {
    return match.str(1);
  }
  return "";
}

bool is_private_ipv4_addr(const std::string &ipv4_addr) {
  std::regex IPV4_PRIV_ADDR(
      "(^127\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})|"
      "(^192\\.168\\.[0-9]{1,3}\\.[0-9]{1,3})|"
      "(^10\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})|"
      "(^172\\.1[6-9]\\.[0-9]{1,3}\\.[0-9]{1,3})|"
      "(^172\\.2[0-9]\\.[0-9]{1,3}\\.[0-9]{1,3})|"
      "(^172\\.3[0-1]\\.[0-9]{1,3}\\.[0-9]{1,3})|"
      "localhost"
  );
  std::smatch match;

  if (std::regex_search(ipv4_addr, match, IPV4_PRIV_ADDR) && match.size() > 1) {
    return true;
  }
  return false;
}

bool is_ip_addr(const std::string &ip_addr) {
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;
    if ((inet_pton(AF_INET, ip_addr.c_str(), &(sa.sin_addr)) == 1) ||
        (inet_pton(AF_INET6, ip_addr.c_str(), &(sa6.sin6_addr)) == 1)) {
        return true;
    }
    return false;
}

} // namespace ooni
} // namespace mk
