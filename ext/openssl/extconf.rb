require 'mkmf'
require 'shellwords'

openssl_include = []
openssl_lib     = []

# Check to see if Ruby was compiled with a custom OpenSSL
openssl_config = CONFIG['configure_args'].shellsplit.find {|e| e.start_with? '--with-openssl-dir='}
unless openssl_config.nil?
  _, _, openssl_dir = openssl_config.partition('=')

  openssl_include << File.join(openssl_dir, 'include')
  openssl_lib     << File.join(openssl_dir, 'lib')
end

dir_config('openssl', openssl_include, openssl_lib)

unless find_header('openssl/ssl.h')
  abort <<-EOS
Could not find openssl/ssl.h. Ensure the OpenSSL headers are installed.
The --with-openssl-include option can be used to search a non-standard location.
EOS
end

create_makefile('sslkeylog/openssl')
