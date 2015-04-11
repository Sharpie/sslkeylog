require 'spec_helper'

describe OpenSSL::SSL::Session do
  it 'is extended by OpenSSL::Keylog::SSLSessionExtensions' do
    expect(described_class.ancestors).to include(OpenSSL::Keylog::SessionExtensions)
  end
end
