// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.
#ifndef MK_FAILURE_HPP
#define MK_FAILURE_HPP

// The `mk/failure.hpp` header defines the `mk::Failure` class. This header also
// defines all the possible failure strings. These strings are compliant with
// the [OONI specification](https://github.com/TheTorProject/ooni-spec/).

#include <mk/safe.hpp> // for mk::Safe
#include <string>      // for std::string
#include <utility>     // for std::move
#include <vector>      // for std::vector

namespace mk {

// ## The Failure class
//
// The `Failure` class holds the failure that occurred while running a specific
// measurement (hopefully no failure if the measurement succeded).
//
// Note that the absence of failures does not necessarily mean that no network
// error was detected during the measurement itself. A specific measurement may
// treat a network error as a failure, while another test may not.
//
// For example, if you are running a performance test, then receiving
// `ECONNRESET` while trying to connect to a measurement server, is a failure to
// be reported. But another test that is measuring censorship may consider
// instead the RST not as a failure but as an interesting anomaly to include
// into the results.
//
// A special failure is `composite_failure`. This happens when an operation
// fails multiple times. For example, a composite failure is returned when
// you attempt to connect to an hostname for which there are multiple A records
// where connecting to each A record failed. In such case, the failure string
// will be `composite_failure` and you can get specific sub-failures using
// the `child_failures()` method; otherwise, predictably, such method would
// return an empty vector.
//
// ### Methods
class Failure {
  public:
    // The default constructor creates a failure containing no error. That is
    // to say that no failure has actually occurred.
    Failure();

    // The constructor with failure string creates the failure corresponding
    // to the string specified as argument.
    Failure(std::string s);

    // The cast to `bool` operator returns `false` if no failure actually
    // occurred and `true` otherwise.
    operator bool() const noexcept;

    // The `reason()` method returns the failure string. In case no failure
    // occurred, the string would actually be an empty string.
    const char *reason() const;

    // The `detailed_reason()` method returns the failure (including all
    // child failures) as a serialized JSON object. In case of no child
    // failures, a string is returned, just like `reason()`. In case there
    // was no failure, an empty string would be returned.
    const char *detailed_reason() const;

    // The `child_failures()` method returns the child failures.
    const std::vector<Failure> &child_failures() const;

  private:
    class Impl;
    mk::Safe<std::shared_ptr<Impl>> impl_;
};

/* TODO: define here all the failure strings */

// ## Failure strings

#define MK_GENERIC_TIMEOUT_ERROR "generic_timeout_error"

#define MK_EOF_ERROR "eof_error"

} // namespace mk
#endif
