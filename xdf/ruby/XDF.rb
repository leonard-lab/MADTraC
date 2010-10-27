require 'rexml/document'

class XDF
  attr_accessor :filename
  attr_accessor :xml_doc
  @loaded = false

  @valid_extensions = [".xdf", ".xml"]

  def initialize(fname = "")
    @filename = fname

    unless fname.empty? or not XDF.xdf?(fname)
      load_file(@filename) unless fname.empty?
    end
    
  end

  def load_file(fname)
    return unless XDF.xdf?(fname)
    @filename = fname
    f = File.new(fname, "r")
    @xml_doc = REXML::Document.new(f)
    f.close
    @loaded = true;
  end

  def to_s
    xml_doc.to_s
  end

  def save
    return unless @loaded
    f = File.new(@filename, "w");
    @xml_doc.write(f , 4)
    f << "\n"
    f.close
	true
  end

  def print_info
    def info_print(text, level = 1)
      print " " * (level * 4)
      print "#{text}\n"
    end

    def dump_section(sec_name)
      s = get_section(sec_name)
      if not s
        info_print("(none)", 2)
      else
        s.each {|p|
          info_print("#{p.name.gsub(/_/, ' ')}:  #{p.text.strip}", 2)}
      end
    end
    
    print "XDF Info:\n"
    if not @loaded
      info_print "(no file loaded)"
    else
      info_print "File name: #{@filename}"
      info_print "Parameters:"
      dump_section('Parameters')
      info_print "Data Files:"
      dump_section('DataFiles')
      info_print "Reports:"
      dump_section('Reports')
    end
    print "\n"
  end

  def get_or_create_section(sec_name)
    return unless @loaded
    unless @xml_doc.root.elements[sec_name]
      @xml_doc.root.elements << REXML::Element.new(sec_name)
    end
    @xml_doc.root.elements[sec_name].elements
  end

  def get_section(sec_name)
    return unless @loaded and @xml_doc.root.elements[sec_name]
    @xml_doc.root.elements[sec_name].elements
  end

  def get_section_labels(sec_name)
    s = get_section(sec_name)
    return unless s
    r = []
    s.each {|e| r << e.name}
    r
  end

  def set_or_add_element_in_section(sec_name, label, value)
    s = get_or_create_section(sec_name)
    return unless s and label.respond_to?('to_s') and value.respond_to?('to_s')
    label.gsub!(/ /, '_')
    if get_section_labels(sec_name).include? label.to_s
      s[label.to_s].text = value.to_s
    else
      e = REXML::Element.new(label.to_s)
      e.text = value.to_s
      s << e
    end
    true
  end

  def get_parameter_names
    get_section_labels('Parameters')
  end

  def parameters
    get_or_create_section('Parameters')
  end
  
  def set_parameter(param_name, param_val)
    set_or_add_element_in_section('Parameters', param_name, param_val)
  end
  
  def get_data_file_labels
    get_section_labels('DataFiles')
  end

  def data_files
    get_or_create_section('DataFiles')
  end

  def get_full_path(filename)
    return unless @loaded
    File.join(File.dirname(@filename), filename)
  end

  def get_data_file_path(file_label)
    get_full_path(data_files[file_label].text)
  end

  def set_data_file(file_label, file_name)
    set_or_add_element_in_section('DataFiles', file_label, file_name)    
  end

  def get_report_labels
    get_section_labels('Reports')
  end

  def reports
    get_or_create_section 'Reports'
  end

  def set_report(report_label, report_text)
    set_or_add_element_in_section('Reports', report_label, report_text)
  end

  def self.xdf?(fname)
    unless (File.file?(fname) && File.readable?(fname) && File.writable?(fname))
      puts "XDF Error:  #{fname} is not accessible or not writable.\n"
      return false
    end

    unless @valid_extensions.include? File.extname(fname).downcase
      print "XDF Error:  #{fname} has the wrong extension.\n"
      return false
    end

    begin
      d = REXML::Document.new(File.new(fname))
      unless d.root.name =~ /ExperimentData$/
        print "XDF Error:  #{fname} Does not appear to have a "
        print "root node with name ending in ExperimentData.\n"
        return false
      end
    rescue
      puts "XDF Error:  Parsing #{fname}.\n"
      return false
    end
    
    true
  end

end
