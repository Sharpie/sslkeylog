require 'spec_helper'

describe 'OpenSSL client connections' do
  let(:ssl_server_address) { '127.0.0.1' }
  let(:ssl_server_port)    { 9045 }
  include_context 'ssl_server'

  before(:each) do
    socket      = TCPSocket.new(ssl_server_address, ssl_server_port)
    @ssl_socket = OpenSSL::SSL::SSLSocket.new(socket)
    @ssl_socket.connect
  end
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
