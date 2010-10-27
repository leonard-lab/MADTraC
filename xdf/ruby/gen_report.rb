#!/usr/local/bin/ruby

load File.join(File.dirname(__FILE__), 'XDF.rb')

Report = Struct.new(:group,
                    :nfish,
                    :ntimes,
                    :all_times,
                    :all_names)

def usage
  print <<-'THERE'
Usage:  gen_reports filename.xdf (or) gen_reports directory
  THERE
end

def count_rows_cols(f)
  return unless f.is_a?(File)

  r = 0
  c = 0

  while line = f.gets
    r += 1
    c = [c, line.split.length].max
  end

  return r, c
  
end
  
def gen_size_report(x, key_name)
  return unless x.is_a?(XDF) && x.data_files[key_name]

  fname = x.get_data_file_path(key_name)

  unless File.file?(fname)
    print "Could not find key file #{fname}\n"
    return
  end

  f = File.new(fname)

  r, c = count_rows_cols(f)

  f.close

  g = File.basename(x.filename)[/T\d*_C\d*/]

  x.set_report('Num Fish', c)
  x.set_report('Num Times', r)
  x.set_report('Fish Group', g)

  return Report.new(g, c, r)
end

unless ARGV.size == 1
  usage
end

if File.directory?(ARGV[0])
  files = Dir[File.join(ARGV[0], "*.xdf")]
else
  files = [ ARGV[0] ]
end

inv_count = 0
count = 0
rs = {}

files.each do |the_file|
  count += 1
  unless XDF.xdf?(the_file)
    inv_count += 1
    next
  end
  x = XDF.new(the_file)
  r = gen_size_report(x, "Blob_X")
  x.save

  unless r
    inv_count += 1
    next
  end

  if not rs.include?(r.group)
    rs[r.group] = r
    rs[r.group].all_times = []
    rs[r.group].all_names = []
  else
    rs[r.group].group = r.group
    rs[r.group].ntimes += r.ntimes
  end

  rs[r.group].all_times << r.ntimes
  rs[r.group].all_names << the_file
  
end

print "Processed #{count} files\n"
if inv_count > 0
  print " " * 4
  print "Warning:  #{inv_count} were invalid and skipped.\n"
end  

rs.each_value do |r|
  print "Group #{r[:group]}:  #{r[:nfish]} fish, #{r[:ntimes]} samples, "
  print "#{r.all_times.size} files \n"
  print "   Minimum length:  #{r.all_times.min} at "
  print "#{File.basename(r.all_names[r.all_times.index(r.all_times.min)])}\n"
  print "   Maximum length:  #{r.all_times.max} at "
  print "#{File.basename(r.all_names[r.all_times.index(r.all_times.max)])}\n"
end






