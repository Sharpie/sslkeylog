require 'rake/extensiontask'

Rake::ExtensionTask.new 'ssl_socket_extensions' do |ext|
  ext.lib_dir = 'lib/openssl/keylog'
end
