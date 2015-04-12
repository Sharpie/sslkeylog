SSLkeylog
=========

0.2.0
-----

(unreleased)

Backwards incompatible feature release.

  - +Break
    Library renamed from `OpenSSL::Keylog` to `SSLkeylog`.

  - +Break
    The library no longer extends (monkeypatches) the `OpenSSL::SSL::SSLSocket`
    class.


0.1.0
-----

2015-04-12

(unreleased) First tagged prototype.

  - +New
    Added the `to_keylog` method for `OpenSSL:SSL::SSLSocket` objects which extracts and formats NSS Key Log data.
