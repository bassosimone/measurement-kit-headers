# NAME

`mk/logger.hpp`

# LIBRARY

measurement-kit (`libmeasurement_kit`, `-lmeasurement_kit`)

# SYNOPSIS

```C++
#ifndef MK_LOGGER_HPP
#define MK_LOGGER_HPP

#define MK_LOG_WARNING 0

#define MK_LOG_INFO 1

#define MK_LOG_DEBUG 2

#define MK_LOG_DEBUG2 3

namespace mk {

class Logger {
  public:
    Logger &set_verbosity(uint32_t v);

    Logger &increase_verbosity();

    [[deprecated]] uint32_t get_verbosity() const;

    uint32_t verbosity() const;

    Logger &on_log(std::function<void(uint32_t, const char *)> &&fn);

    [[deprecated]] Logger &on_eof(std::function<void()> &&fn);

    Logger &on_destroy(std::function<void()> &&fn);

    Logger &on_event(std::function<void(const char *)> &&fn);

    Logger &on_progress(std::function<void(double, const char *)> &&fn);

    Logger &set_logfile(std::string fpath);

  private:
    class Impl;
    mk::Safe<std::unique_ptr<Impl>> impl_;
};

} // namespace mk
#endif
```

# DESCRIPTION

The `mk/logger.hpp` file defines the `mk::Logger` class as well as the macro defining the log severity.

## Log severity macros

`MK_LOG_WARNING` indicates the `WARNING` log severity level.

`MK_LOG_INFO` indicates the `INFO` log severity level.

`MK_LOG_DEBUG` indicates the `DEBUG` log severity level.

`MK_LOG_DEBUG2` indicates the `DEBUG2` log severity level.

## The Logger class 

The `Logger` class allows you to configure logging for running a specific measurement or nettest. 

### Methods

The `set_verbosity()` method allows to set the logger verbosity.

The `increase_verbosity()` method increases the verbosity.

The `verbosity()` method gets the configured verbosity.

The `on_log` method allows you to specify an handler function to be called whenever a log line is emitted. The first argument to the handler will be the verbosity. The second argument to the handler will be the log line, as a C string.  Any exception thrown by the log handler is silently swallowed.

The `on_destroy()` method allows to set the destroy handler.

The `on_event` method allows you to specify an handler function to be called every time an "event" happens while running the test.  Different tests emit different kind of events. For example, the NDT test emits "download-speed" events during the download phase. Consult the documentation of each test for more information. Events will be serialized JSON objects. To make sense of the event, you are expected to unserialize the JSON and interpret it. The first argument to the handler function is the serialized JSON.

The `on_progress()` method allows to set the progress handler.

The `set_logfile()` method sets the file where to write logs.

