# Part of measurement-kit <https://measurement-kit.github.io/>.
# Measurement-kit is free software. See AUTHORS and LICENSE for more
# information on the copying conditions.

AUTOMAKE_OPTIONS = foreign 1.9 subdir-objects
ACLOCAL_AMFLAGS = -I m4

LIBMEASUREMENT_KIT_I_FLAGS = -I $(top_srcdir)/include
LIBMEASUREMENT_KIT_W_FLAGS = -Wall -Wextra -pedantic

VERSION_INFO = -release @VERSION@ -version-info 0
AM_CFLAGS = $(LIBMEASUREMENT_KIT_W_FLAGS) $(LIBMEASUREMENT_KIT_I_FLAGS)
AM_CXXFLAGS = $(LIBMEASUREMENT_KIT_W_FLAGS) $(LIBMEASUREMENT_KIT_I_FLAGS)

lib_LTLIBRARIES               = libmeasurement_kit.la
libmeasurement_kit_la_LDFLAGS = $(VERSION_INFO)
libmeasurement_kit_la_SOURCES = # Empty

if BUILD_BINARIES
    bin_PROGRAMS = measurement_kit
    measurement_kit_LDADD = libmeasurement_kit.la
    measurement_kit_SOURCES = src/measurement_kit/main.cpp
endif

noinst_PROGRAMS    = # Empty
ALL_TESTS          = # Empty

include include.am

TESTS = $(ALL_TESTS)
check_PROGRAMS = $(ALL_TESTS)

VALGRIND = valgrind --trace-children=yes --error-exitcode=1                    \
                    --dsymutil=yes --leak-check=yes --gen-suppressions=all     \
                    --suppressions=build/valgrind/openssl.supp

run-valgrind: $(check_PROGRAMS)
	                                                                       \
	for test_name in $(TESTS); do                                          \
	  echo "";                                                             \
	  if ! $(VALGRIND) $$test_name; then                                   \
	    failed="$$test_name $$failed";                                     \
	  fi;                                                                  \
	done;                                                                  \
	if test "$$failed" != ""; then                                         \
	  echo "";                                                             \
	  echo "failed tests: $$failed";                                       \
	  exit 1;                                                              \
	fi
