require 'openssl/keylog'

module OpenSSL::Keylog
  module SessionExtensions
    def to_keylog
      # TODO: Create an actual implementation.
      'CLIENT_RANDOM RANDOM HOSTKEY'
    end
  end
end

OpenSSL::SSL::Session.send(:include, OpenSSL::Keylog::SessionExtensions)
