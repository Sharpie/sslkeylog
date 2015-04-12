require 'webrick/https'

# This context runs an OpenSSL::SSL:SSLServer that is accessible to tests.  The
# `ssl_server_port` and `ssl_server_address` will need to be specified before
# this context is included:
#
#     let(:ssl_server_address) { ... }
#     let(:ssl_server_port)    { ... }
#     include 'ssl_server'
shared_context 'ssl_server' do
  before(:each) do
    @server = WEBrick::HTTPServer.new(
      :BindAddress  => ssl_server_address,
      :Port         => ssl_server_port,
      :SSLEnable    => true,
      :SSLCertName  => [%w[CN localhost]],
      :Logger       => WEBrick::Log.new(nil, WEBrick::BasicLog::WARN),
      )
    @thread = Thread.new { @server.start }
  end

  after(:each) do
    @server.shutdown rescue nil
    @thread.join rescue nil
  end
end
