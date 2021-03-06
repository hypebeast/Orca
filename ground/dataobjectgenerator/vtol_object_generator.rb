#!/usr/bin/env ruby 

# Copyright (C) 2012 Sebastian Ruml <sebastian.ruml@gmail.com>
#
# This file is part of the Matunus project (part of the orcacopter project)
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 1, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# == Synopsis 
#   This application generates the data object code for the FMU and GCS from the
#   object definition files.
#
# == Examples
#   TODO
#
# == Usage 
#   vtol_object_generator [options]
#
#   For help use: vtol_object_generator -h
#
# == Options
#   -h, --help          Displays help message
#   -v, --version       Display the version, then exit
#   -q, --quiet         Output as little as possible, overrides verbose
#   -V, --verbose       Verbose output
#
# == Author
#   Sebastian Ruml
#
# == Copyright
#   Copyright (c) 2012 Sebastian Ruml. Licensed under the MIT License:
#   http://www.opensource.org/licenses/mit-license.php


require 'optparse' 
require 'ostruct'
require 'date'
require 'rexml/document'
require 'erb'


BASE_TEMPLATE_DIR = File.join(File.dirname(__FILE__), "templates")
OBJECT_DEFINITON_FILES_DIR = File.join("..", "..", "objectdefinitions")


class App
  VERSION = '0.0.1'
  
  attr_reader :options

  def initialize(arguments, stdin)
    @arguments = arguments
    
    # Set defaults
    @options = OpenStruct.new
    @options.verbose = false
    @options.quiet = false
    @options.outputDir = ""
    @options.fmuOnly = false
    @options.gcsOnly = false
  end

  # Parse options, check arguments, then process the command
  def run
    if parsed_options? && arguments_valid?
      puts "Start at #{DateTime.now}"    

      # Create VTOL objects for the GCS
      GcsCodeGenerator.new(@options.outputDir)   

      # Create VTOL objects for the FMU
      FmuCodeGenerator.new(@options.outputDir)
    else
      exit 0
    end
  end        
  
  private
  
    def parsed_options?
      
      # Specify options
      opts = OptionParser.new 
      opts.on('-v', '--version', "Show version")        { output_version ; exit 0 }
      opts.on('-V', '--verbose', "Run verbosely")       { @options.verbose = true }  
      opts.on('-q', '--quiet', "Run quietly")           { @options.quiet = true }
      opts.on('-h', '--help', "Show this message")      { puts opts; exit 0 }
      opts.on('-o dir', '--out dir', String, "Output directory for the generated code") do |dir|
        @options.outputDir = dir
      end
      opts.on('-f', '--fmu', 'Generate code only for the FMU') { @options.fmuOnly = true }
      opts.on('-g', '--gcs', 'Generate code only for the ground control station') { @options.gcsOnly = true }

      opts.parse!(@arguments) rescue return false
      
      process_options
      true      
    end

    # Performs post-parse processing on options
    def process_options
      @options.verbose = false if @options.quiet
      @options.fmuOnly = false if @options.gcsOnly
      @options.gcsOnly = false if @options.fmuOnly
    end
    
    # True if required arguments were provided
    def arguments_valid?
      true
    end
    
    # Setup the arguments
    def process_arguments
      # TO DO - place in local vars, etc
    end
    
    def output_version
      puts "#{File.basename(__FILE__)} Version #{VERSION}"
    end

end

# This function reads all specification files and generates for every VTOL
# object an ID.
def processVTOLSpecifications(directory) 
  # TODO
end 

class FmuCodeGenerator

  def initialize(outputDir="")
    if outputDir == ""
      # Use the default output directory
      @outputDir = File.join("..", "..", "flight", "OrcaFirmware", "OrcaFirmware", "src", "vtol_objects")
    else
      @outputDir = outputDir
    end

    if not File.exists? @outputDir
      Dir.mkdir(@outputDir)
    end

    # Directory and file definitions
    vtolObjectTemplateHFile = File.join(BASE_TEMPLATE_DIR, "fmu", "vtolobjecttemplate.h.erb")
    vtolObjectTemplateCFile = File.join(BASE_TEMPLATE_DIR, "fmu", "vtolobjecttemplate.c.erb")
    @objectTemplateFiles = [vtolObjectTemplateHFile, vtolObjectTemplateCFile]
    vtolObjectsInitTemplateHFile = File.join(BASE_TEMPLATE_DIR, "fmu", "vtolobjectsinittemplate.h.erb")
    vtolObjectsInitTemplateCFile = File.join(BASE_TEMPLATE_DIR, "fmu", "vtolobjectsinittemplate.c.erb")
    @objectInitTemplateFiles = {
                      vtolObjectsInitTemplateHFile => "vtol_object_init.h" ,
                      vtolObjectsInitTemplateCFile => "vtol_object_init.c"
                      }

    @header_files = []
    @object_init_functions = []

    # Type sizes (in bytes)
    @typeSizes = {
      'INT8' => 1,
      'INT16' => 2,
      'INT32' => 4,
      'UINT8' => 1,
      'UINT16' => 2,
      'UINT32' => 4,
      'FLOAT32' => 8
    }

    # Type mappings
    @typeMappings = {
      'INT8' => 'int8_t',
      'INT16' => 'int16_t',
      'INT32' => 'int',
      'UINT8' => 'uint8_t',
      'UINT16' => 'uint16_t',
      'UINT32' => 'uint32_t',
      'FLOAT32' => 'float' 
    }

    @FIELDTYPE_FLOAT = "FLOAT32"

    puts "Generating code for FMU..."

    # Find all object definition files
    specFiles = findSpecificationFiles(OBJECT_DEFINITON_FILES_DIR)
    # Generate all VTOL objects code
    generateObjectFiles(specFiles)
    # Generate object initialization code
    generateObjectInitFiles(specFiles)

    puts "Generating Done."
  end

  # This function finds all object definition files and returns it
  def findSpecificationFiles(directory)
    pattern = File.join(directory, "*.xml")
    return Dir.glob(pattern)
  end

  def generateObjectFiles(specFiles)
    obj_id = 1

    specFiles.each do |file|
      puts "Processing \"#{File.basename(file)}\"..."

      doc = REXML::Document.new(File.open(file))
      doc.root.each_element("object") do |obj|
        # Get all info from the specification
        xmlfile = File.basename(file)
        name = obj.attributes()['name']
        objnamecp = obj.attributes()['name'] + "Object"
        objnameuc = objnamecp.upcase
        objnamelc = obj.attributes()['name'].downcase + "_object"
        issingleinst = (obj.attributes()['singleinstance'] == "true") ? 1 : 0
        issettings = (obj.attributes()['settings'] == "true") ? 1 : 0 
        description = obj.elements["description"].get_text.value
        
        # <% numbytes %> tag
        numbytes = 0
        obj.each_element("field") do |field|
          numbytes += @typeSizes[field.attributes()['type']]
        end

        # <% datafields %> tag
        datafields = []
        obj.each_element("field") do |field|
          data = {
            'name' => field.attributes()['name'],
            'type' => @typeMappings[field.attributes()['type']]
          }
          datafields.push(data)
        end

        # <% enums %> tag
        # TODO: Needs rework!!
        enums = []
        m = 1
        obj.each_element("field") do |field|
          if field.attributes()['type'] == "enum"
            data = {
              'start' => "typedef enum {",
              'stop' => "} #{name}#{field.attributes()['name']}Options;"
            }
            enums.push(data)
            m = m +1
          end
        end 

        # <% initfields %> tag
        initfields = []
        obj.each_element("field") do |field|
          if field.attributes()['defaultvalue'] != nil
            data = "data.#{field.attributes()['name']} = #{field.attributes()['defaultvalue']}"
            initfields.push(data)
          end
        end

        # <% setgetfunctions %> tag
        setgetfunctions = []
        obj.each_element("field") do |field|
          objectname = name.downcase
          fieldname = field.attributes()['name'].downcase
          fieldnamecp = field.attributes()['name'].capitalize
          fieldtype = @typeMappings[field.attributes()['type']]

          # Create setter
          setString = "void #{objectname}_#{fieldname}_set(#{fieldtype} *new#{fieldnamecp})\n"
          setString += "{\n"
          setString += "\tvtol_set_data_field(#{objnamelc}_handle(), (void*)new#{fieldnamecp}, offsetof(#{objnamecp}Data_t, #{fieldname}), sizeof(#{fieldtype}));\n"
          setString += "}\n"
          setgetfunctions.push(setString)

          # Create getter
          getString = "void #{objectname}_#{fieldname}_get(#{fieldtype} *new#{fieldnamecp})\n"
          getString += "{\n"
          getString += "\tvtol_get_data_field(#{objnamelc}_handle(), (void*)new#{fieldnamecp}, offsetof(#{objnamecp}Data_t, #{fieldname}), sizeof(#{fieldtype}));\n"
          getString += "}\n"
          setgetfunctions.push(getString)
        end

        # <% setgetfunctionsextern %> tag
        setgetfunctionsextern = []
        obj.each_element("field") do |field|
          objectname = name.downcase
          fieldname = field.attributes()['name']
          fieldnamecp = field.attributes()['name'].capitalize
          fieldtype = @typeMappings[field.attributes()['type']]

          # Create setter
          setString = "void #{objectname}_#{fieldname}_set(#{fieldtype} *new#{fieldnamecp});"
          setgetfunctionsextern.push(setString)

          # Create getter
          getString = "void #{objectname}_#{fieldname}_get(#{fieldtype} *new#{fieldnamecp});"
          setgetfunctionsextern.push(getString)
        end

        @object_init_functions.push("#{objnamelc}_initialize();")

        @objectTemplateFiles.each do |template|
          # Build filename
          filename = name.downcase + "_" + "object"
          if template.include? ".h"
            filename = filename + ".h"
          elsif template.include? ".c"
            filename = filename + ".c"
          end

          # Header file name
          headerfile = name.downcase + "_" + "object" + ".h"
          @header_files.push(headerfile)
          # Name of the include define
          incdefine = name.upcase + "_" + "OBJECT"

          erb = ERB.new(File.open(template).read)
          new_code = erb.result(binding)
          puts "Creating #{filename}"
          outputFileName = File.join(@outputDir, "#{filename}")
          File.open(outputFileName, "w").write(new_code)
          puts "Done"
        end
      end
    end

  end

  def generateObjectInitFiles(specFiles)
    @objectInitTemplateFiles.each do |template, outputfile|
      headers = @header_files.uniq
      init_functions = @object_init_functions.uniq
      filename = outputfile
      erb = ERB.new(File.open(template).read)
      new_code = erb.result(binding)
      puts "Creating #{filename}"
      outputFileName = File.join(@outputDir, "#{filename}")
      File.open(outputFileName, "w").write(new_code)
      puts "Done"
    end
  end

  # This function adds all genreated FMU code to the Visual Studio project
  def addGeneratedCodeToVisualStudioProject(files)
    # TODO
  end

end

class GcsCodeGenerator

  # Initializes an instance of this class
  #
  # outputDir - The directory where the generated code files will be put
  #
  # Returns an instance of GcsCodeGenerator.
  def initialize(outputDir="")
    if outputDir == ""
      # Use default output directory
      @outputDir = File.join("..", "Matunus", "src", "vtolobjects")
    else
      @outputDir = outputDir
    end

    if not File.exists? @outputDir
      Dir.mkdir(@outputDir)
    end

    # Directory and files definitions
    @templateFile = File.join(BASE_TEMPLATE_DIR, "gcs", "vtolobjecttemplate.py.erb")
    @modulesFileName = File.join(@outputDir, "objectmodules.txt")

    @typeDefinitions = {
      'INT8' => 0,
      'INT16' => 1,
      'INT32' => 2,
      'UINT8' => 3,
      'UINT16' => 4,
      'UINT32' => 5,
      'FLOAT32' => 6
    }

    # Find all object definition files
    specFiles = findSpecificationFiles(OBJECT_DEFINITON_FILES_DIR)
    # Process all definitions files
    generateObjectFiles(specFiles)
  end

  # This function finds all object definition files and returns it
  def findSpecificationFiles(directory)
    pattern = File.join(directory, "*.xml")
    return Dir.glob(pattern)
  end

  def generateObjectFiles(files)

    obj_id = 1
    module_names = []
    files.each do |file|
      puts "Processing \"#{File.basename(file)}\"..."

      doc = REXML::Document.new(File.open(file))
      doc.root.each_element("object") do |obj|
        xmlfile = File.basename(file)
        name = obj.attributes()['name']
        obj_name = obj.attributes()['name'] + "Object"
        singleinstance = obj.attributes()['singleinstance']
        settings = obj.attributes()['settings']
        description = obj.elements["description"].get_text.value

        fields = []
        obj.each_element("field") do |field|
          field = {
            'name' => field.attributes()['name'],
            'units' => field.attributes()['units'],
            'type' => @typeDefinitions[field.attributes()['type']]
          }
          fields.push(field)
        end

        erb = ERB.new(File.open(@templateFile).read)
        new_code = erb.result(binding)
        fileName = "#{obj_name}.py"
        puts "Creating #{fileName}"
        outputFileName = File.join(@outputDir, "#{obj_name}.py")
        File.open(outputFileName, "w").write(new_code)
        puts "Done"

        module_names.push("#{obj_name}")
        obj_id += 1
      end
    end

    # Create a file with the names of all generated modules
    File.open(@modulesFileName, 'w') do |f|
      module_names.each do |line|
        f.puts line
      end
    end
  end

end


# Create and run the application
app = App.new(ARGV, STDIN)
app.run