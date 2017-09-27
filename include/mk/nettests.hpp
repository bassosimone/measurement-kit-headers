// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.
#ifndef MK_NETTESTS_HPP
#define MK_NETTESTS_HPP

// The `mk/nettests.hpp` header defines the nettests API. This is the most
// high level API in measurement-kit. It allows you to run whole tests,
// to write their results and logs on files, and to be notified of events
// that occur during the test. If your are integrating measurement-kit
// as an engine for running tests, this is the API that you want to use.

#include <cstdint>       // for unint32_t
#include <functional>    // for std::function<>
#include <mk/logger.hpp> // for mk::Logger
#include <mk/safe.hpp>   // for mk::Safe<>
#include <string>        // for std::string

namespace mk {

// ## The BaseTest class
//
// The `BaseTest` class is common to all tests. With its methods it allows you
// to configure the test that you want to run, by providing it with input, by
// registering callbacks to be called on events, etc.
//
// In general, it is unlikely that you want to instance this class directly,
// and what you actually want is to create an instance of the specific test
// that you want to run (e.g. `WebConnectivityTest`, `DashTest`).
//
// Once you created an instance of the test you want to run, you can
// configure the test using several methods (e.g. `add_input`). All
// these configuration methods return an instance of the object itself,
// thus supporting the FluentInterface style. That is, you can concatenate
// various calls together while configuring the object.
//
// These fluent configuration methods will set internal variables that
// are stored into a unique pointer. This means that only one piece
// of code at a time can own an instance of this class.
//
// The design reason why we have chosen to implement configuration methods
// rather than exposing you the internal variables is that this gives us more
// flexibility to evolve the internal implementation without breaking
// the API exposed to measurement-kit users.
//
// Once you have configured the test, you can run it using `run()` or
// `start()`. The former executes the test and blocks until it id done.
// The latter starts the test in a background thread and calls the
// callback specified as argument when done. Do not assume that `run()`
// will run the test in the current thread. Different versions of MK
// may run it in a background thread and block the current thread until
// the test is complete.
//
// The same `BaseTest` (or derived class) cannot be used to start more
// than one test. This is because, when the test is started, we move
// the ownership of the internal state to the thread that will run the
// test itself. Therefore, attempting to start a subsequent test is
// not going to work because the state will be empty. Depending on the
// version of MK, this may either result to the callback being called
// "soon" with an error code or to an exception being raised.
//
// The fact that we want the background thread to have exclusive ownership
// of state also explains why we expect all callbacks to be moved, thus
// transferring ownership, rather than just copied.
//
// Many callbacks passed to this class take as argument a `const char *`. You
// should be careful to copy the string pointed by such pointer, if you do
// not plan on using it in the context of the handler, because in most cases
// these pointers are will be invalidated after the handler returns.
//
// Regarding exceptions, any `std::exception` or derived class thrown by
// any callback will be swallowed by the code (but a warning message
// printing the description of the exception should be printed in most case).
//
// ### Test sequence
//
// Here we describe the sequence of operations performed when running a
// measurement-kit test, the options that you can use (via `set_option()`) to
// control the behavior, and the callbacks that will be called.
//
// 1. The options with which the test was started are logged using the
// currently registered logger with severity `MK_LOG_DEBUG`.
//
// 2. The test start time is recorded and the callback registered with
// `on_begin()` will be called.
//
// 3. Unless the `MK_OPT_NO_BOUNCER` option is explicitly set to true, we
// query the bouncer specified using the bouncer with base URL specified using
// the `MK_OPT_BOUNCER_BASE_URL` option. This must be a bouncer following the
// OONI protocol, and will return information on the collector and the test
// helpers to be used. Failing to contact the bouncer will cause the test
// to fail, thus jumping to the final phase of the test (see below).
//
// 4. If `MK_OPT_COLLECTOR_BASE_URL` is explicitly set, this is used possibly
// overriding the choice made by the bouncer (if any, since it may be disabled).
//
// 5. Specific tests have specific test helpers options. For example, for
// OONI's Web Connectivity, the setting is `MK_OPT_WEB_CONNECTIVITY_HELPER`.
// By explicitly setting these options, you can override the choices the
// bouncer made (if any, since the bouncer may be disabled).
//
// 6. Unless the `MK_OPT_NO_IP_LOOKUP` option is explicitly set to true, we
// query a remote service for discovering the IP address of the probe. At the
// moment of writing this documentation, we query `geoip.ubuntu.com` but
// that may change in future versions of measurement-kit. If we fail to discover
// the probe IP and the `MK_OPT_FAIL_IF_IP_LOOKUP_FAILS` option is explicitly
// set to true, the test will fail. Otherwise the test will continue. In any
// case in which the IP address cannot be looked up, the probe IP will be
// set to `127.0.0.1`.
//
// 7. If `MK_OPT_GEOIP_COUNTRY_PATH` is explicitly set to the path of a valid
// GeoIP country database, the previously discovered probe IP is mapped to the
// country of the probe. If the lookup fails, the probe country is `ZZ`.
//
// 8. If `MK_OPT_GEOIP_ASN_PATH` is explicitly set to the path of a valid GeoIP
// ASN database, the previously discovered probe IP is mapped to the ASN of the
// probe. If the lookup fails, the probe ASN is set to `AS0`.
//
// 9. Unless `MK_OPT_SAVE_PROBE_IP` is explicitly set to true, the probe IP
// is then discarded and replaced with `127.0.0.1`.
//
// 10. If `MK_OPT_SAVE_PROBE_ASN` is explicitly set to false, the probe ASN
// is discarded and replaced with `AS0`.
//
// 11. If `MK_OPT_SAVE_PROBE_CC` is explicitly set to false, the probe country
// code is discarded and replaced with `ZZ`.
//
// 12. The probe IP, ASN, and CC (possibly redacted as explained above) are
// included into the test results.
//
// 13. Unless `MK_OPT_NO_RESOLVER_LOOKUP` is explicitly set to true, we try to
// understand the IP address of the resolver. Depending on how the DNS engine
// is configured (options `MK_OPT_DNS_RESOLVER` and `MK_OPT_DNS_ENGINE`, by
// default both empty), this may entail a different algorithm. If discovering
// the DNS resolver fails and `MK_OPT_FAIL_IF_RESOLVER_LOOKUP_FAILS` is
// explicitly set to true, the test will fail. Otherwise, it will continue.
//
// 14. Unless `MK_OPT_NO_FILE_REPORT` is explicitly set to true, the output
// file is opened. You can control the file path using `set_output_filepath()`.
// If you don't provide an explicit output filepath, a file with a test
// and current-time dependent name will be written in the current working
// directory. If opening the file fails and
// `MK_OPT_FAIL_IF_OPEN_FILE_REPORT_FAILS` is explicitly set to true,
// the test will fail. Otherwise it will continue.
//
// ### Methods
class BaseTest {
  public:
    BaseTest();

    virtual ~BaseTest();

    [[deprecated]] BaseTest &on_logger_eof(std::function<void()> &&cb);

    [[deprecated]] BaseTest &
    on_log(std::function<void(uint32_t, const char *)> &&cb);

    [[deprecated]] BaseTest &on_event(std::function<void(const char *)> &&cb);

    [[deprecated]] BaseTest &
    on_progress(std::function<void(double, const char *)> &&cb);

    [[deprecated]] BaseTest &set_verbosity(uint32_t v);

    [[deprecated]] BaseTest &increase_verbosity();

    // TODO: finish documenting all methods (straightforward but boring...)

    BaseTest &set_logger(Logger logger);

    BaseTest &add_input(std::string s);

    BaseTest &add_input_filepath(std::string s);

    BaseTest &set_input_filepath(std::string s);

    BaseTest &set_output_filepath(std::string s);

    BaseTest &set_error_filepath(std::string s);

    [[deprecated]] BaseTest &set_options(std::string key, std::string value);

    BaseTest &set_option(std::string key, std::string value);

    [[deprecated]] BaseTest &on_entry(std::function<void(std::string)> &&cb);

    BaseTest &on_entry(std::function<void(const char *)> &&cb);

    BaseTest &on_begin(std::function<void()> &&cb);

    BaseTest &on_end(std::function<void()> &&cb);

    BaseTest &on_destroy(std::function<void()> &&cb);

    void run();

    void start(std::function<void()> &&cb);

  private:
    class Impl;
    mk::Safe<std::unique_ptr<Impl>> impl_;
};

// ## Derived classes

#define MK_DECLARE_TEST(_name_)                                                \
    class _name_ : public BaseTest {                                           \
      public:                                                                  \
        _name_();                                                              \
    }

/* TODO: write a description message before each test */

MK_DECLARE_TEST(DashTest);

MK_DECLARE_TEST(CaptivePortalTest);

MK_DECLARE_TEST(DnsInjectionTest);

MK_DECLARE_TEST(FacebookMessengerTest);

MK_DECLARE_TEST(HttpHeaderFieldManipulationTest);

MK_DECLARE_TEST(HttpInvalidRequestLineTest);

MK_DECLARE_TEST(MeekFrontedRequestsTest);

MK_DECLARE_TEST(ExtendedNetworkDiagnosticTest);

using MultiNdtTest = ExtendedNetworkDiagnosticTest; // backward compat alias

MK_DECLARE_TEST(NetworkDiagnosticTest);

using NdtTest = NetworkDiagnosticTest; // backward compat alias

MK_DECLARE_TEST(TcpConnectTest);

MK_DECLARE_TEST(TelegramTest);

MK_DECLARE_TEST(WebConnectivityTest);

#undef MK_DECLARE_TEST

} // namespace mk
#endif
