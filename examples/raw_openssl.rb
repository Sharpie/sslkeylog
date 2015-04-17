require 'sslkeylog/openssl'

socket = TCPSocket.new('github.com', '443')
ssl_socket = OpenSSL::SSL::SSLSocket.new(socket)
ssl_socket.connect

puts SSLkeylog::OpenSSL.to_keylog(ssl_socket)
ssl_socket.close