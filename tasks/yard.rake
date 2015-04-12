begin
  require 'yard'
  require 'yard/rake/yardoc_task'
rescue LoadError
  # YARD is not installed during CI runs.
  nil
else
  YARD::Rake::YardocTask.new(:doc)
end
