require 'openssl'

module OpenSSL
  module Keylog
    require 'openssl/keylog/version'
    require 'openssl/keylog/session_extensions'

    OpenSSL::SSL::Session.send(:include, OpenSSL::Keylog::SessionExtensions)
  end
end
