require 'rspec/core/rake_task'
require 'rake/extensiontask'
require 'yard'
require 'yard/rake/yardoc_task'

RSpec::Core::RakeTask.new(:spec)

Rake::ExtensionTask.new 'ssl_socket_extensions' do |ext|
  ext.lib_dir = 'lib/openssl/keylog'
end

YARD::Rake::YardocTask.new(:doc)

task :spec => [:compile]
