// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.
#ifndef MEASUREMENT_KIT_COMMON_RUNNER_HPP
#define MEASUREMENT_KIT_COMMON_RUNNER_HPP

#include <measurement_kit/common/reactor.hpp>

#include <atomic>
#include <string>
#include <thread>

namespace mk {

class NetTest;

class Runner {
  public:
    Runner();
    void run_test(Var<NetTest> test, Callback<Var<NetTest>> func);
    void break_loop_();
    bool empty();
    void join_();
    ~Runner();
    static Var<Runner> global();

  private:
    std::atomic<int> active{0};
    Var<Reactor> reactor = Reactor::global();
    std::atomic<bool> running{false};
    std::thread thread;
};

} // namespace mk
#endif
