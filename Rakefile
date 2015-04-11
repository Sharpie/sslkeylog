require 'rspec/core/rake_task'
require 'rake/extensiontask'

RSpec::Core::RakeTask.new(:spec)

Rake::ExtensionTask.new 'session_extensions' do |ext|
  ext.lib_dir = 'lib/openssl/keylog'
end

task :spec => [:compile]
