require 'openssl/keylog'

module OpenSSL::Keylog
  module SessionExtensions
  end
end

OpenSSL::SSL::Session.send(:include, OpenSSL::Keylog::SessionExtensions)
