require 'sslkeylog/logging'

module SSLkeylog

  # This module provides methods for tracing SSL connections
  #
  # Currently, tracing is only implemented for client connections. Tracing of
  # connections accepted by servers is not implemented. Tracing is implemented
  # using the Ruby 2.x `TracePoint` API.
  module Trace
    # A TracePoint that watches SSL client connections
    #
    # This tracepoint watches for returns from `OpenSSL::SSL::SSLSocket#connect`
    # and logs the pre master secret to the logger returned by
    # {SSLkeylog::Logging.logger}. Messages are logged at `info` level.
    #
    # @return [TracePoint]
    CLIENT_TRACER = TracePoint.new(:c_return) do |tp|
      if tp.method_id == :connect && tp.defined_class == ::OpenSSL::SSL::SSLSocket
        ssl_info = ::SSLkeylog::OpenSSL.to_keylog(tp.self)
        ::SSLkeylog::Logging.logger.info(ssl_info) unless ssl_info.nil?
      end
    end

    # Enable tracing of SSL connections
    #
    # @return [void]
    def self.enable
      CLIENT_TRACER.enable
    end

    # Disable tracing of SSL connections
    #
    # @return [void]
    def self.disable
      CLIENT_TRACER.disable
    end
  end
end
