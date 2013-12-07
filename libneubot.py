#
# LibNeubot interface - Public domain.
# WARNING: Autogenerated file - do not edit!
#

import ctypes
import sys

if sys.platform == "darwin":
    LIBNEUBOT_NAME = "/usr/local/lib/libneubot.dylib.0"
else:
    LIBNEUBOT_NAME = "/usr/local/lib/libneubot.so.0"

LIBNEUBOT = ctypes.CDLL(LIBNEUBOT_NAME)

# Classes:

# struct NeubotEchoServer
# struct NeubotEvent
# struct NeubotPollable
# struct NeubotPoller

# Callbacks:

NEUBOT_POLLER_RESOLVE_CALLBACK = ctypes.CFUNCTYPE(None, ctypes.c_void_p, 
    ctypes.c_char_p)
NEUBOT_POLLER_CALLBACK = ctypes.CFUNCTYPE(None, ctypes.c_void_p)
NEUBOT_POLLABLE_CALLBACK = ctypes.CFUNCTYPE(None, ctypes.c_void_p)

# NeubotEchoServer API:

LIBNEUBOT.NeubotEchoServer_construct.restype = ctypes.c_void_p
LIBNEUBOT.NeubotEchoServer_construct.argtypes = (
    ctypes.c_void_p,
    ctypes.c_int,
    ctypes.c_char_p,
    ctypes.c_char_p,
)

def NeubotEchoServer_construct(poller, use_ipv6, address, port):
    ret = LIBNEUBOT.NeubotEchoServer_construct(poller, use_ipv6, address, 
      port)
    if not ret:
        raise RuntimeError('LibNeubot error')
    return ret

# NeubotEvent API:

LIBNEUBOT.NeubotEvent_cancel.argtypes = (
    ctypes.c_void_p,
)

def NeubotEvent_cancel(handle):
    LIBNEUBOT.NeubotEvent_cancel(handle)

# NeubotPollable API:

LIBNEUBOT.NeubotPollable_construct.restype = ctypes.c_void_p
LIBNEUBOT.NeubotPollable_construct.argtypes = (
    ctypes.c_void_p,
    NEUBOT_POLLABLE_CALLBACK,
    NEUBOT_POLLABLE_CALLBACK,
    NEUBOT_POLLABLE_CALLBACK,
    ctypes.c_void_p,
)

def NeubotPollable_construct(poller, handle_read, handle_write, 
      handle_close, opaque):
    ret = LIBNEUBOT.NeubotPollable_construct(poller, handle_read, 
      handle_write, handle_close, opaque)
    if not ret:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPollable_opaque.restype = ctypes.c_void_p
LIBNEUBOT.NeubotPollable_opaque.argtypes = (
    ctypes.c_void_p,
)

def NeubotPollable_opaque(handle):
    ret = LIBNEUBOT.NeubotPollable_opaque(handle)
    if not ret:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPollable_poller.restype = ctypes.c_void_p
LIBNEUBOT.NeubotPollable_poller.argtypes = (
    ctypes.c_void_p,
)

def NeubotPollable_poller(handle):
    ret = LIBNEUBOT.NeubotPollable_poller(handle)
    if not ret:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPollable_attach.restype = ctypes.c_int
LIBNEUBOT.NeubotPollable_attach.argtypes = (
    ctypes.c_void_p,
    ctypes.c_longlong,
)

def NeubotPollable_attach(handle, filenum):
    ret = LIBNEUBOT.NeubotPollable_attach(handle, filenum)
    if ret != 0:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPollable_detach.argtypes = (
    ctypes.c_void_p,
)

def NeubotPollable_detach(handle):
    LIBNEUBOT.NeubotPollable_detach(handle)

LIBNEUBOT.NeubotPollable_fileno.restype = ctypes.c_longlong
LIBNEUBOT.NeubotPollable_fileno.argtypes = (
    ctypes.c_void_p,
)

def NeubotPollable_fileno(handle):
    ret = LIBNEUBOT.NeubotPollable_fileno(handle)
    return ret

LIBNEUBOT.NeubotPollable_set_readable.restype = ctypes.c_int
LIBNEUBOT.NeubotPollable_set_readable.argtypes = (
    ctypes.c_void_p,
)

def NeubotPollable_set_readable(handle):
    ret = LIBNEUBOT.NeubotPollable_set_readable(handle)
    if ret != 0:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPollable_unset_readable.restype = ctypes.c_int
LIBNEUBOT.NeubotPollable_unset_readable.argtypes = (
    ctypes.c_void_p,
)

def NeubotPollable_unset_readable(handle):
    ret = LIBNEUBOT.NeubotPollable_unset_readable(handle)
    if ret != 0:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPollable_set_writable.restype = ctypes.c_int
LIBNEUBOT.NeubotPollable_set_writable.argtypes = (
    ctypes.c_void_p,
)

def NeubotPollable_set_writable(handle):
    ret = LIBNEUBOT.NeubotPollable_set_writable(handle)
    if ret != 0:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPollable_unset_writable.restype = ctypes.c_int
LIBNEUBOT.NeubotPollable_unset_writable.argtypes = (
    ctypes.c_void_p,
)

def NeubotPollable_unset_writable(handle):
    ret = LIBNEUBOT.NeubotPollable_unset_writable(handle)
    if ret != 0:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPollable_set_timeout.argtypes = (
    ctypes.c_void_p,
    ctypes.c_double,
)

def NeubotPollable_set_timeout(handle, delta):
    LIBNEUBOT.NeubotPollable_set_timeout(handle, delta)

LIBNEUBOT.NeubotPollable_clear_timeout.argtypes = (
    ctypes.c_void_p,
)

def NeubotPollable_clear_timeout(handle):
    LIBNEUBOT.NeubotPollable_clear_timeout(handle)

LIBNEUBOT.NeubotPollable_close.argtypes = (
    ctypes.c_void_p,
)

def NeubotPollable_close(handle):
    LIBNEUBOT.NeubotPollable_close(handle)

# NeubotPoller API:

LIBNEUBOT.NeubotPoller_construct.restype = ctypes.c_void_p
LIBNEUBOT.NeubotPoller_construct.argtypes = (
)

def NeubotPoller_construct():
    ret = LIBNEUBOT.NeubotPoller_construct()
    if not ret:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPoller_sched.restype = ctypes.c_int
LIBNEUBOT.NeubotPoller_sched.argtypes = (
    ctypes.c_void_p,
    ctypes.c_double,
    NEUBOT_POLLER_CALLBACK,
    ctypes.py_object,
)

def NeubotPoller_sched(handle, delta, callback, obj):
    ret = LIBNEUBOT.NeubotPoller_sched(handle, delta, callback, obj)
    if ret != 0:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPoller_defer_read.restype = ctypes.c_void_p
LIBNEUBOT.NeubotPoller_defer_read.argtypes = (
    ctypes.c_void_p,
    ctypes.c_longlong,
    NEUBOT_POLLER_CALLBACK,
    NEUBOT_POLLER_CALLBACK,
    ctypes.py_object,
    ctypes.c_double,
)

def NeubotPoller_defer_read(handle, fileno, handle_ok, handle_timeout, 
      obj, timeout):
    ret = LIBNEUBOT.NeubotPoller_defer_read(handle, fileno, handle_ok, 
      handle_timeout, obj, timeout)
    if not ret:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPoller_defer_write.restype = ctypes.c_void_p
LIBNEUBOT.NeubotPoller_defer_write.argtypes = (
    ctypes.c_void_p,
    ctypes.c_longlong,
    NEUBOT_POLLER_CALLBACK,
    NEUBOT_POLLER_CALLBACK,
    ctypes.py_object,
    ctypes.c_double,
)

def NeubotPoller_defer_write(handle, fileno, handle_ok, handle_timeout, 
      obj, timeout):
    ret = LIBNEUBOT.NeubotPoller_defer_write(handle, fileno, handle_ok, 
      handle_timeout, obj, timeout)
    if not ret:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPoller_resolve.restype = ctypes.c_int
LIBNEUBOT.NeubotPoller_resolve.argtypes = (
    ctypes.c_void_p,
    ctypes.c_int,
    ctypes.c_char_p,
    NEUBOT_POLLER_RESOLVE_CALLBACK,
    ctypes.py_object,
)

def NeubotPoller_resolve(handle, use_ipv6, name, callback, opaque):
    ret = LIBNEUBOT.NeubotPoller_resolve(handle, use_ipv6, name, 
      callback, opaque)
    if ret != 0:
        raise RuntimeError('LibNeubot error')
    return ret

LIBNEUBOT.NeubotPoller_loop.argtypes = (
    ctypes.c_void_p,
)

def NeubotPoller_loop(handle):
    LIBNEUBOT.NeubotPoller_loop(handle)

LIBNEUBOT.NeubotPoller_break_loop.argtypes = (
    ctypes.c_void_p,
)

def NeubotPoller_break_loop(handle):
    LIBNEUBOT.NeubotPoller_break_loop(handle)

