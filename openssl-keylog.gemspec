# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'openssl/keylog/version'

Gem::Specification.new do |spec|
  spec.name          = 'openssl-keylog'
  spec.version       = OpenSSL::Keylog::VERSION
  spec.authors       = ['Charlie Sharpsteen']
  spec.email         = ['source@sharpsteen.net']

  spec.summary       = 'An extension to the Ruby OpenSSL library that logs session keys in NSS Key Log Format.'
  spec.homepage      = 'https://github.com/Sharpie/openssl-keylog'

  spec.files         = Dir['lib/**/*.rb', 'ext/**/*.c']
  spec.require_paths = ['lib']
  spec.extensions    = Dir['ext/**/extconf.rb']

  spec.add_development_dependency 'rake'
  spec.add_development_dependency 'rake-compiler'
  spec.add_development_dependency 'rspec', '~> 3.0'
end
