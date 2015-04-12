require 'rake/extensiontask'

Rake::ExtensionTask.new 'openssl' do |ext|
  ext.lib_dir = 'lib/sslkeylog'
end
