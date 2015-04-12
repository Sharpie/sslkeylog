require 'openssl'

module OpenSSL
  module Keylog
    require 'openssl/keylog/version'
    require 'openssl/keylog/ssl_socket_extensions'

    OpenSSL::SSL::SSLSocket.send(:include, OpenSSL::Keylog::SSLSocketExtensions)
  end
end
