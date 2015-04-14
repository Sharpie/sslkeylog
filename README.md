SSLkeylog
=========

An Ruby library that logs SSL session keys from client connections in [NSS Key Log Format][nss-format]. This log can be used by tools such as [Wireshark][wireshark] to decrypt data when analyzing network traffic.

**NOTE:** This version of the library is a functional prototype. The implementation may change in the future.

[![Build Status](https://travis-ci.org/Sharpie/sslkeylog.svg?branch=master)](https://travis-ci.org/Sharpie/sslkeylog)

  [nss-format]: https://developer.mozilla.org/en-US/docs/Mozilla/Projects/NSS/Key_Log_Format

  [wireshark]:https://wiki.wireshark.org/SSL#Using_the_.28Pre.29-Master-Secret


## Installation

This gem uses a C extension to extract data from Ruby `OpenSSL::SSL::SSLSocket` objects. This means that the Gem will have to be built using the same OpenSSL headers that Ruby used during compilation.

The logic for locating the `include` directory is not particularly sophisticated, so the proper location may need to be specified during installation:

    gem install openssl-keylog -- --with-openssl-include=...

Use of the wrong header file can result in segmentation faults and other unpleasantness.


## Usage

The simplest way to use this library is to set an output file using the `SSLKEYLOG` environment variable and then require the `sslkeylog/autotrace` module:

```ruby
# In bash: export SSLKEYLOG=$HOME/sslkeylog.log
require 'sslkeylog/autotrace'
```

If the `SSLKEYLOG` variable is unset, output will be sent to `$stderr`.


Tracing can be enabled for specific portions of a Ruby program and output can be routed to any object which behaves like a standard Ruby `Logger`:

```ruby
require 'sslkeylog'
require 'stringio'

output = StringIO.new
SSLkeylog::Logging.logger = Logger.new(output)

socket = TCPSocket.new('github.com', '443')
ssl_socket = OpenSSL::SSL::SSLSocket.new(socket)

SSLkeylog::Trace.enable
ssl_socket.connect
SSLkeylog::Trace.disable

ssl_socket.close
puts output.string
# => I, [2015-04-14T12:51:33.016410 #90145]  INFO -- : CLIENT_RANDOM 20A97D56EAE0850DE6CECB63D4D587D863E62750AF3AF032453608DA5F8787C0 58260274A5BD57E1158AD2CAFEE1D8F671F900419F37B6A2DDB20FA0AC8B96AC0940398B33D42F366E2937151C751CED
```

Data can be extracted directly from `OpenSSL::SSL:SSLSocket` objects using the `to_keylog` method of the `SSLkeylog::OpenSSL` module:

```ruby
require 'sslkeylog/openssl'

socket = TCPSocket.new('github.com', '443')
ssl_socket = OpenSSL::SSL::SSLSocket.new(socket)
ssl_socket.connect

puts SSLkeylog::OpenSSL.to_keylog(ssl_socket)
ssl_socket.close
# => "CLIENT_RANDOM 7374BF6508668783736B211242A4BC2CF075FC508E49B9797B038D6357370A10 C5BB2BDFEF788E7BB6ED0A37962BEEB140AC7F33DEF0E344F576D18305AF5A6C0121E069F1FF4CE4424530A83D443EFD\n"
```


## Implementation

The NSS Key Log format contains two pieces of information that can be used to decrypt SSL session data:

  1. A string of 32 random bytes sent by the client during the "Client Hello" phase of the SSL handshake.

  2. A string of 48 bytes generated during the handshake that is the "pre-master key" required to decrypt the data.

The Ruby OpenSSL library exposes the pre-master key through the `to_text` method of  `OpenSSL::SSL::Session` objects. Unfortunately, there is no way to access the client random data at the Ruby level.

However, many Ruby objects are simple wrappers for C data structures and the OpenSSL `SSL` struct does contains all the data we need --- including the client random data. The Ruby object which wraps `struct SSL` is `OpenSSL::SSL::SSLSocket`. This `to_keylog` method provided is a simple C extension which un-wraps `SSLSocket` objects at the C level and reads the data required form a NSS Key Log entry.
