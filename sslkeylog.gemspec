# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'sslkeylog/version'

Gem::Specification.new do |spec|
  spec.name          = 'sslkeylog'
  spec.version       = SSLkeylog::VERSION
  spec.authors       = ['Charlie Sharpsteen']
  spec.email         = ['source@sharpsteen.net']
  spec.license       = 'MIT'

  spec.summary       = 'A Ruby library that logs SSL session keys in NSS Key Log Format.'
  spec.homepage      = 'https://github.com/Sharpie/sslkeylog'
  spec.description   = <<-EOS
A Ruby library that logs SSL session keys from client connections in NSS
Key Log Format. This log can be used by tools such as Wireshark to decrypt
data when analyzing network traffic.
  EOS

  spec.files         = %x[git ls-files].split($/)
  spec.require_paths = ['lib']
  spec.extensions    = Dir['ext/**/extconf.rb']

  spec.required_ruby_version =  '>= 2.0.0'

  spec.add_development_dependency 'rake', '~> 0'
  spec.add_development_dependency 'rake-compiler', '~> 0.9.0'
  spec.add_development_dependency 'rspec', '~> 3.0'
end
