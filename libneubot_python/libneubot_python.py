# This file was automatically generated by SWIG (http://www.swig.org).
# Version 2.0.11
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.





from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_libneubot_python', [dirname(__file__)])
        except ImportError:
            import _libneubot_python
            return _libneubot_python
        if fp is not None:
            try:
                _mod = imp.load_module('_libneubot_python', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _libneubot_python = swig_import_helper()
    del swig_import_helper
else:
    import _libneubot_python
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


try:
    import weakref
    weakref_proxy = weakref.proxy
except:
    weakref_proxy = lambda x: x


class Poller(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Poller, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Poller, name)
    __repr__ = _swig_repr
    def __init__(self): 
        this = _libneubot_python.new_Poller()
        try: self.this.append(this)
        except: self.this = this
    def loop(self): return _libneubot_python.Poller_loop(self)
    def break_loop(self): return _libneubot_python.Poller_break_loop(self)
    __swig_destroy__ = _libneubot_python.delete_Poller
    __del__ = lambda self : None;
Poller_swigregister = _libneubot_python.Poller_swigregister
Poller_swigregister(Poller)

class EchoServer(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, EchoServer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, EchoServer, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _libneubot_python.new_EchoServer(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _libneubot_python.delete_EchoServer
    __del__ = lambda self : None;
EchoServer_swigregister = _libneubot_python.EchoServer_swigregister
EchoServer_swigregister(EchoServer)

class Pollable(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Pollable, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Pollable, name)
    __repr__ = _swig_repr
    def handle_read(self): return _libneubot_python.Pollable_handle_read(self)
    def handle_write(self): return _libneubot_python.Pollable_handle_write(self)
    def handle_error(self): return _libneubot_python.Pollable_handle_error(self)
    def __init__(self): 
        if self.__class__ == Pollable:
            _self = None
        else:
            _self = self
        this = _libneubot_python.new_Pollable(_self, )
        try: self.this.append(this)
        except: self.this = this
    def attach(self, *args): return _libneubot_python.Pollable_attach(self, *args)
    def detach(self): return _libneubot_python.Pollable_detach(self)
    def fileno(self): return _libneubot_python.Pollable_fileno(self)
    def set_readable(self): return _libneubot_python.Pollable_set_readable(self)
    def unset_readable(self): return _libneubot_python.Pollable_unset_readable(self)
    def set_writable(self): return _libneubot_python.Pollable_set_writable(self)
    def unset_writable(self): return _libneubot_python.Pollable_unset_writable(self)
    def set_timeout(self, *args): return _libneubot_python.Pollable_set_timeout(self, *args)
    def clear_timeout(self): return _libneubot_python.Pollable_clear_timeout(self)
    __swig_destroy__ = _libneubot_python.delete_Pollable
    __del__ = lambda self : None;
    def __disown__(self):
        self.this.disown()
        _libneubot_python.disown_Pollable(self)
        return weakref_proxy(self)
Pollable_swigregister = _libneubot_python.Pollable_swigregister
Pollable_swigregister(Pollable)

# This file is compatible with both classic and new-style classes.

