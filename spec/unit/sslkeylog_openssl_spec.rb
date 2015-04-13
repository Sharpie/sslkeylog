require 'spec_helper'

describe SSLkeylog::OpenSSL do
  it 'reports the OPENSSL_VERSION it was built against' do
    expect(SSLkeylog::OpenSSL::OPENSSL_VERSION).to match(/^OpenSSL/)
  end

  it 'reports the OPENSSL_VERSION_NUMBER it was built against' do
    expect(SSLkeylog::OpenSSL::OPENSSL_VERSION_NUMBER).to be_a(Integer)
  end

  describe '.to_keylog' do
    it 'rejects arguments that are not SSLSocket objects' do
      expect { described_class.to_keylog('foo') }.to raise_error(TypeError,
        /Expected instance of OpenSSL::SSL::SSLSocket\)$/)
    end
  end
end
