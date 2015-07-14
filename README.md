SSLkeylog
=========

A Ruby library that logs SSL session keys from client connections in [NSS Key Log Format][nss-format]. This log can be used by tools such as [Wireshark][wireshark] to decrypt data when analyzing network traffic.

**NOTE:** This version of the library is a functional prototype. The implementation may change in the future.

[![Build Status](https://travis-ci.org/Sharpie/sslkeylog.svg?branch=master)](https://travis-ci.org/Sharpie/sslkeylog)

  [nss-format]: https://developer.mozilla.org/en-US/docs/Mozilla/Projects/NSS/Key_Log_Format

  [wireshark]:https://wiki.wireshark.org/SSL#Using_the_.28Pre.29-Master-Secret


## Installation

This gem uses a C extension to extract data from Ruby `OpenSSL::SSL::SSLSocket` objects. This means that the Gem will have to be built using the same OpenSSL headers that were used to compile the Ruby interpreter.

The logic for locating the `include` directory is not particularly sophisticated, so the proper location may need to be specified during installation:

    gem install sslkeylog -- --with-openssl-include=...

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
# D, [2015-04-16T18:31:53.959279 #82759] DEBUG -- : SSLSocket connect: 192.30.252.129:443
# I, [2015-04-16T18:31:53.959423 #82759]  INFO -- : CLIENT_RANDOM 98DC410A9D6D5851487DA25991E77E60D236832D35AACDF59896927A1C56063D 5EA02F1DBCB9138C50D091949A838C074AD4574490058CBD0BBE6D2D94DA58D4AB190E939CF227EE6AD6E20317C9D922
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
