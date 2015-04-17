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