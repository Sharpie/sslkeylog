require 'spec_helper'
require 'stringio'

describe 'OpenSSL client connections' do
  let(:ssl_server_address) { '127.0.0.1' }
  let(:ssl_server_port)    { 9045 }
  include_context 'ssl_server'

  before(:each) do
    @tcp_socket = TCPSocket.new(ssl_server_address, ssl_server_port)
    @ssl_socket = OpenSSL::SSL::SSLSocket.new(@tcp_socket)
  end

  context 'before a connection is established' do
    after(:each) { @tcp_socket.close }

    it 'returns nil' do
      expect(SSLkeylog::OpenSSL.to_keylog(@ssl_socket)).to be_nil
    end
  end

  context 'after a connection is established' do
    before(:each) { @ssl_socket.connect }
    after(:each)  { @ssl_socket.close }

    it 'returns a string starting with CLIENT_RANDOM' do
      expect(SSLkeylog::OpenSSL.to_keylog(@ssl_socket)).to start_with('CLIENT_RANDOM')
    end

    it 'contains the session master key' do
      logged_key = SSLkeylog::OpenSSL.to_keylog(@ssl_socket).split.last.strip
      master_key = @ssl_socket.session.to_text.lines.find {|l| l.match /^\s*Master-Key:/}.partition(': ').last.strip

      expect(logged_key).to match(master_key)
    end
  end

  context 'when tracing is enabled' do
    subject { StringIO.new }

    before(:each) do
      logger           = Logger.new(subject)
      logger.formatter = SSLkeylog::Logging.default_formatter
      SSLkeylog::Logging.logger = logger # TODO: Replace with a mock.

      SSLkeylog::Trace.enable
      @ssl_socket.connect
    end
    after(:each)  do
      @ssl_socket.close
      SSLkeylog::Trace.disable
    end

    it 'logs the session master key' do
      logged_key = subject.string.split.last.strip
      master_key = @ssl_socket.session.to_text.lines.find {|l| l.match /^\s*Master-Key:/}.partition(': ').last.strip

      expect(logged_key).to match(master_key)
    end
  end
end
