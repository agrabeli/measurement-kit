// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.
#ifndef SRC_LIBMEASUREMENT_KIT_NDT_PROTOCOL_IMPL_HPP
#define SRC_LIBMEASUREMENT_KIT_NDT_PROTOCOL_IMPL_HPP

#include "../ndt/internal.hpp"

namespace mk {
namespace ndt {
namespace protocol {

template <MK_MOCK_NAMESPACE(net, connect)>
void connect_impl(Var<Context> ctx, Callback<Error> callback) {
    ctx->logger->debug("ndt: connect ...");
    net_connect(ctx->address, ctx->port,
                [=](Error err, Var<Transport> txp) {
                    ctx->logger->debug("ndt: connect ... %d", (int)err);
                    if (err) {
                        callback(ConnectControlConnectionError(err));
                        return;
                    }
                    txp->set_timeout(ctx->timeout);
                    ctx->txp = txp;
                    ctx->logger->info("Connected to %s:%d",
                                      ctx->address.c_str(), ctx->port);
                    callback(NoError());
                },
                ctx->settings, ctx->logger, ctx->reactor);
}

template <MK_MOCK_NAMESPACE(messages, format_msg_extended_login),
          MK_MOCK_NAMESPACE(messages, write)>
void send_extended_login_impl(Var<Context> ctx, Callback<Error> callback) {
    ctx->logger->debug("ndt: send login ...");
    ErrorOr<Buffer> out = messages_format_msg_extended_login(ctx->test_suite);
    if (!out) {
        ctx->logger->debug("ndt: send login ... %d", (int)out.as_error());
        callback(FormatExtendedLoginMessageError(out.as_error()));
        return;
    }
    messages_write(ctx, *out, [=](Error err) {
        ctx->logger->debug("ndt: send login ... %d", (int)err);
        if (err) {
            callback(WriteExtendedLoginMessageError(err));
            return;
        }
        ctx->logger->info("Sent LOGIN with test suite: %d", ctx->test_suite);
        callback(NoError());
    });
}

template <MK_MOCK_NAMESPACE(net, readn)>
void recv_and_ignore_kickoff_impl(Var<Context> ctx, Callback<Error> callback) {
    ctx->logger->debug("ndt: recv and ignore kickoff ...");
    net_readn(ctx->txp, ctx->buff, KICKOFF_MESSAGE_SIZE, [=](Error err) {
        ctx->logger->debug("ndt: recv and ignore kickoff ... %d", (int)err);
        if (err) {
            callback(ReadingKickoffMessageError(err));
            return;
        }
        std::string s = ctx->buff->readn(KICKOFF_MESSAGE_SIZE);
        if (s != KICKOFF_MESSAGE) {
            callback(InvalidKickoffMessageError());
            return;
        }
        ctx->logger->info("Got legacy KICKOFF message (ignored)");
        callback(NoError());
    }, ctx->reactor);
}

template <MK_MOCK_NAMESPACE(messages, read_msg),
          MK_MOCK_NAMESPACE(messages, format_msg_waiting),
          MK_MOCK_NAMESPACE(messages, write_noasync),
          MK_MOCK(reactor_call_soon)>
void wait_in_queue_impl(Var<Context> ctx, Callback<Error> callback) {
    ctx->logger->debug("ndt: wait in queue ...");
    messages_read_msg(ctx, [=](Error err, uint8_t type, std::string s) {
        ctx->logger->debug("ndt: wait in queue ... %d", (int)err);
        if (err) {
            callback(ReadingSrvQueueMessageError(err));
            return;
        }
        if (type != SRV_QUEUE) {
            callback(NotSrvQueueMessageError());
            return;
        }
        ErrorOr<unsigned> wait_time = lexical_cast_noexcept<unsigned>(s);
        if (!wait_time) {
            callback(InvalidSrvQueueMessageError(wait_time.as_error()));
            return;
        }
        if (*wait_time > 0) {
            if (*wait_time == SRV_QUEUE_SERVER_FAULT) {
                ctx->logger->warn("Server fault; aborting test");
                callback(QueueServerFaultError());
                return;
            }
            if (*wait_time == SRV_QUEUE_SERVER_BUSY or
                *wait_time == SRV_QUEUE_SERVER_BUSY_60s) {
                // Original NDT does exit(0) in both of these cases
                ctx->logger->warn("Server busy; aborting test");
                callback(QueueServerBusyError());
                return;
            }
            if (*wait_time == SRV_QUEUE_HEARTBEAT) {
                ctx->logger->info("Got HEARTBEAT message; responding...");
                ErrorOr<Buffer> buff = messages_format_msg_waiting();
                if (!buff) {
                    callback(FormatMsgWaitingError(buff.as_error()));
                    return;
                }
                messages_write_noasync(ctx, *buff);
            } else {
                ctx->logger->info("Number of clients in queue: %d",
                                  *wait_time);
            }
            // TODO: in theory the server can keep us in queue forever...
            reactor_call_soon(ctx->reactor, [=]() {
                wait_in_queue_impl<messages_read_msg,
                                   messages_format_msg_waiting,
                                   messages_write_noasync,
                                   reactor_call_soon>(ctx, callback);
            });
            return;
        }
        ctx->logger->info("Authorized to run the test");
        callback(NoError());
    }, ctx->reactor);
}

template <MK_MOCK_NAMESPACE(messages, read_msg)>
void recv_version_impl(Var<Context> ctx, Callback<Error> callback) {
    ctx->logger->debug("ndt: recv server version ...");
    messages_read_msg(ctx, [=](Error err, uint8_t type, std::string s) {
        ctx->logger->debug("ndt: recv server version ... %d", (int)err);
        if (err) {
            callback(ReadingServerVersionMessageError(err));
            return;
        }
        if (type != MSG_LOGIN) {
            callback(NotServerVersionMessageError());
            return;
        }
        ctx->logger->info("Got server version: %s", s.c_str());
        // TODO: validate the server version?
        callback(NoError());
    }, ctx->reactor);
}

template <MK_MOCK_NAMESPACE(messages, read_msg)>
void recv_tests_id_impl(Var<Context> ctx, Callback<Error> callback) {
    ctx->logger->debug("ndt: recv tests ID ...");
    messages_read_msg(ctx, [=](Error err, uint8_t type, std::string s) {
        ctx->logger->debug("ndt: recv tests ID ... %d", (int)err);
        if (err) {
            callback(ReadingTestsIdMessageError(err));
            return;
        }
        if (type != MSG_LOGIN) {
            callback(NotTestsIdMessageError());
            return;
        }
        ctx->logger->info("Authorized tests: %s", s.c_str());
        ctx->granted_suite = split(s);
        callback(NoError());
    }, ctx->reactor);
}

template <MK_MOCK_NAMESPACE(test_c2s, run), MK_MOCK_NAMESPACE(test_meta, run),
          MK_MOCK_NAMESPACE(test_s2c, run)>
void run_tests_impl(Var<Context> ctx, Callback<Error> callback) {

    if (ctx->granted_suite.size() <= 0) {
        callback(NoError());
        return;
    }

    std::string s = ctx->granted_suite.front();
    ctx->granted_suite.pop_front();

    ErrorOr<int> num = lexical_cast_noexcept<int>(s);
    if (!num) {
        callback(InvalidTestIdError(num.as_error()));
        return;
    }

    std::function<void(Var<Context>, Callback<Error>)> func;
    if (*num == TEST_C2S) {
        func = test_c2s_run;
    } else if (*num == TEST_META) {
        func = test_meta_run;
    } else if (*num == TEST_S2C) {
        func = test_s2c_run;
    } else {
        /* nothing */
    }

    if (!func) {
        ctx->logger->warn("ndt: unknown test: %d", *num);
        // The spec says that the connection MUST be closed if we receive
        // an test we do not requested for; this is what happens below when
        // the callback of this stage is called with an error argument
        callback(UnknownTestIdError());
        return;
    }

    ctx->logger->info("Run test with id %d ...", *num);
    func(ctx, [=](Error err) {
        ctx->logger->info("Run test with id %d ... complete (%d)", *num,
                          (int)err);
        if (err) {
            // TODO: perhaps it would be better to have a specific error
            // for each failed test to disambiguate
            callback(TestFailedError(err));
            return;
        }
        run_tests_impl<test_c2s_run, test_meta_run, test_s2c_run>(ctx,
                                                                  callback);
    });
}

template <MK_MOCK_NAMESPACE(messages, read_msg)>
void recv_results_and_logout_impl(Var<Context> ctx, Callback<Error> callback) {
    ctx->logger->debug("ndt: recv RESULTS ...");
    messages_read_msg(ctx, [=](Error err, uint8_t type, std::string s) {
        ctx->logger->debug("ndt: recv RESULTS ... %d", (int)err);
        if (err) {
            callback(ReadingResultsOrLogoutError(err));
            return;
        }
        if (type == MSG_RESULTS) {
            for (auto x : split(s, "\n")) {
                if (x != "") {
                    ctx->logger->debug("%s", x.c_str());
                    (void)messages::add_to_report(ctx->entry,
                            "summary_data", x);
                }
            }
            // XXX: here we have the potential to loop forever...
            recv_results_and_logout_impl<messages_read_msg>(ctx, callback);
            return;
        }
        if (type != MSG_LOGOUT) {
            callback(NotResultsOrLogoutError());
            return;
        }
        ctx->logger->info("Got LOGOUT");
        callback(NoError());
    }, ctx->reactor);
}

template <MK_MOCK_NAMESPACE(net, read)>
void wait_close_impl(Var<Context> ctx, Callback<Error> callback) {
    ctx->logger->debug("ndt: wait close ...");
    ctx->txp->set_timeout(1.0);
    // TODO: here we should probably use ctx->buff
    Var<Buffer> buffer(new Buffer);
    net_read(ctx->txp, buffer, [=](Error err) {
        ctx->logger->debug("ndt: wait close ... %d", (int)err);
        // Note: the server SHOULD close the connection
        if (err == EofError()) {
            ctx->logger->info("Connection closed");
            callback(NoError());
            return;
        }
        if (err == TimeoutError()) {
            ctx->logger->info("Closing connection after 1.0 sec timeout");
            callback(NoError());
            return;
        }
        if (err) {
            callback(WaitingCloseError(err));
            return;
        }
        ctx->logger->debug("ndt: got extra data: %s", buffer->read().c_str());
        callback(DataAfterLogoutError());
    }, ctx->reactor);
}

static inline void disconnect_and_callback_impl(Var<Context> ctx, Error err) {
    if (ctx->txp) {
        Var<Transport> txp = ctx->txp;
        ctx->txp = nullptr;
        txp->close([=]() { ctx->callback(err); });
        return;
    }
    ctx->callback(err);
}

} // namespace protocol
} // namespace mk
} // namespace ndt
#endif
