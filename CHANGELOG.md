SSLkeylog
=========

0.2.0
-----

2015-04-14

Backwards incompatible feature release.

  - +Break
    Library renamed from `OpenSSL::Keylog` to `SSLkeylog`.

  - +Break
    The library no longer extends (monkeypatches) the `OpenSSL::SSL::SSLSocket`
    class. This functionality is now provided by module functions on
    `SSLkeylog::OpenSSL`.

  - +New
    A simple tracing and logging system built on top of the standard `Logging`
    library and the Ruby 2.0 `TracePoint` API that allows SSL client
    connections to be logged.


0.1.0
-----

2015-04-12

(unreleased) First tagged prototype.

  - +New
    Added the `to_keylog` method for `OpenSSL:SSL::SSLSocket` objects which extracts and formats NSS Key Log data.
