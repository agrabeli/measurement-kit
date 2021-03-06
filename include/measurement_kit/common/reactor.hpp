// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.
#ifndef MEASUREMENT_KIT_COMMON_REACTOR_HPP
#define MEASUREMENT_KIT_COMMON_REACTOR_HPP

#include <measurement_kit/common/callback.hpp>
#include <measurement_kit/common/var.hpp>

struct event_base;

namespace mk {

class Reactor {
  public:
    static Var<Reactor> make();
    virtual ~Reactor() {}

    virtual void call_soon(Callback<> cb) = 0;
    virtual void call_later(double, Callback<> cb) = 0;

    virtual void loop_with_initial_event(Callback<> cb) = 0;
    virtual void loop() = 0;
    virtual void loop_once() = 0;
    virtual void break_loop() = 0;

    virtual event_base *get_event_base() = 0;

    static Var<Reactor> global() {
        static Var<Reactor> singleton = make();
        return singleton;
    }
};

// This function is just a wrapper for `reactor->call_soon` but is useful
// in the regress tests because it can be passed as a template argument
void reactor_call_soon(Var<Reactor> reactor, Callback<> cb);

inline void call_soon(Callback<> cb) {
    Reactor::global()->call_soon(cb);
}

inline void call_later(double t, Callback<> cb) {
    Reactor::global()->call_later(t, cb);
}

inline void loop_with_initial_event(Callback<> cb) {
    Reactor::global()->loop_with_initial_event(cb);
}

void loop_with_initial_event_and_connectivity(Callback<> cb);

inline void loop() { Reactor::global()->loop(); }

inline void loop_once() { Reactor::global()->loop_once(); }

inline void break_loop() { Reactor::global()->break_loop(); }

} // namespace mk
#endif
