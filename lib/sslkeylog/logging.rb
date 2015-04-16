require 'logger'

module SSLkeylog
  # This module configures logging used by SSL tracing
  module Logging
    class << self
      # The global logger used by SSL tracers
      #
      # Must be set to an object that implements the interface provided by the
      # standard Ruby `Logger` library.
      #
      # @return [Logger]
      attr_accessor :logger
    end

    # Returns a default Logger formatter for NSS Key Log messages
    #
    # This formatter simply echoes any message without adding additional data
    # such as timestamp, severity, etc.
    #
    # @return [Proc]
    def self.default_formatter
      Proc.new do |severity, datetime, progname, msg|
        case severity
        when 'DEBUG'
          # Prefix debug-level message so they appear as comments in NSS Log
          # Format.
          "# " + msg
        else
          msg
        end
      end
    end

    # Returns a default Logger for NSS Key Log messages
    #
    # @return [Logger] A logger that appends to a file specified by the
    #   `SSLKEYLOG` environment variable.
    # @return [Logger] A logger that writes to `$stderr` if the `SSLKEYLOG`
    #   environment variable is not set.
    def self.default_logger
      if ENV['SSLKEYLOG']
        output = File.open(ENV['SSLKEYLOG'], 'a')
        # Flush output after every write as an external process, such as
        # Wireshark, may be watching this file.
        output.sync = true
      else
        output = $stderr
      end

      logger = Logger.new(output)
      logger.formatter = default_formatter

      logger
    end

    self.logger = self.default_logger
  end
end
