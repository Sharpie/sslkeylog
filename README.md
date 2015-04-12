OpenSSL::Keylog
===============

**NOTE:** This version of the library is a functional prototype. The name and implementation will definitely change.

An extension to the Ruby OpenSSL library that logs session keys in [NSS Key Log Format][nss-format]. This log can be used by tools such as [Wireshark][wireshark] to decrypt data when analyzing network traffic.

[![Build Status](https://travis-ci.org/Sharpie/openssl-keylog.svg?branch=master)](https://travis-ci.org/Sharpie/openssl-keylog)

  [nss-format]: https://developer.mozilla.org/en-US/docs/Mozilla/Projects/NSS/Key_Log_Format

  [wireshark]:https://wiki.wireshark.org/SSL#Using_the_.28Pre.29-Master-Secret


## Installation

This gem uses a C extension to extract data from Ruby `OpenSSL::SSL::SSLSocket` objects. This means that the Gem will have to be built using the same OpenSSL headers that Ruby used during compilation.

The logic for locating the `include` directory is not particularly sophisticated, so the proper location may need to be specified during installation:

    gem install openssl-keylog -- --with-openssl-include=...

Use of the wrong header file can result in segmentation faults and other unpleasantness.


## Usage

Currently, the library just extends `OpenSSL::SSL::SSLSocket` objects with a `to_keylog` method that can be used to return NSS Key Log data:

```ruby
require 'openssl/keylog'

socket = TCPSocket.new('github.com', '443')
ssl_socket = OpenSSL::SSL::SSLSocket.new(socket)
ssl_socket.connect

ssl_socket.to_keylog
# => "CLIENT_RANDOM 7374BF6508668783736B211242A4BC2CF075FC508E49B9797B038D6357370A10 C5BB2BDFEF788E7BB6ED0A37962BEEB140AC7F33DEF0E344F576D18305AF5A6C0121E069F1FF4CE4424530A83D443EFD\n"
```

The `to_keylog` method should not be called on an unconnected SSL Socket. Bad things will happen. Proper error checking for this situation will be added in a future version.


## Implementation

The NSS Key Log format contains two pieces of information that can be used to decrypt SSL session data:

  1. A string of 32 random bytes sent by the client during the "Client Hello" phase of the SSL handshake.

  2. A string of 48 bytes generated during the handshake that is the "pre-master key" required to decrypt the data.

The Ruby OpenSSL library exposes the pre-master key through the `to_text` method of  `OpenSSL::SSL::Session` objects. Unfortunately, there is no way to access the client random data at the Ruby level.

However, many Ruby objects are simple wrappers for C data structures and the OpenSSL `SSL` struct does contains all the data we need --- including the client random data. The Ruby object which wraps `struct SSL` is `OpenSSL::SSL::SSLSocket`. This `to_keylog` method provided is a simple C extension which un-wraps `SSLSocket` objects at the C level and reads the data required form a NSS Key Log entry.
