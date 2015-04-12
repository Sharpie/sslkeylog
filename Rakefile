require 'rspec/core/rake_task'
require 'rake/extensiontask'

RSpec::Core::RakeTask.new(:spec)

Rake::ExtensionTask.new 'ssl_socket_extensions' do |ext|
  ext.lib_dir = 'lib/openssl/keylog'
end

task :spec => [:compile]
