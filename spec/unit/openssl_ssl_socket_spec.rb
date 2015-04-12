require 'spec_helper'

describe OpenSSL::SSL::SSLSocket do
  it 'is extended by OpenSSL::Keylog::SSLSocketExtensions' do
    expect(described_class.ancestors).to include(OpenSSL::Keylog::SSLSocketExtensions)
  end
end
