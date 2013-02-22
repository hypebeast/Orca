#!/usr/bin/env ruby 

# Copyright (C) 2012 - 2013 Sebastian Ruml <sebastian.ruml@gmail.com>
#
# This file is part of the Orca project (https://github.com/hypebeast/Orca/)
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
#   This application generates the VTOL object code for the FMU and GCS from
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
#   Copyright (c) 2012 - 2013 Sebastian Ruml. Licensed under the MIT License:
#   http://www.opensource.org/licenses/mit-license.php


require 'optparse' 
require 'ostruct'
require 'date'
require 'rexml/document'
require 'erb'


# File and path definitions
BASE_TEMPLATE_DIR = File.join(File.dirname(__FILE__), "templates")
OBJECT_DEFINITON_FILES_DIR = File.join("..", "..", "objectdefinitions")
PROJECT_ROOT_FOLDER = File.join("..", "..")
FMU_VS_PROJECT_FILE = File.join(PROJECT_ROOT_FOLDER, "flight", "OrcaFirmware", "OrcaFirmware", "OrcaFirmware.cproj")


# Application class. Handles the startup and arguments parsing.
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
    @options.modifyFMUProject = true
  end

  # Parse options, check arguments, then process the command
  def run
    if parsed_options? && arguments_valid?
      puts "Start at #{DateTime.now}"    

      # Generating IDs
      ids = generateIDs()

      # Create VTOL objects for the GCS
      GcsCodeGenerator.new(@options.outputDir, ids)   

      # Create VTOL objects for the FMU
      FmuCodeGenerator.new(@options.outputDir, ids, @options.modifyFMUProject)
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
      opts.on('-o dir', '--out dir', String, "Output directory for the generated code. Can be used for testing. No project files will be modified!") do |dir|
        @options.outputDir = dir
      end
      opts.on('-f', '--fmu', 'Generate code only for the FMU') { @options.fmuOnly = true }
      opts.on('-g', '--gcs', 'Generate code only for the ground control station') { @options.gcsOnly = true }
      opts.on('--exclude-vsproj', 'If specified the FMU project will not be modified') {@options.modifyFMUProject = false}

      opts.parse!(@arguments) rescue return false
      
      process_options
      true      
    end

    # Performs post-parse processing on options
    def process_options
      @options.verbose = false if @options.quiet
      @options.fmuOnly = false if @options.gcsOnly
      @options.gcsOnly = false if @options.fmuOnly
      @options.modifyFMUProject = false if @options.outputDir != ""
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

# This function generates for each specification file an unique ID. It returns an
# hash where the IDs are mapped to the VTOL object names.
def generateIDs()
  # Get all specification files
  pattern = File.join(OBJECT_DEFINITON_FILES_DIR, "*.xml")
  specFiles = Dir.glob(pattern)

  # Generate IDs and start with zero
  id = 0
  idHash = Hash.new
  specFiles.each do |file|
    idHash[file] = id
    id += 1
  end
  idHash
end


# Generates code for the FMU
class FmuCodeGenerator
  TARGET_FOLDER = "vtol_objects"

  def initialize(outputDir="", ids, modifyFMUProject)
    if outputDir == ""
      # Use the default output directory
      @outputDir = File.join("..", "..", "flight", "OrcaFirmware", "OrcaFirmware", "src", TARGET_FOLDER)
    else
      @outputDir = outputDir
    end

    if not File.exists? @outputDir
      Dir.mkdir(@outputDir)
    end

    # Object IDs
    @ids = ids

    # Define directories
    fmuTemplateFolder = File.join(BASE_TEMPLATE_DIR, "fmu")

    # Template files
    vtolObjectTemplateHFile = File.join(fmuTemplateFolder, "vtolobjecttemplate.h.erb")
    vtolObjectTemplateCFile = File.join(fmuTemplateFolder, "vtolobjecttemplate.c.erb")
    @objectTemplateFiles = [vtolObjectTemplateHFile, vtolObjectTemplateCFile]
    vtolObjectsInitTemplateHFile = File.join(fmuTemplateFolder, "vtolobjectsinittemplate.h.erb")
    vtolObjectsInitTemplateCFile = File.join(fmuTemplateFolder, "vtolobjectsinittemplate.c.erb")
    @objectInitTemplateFiles = {
                      vtolObjectsInitTemplateHFile => "vtol_object_init.h" ,
                      vtolObjectsInitTemplateCFile => "vtol_object_init.c"
                      }

    @generated_files = []
    @header_files = []
    @code_files = []
    @object_init_functions = []

    # Type sizes (in bytes)
    @typeSizes = {
      'ENUM' => 4,
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
      'ENUM' => 'enum',
      'INT8' => 'int8_t',
      'INT16' => 'int16_t',
      'INT32' => 'int',
      'UINT8' => 'uint8_t',
      'UINT16' => 'uint16_t',
      'UINT32' => 'uint32_t',
      'FLOAT32' => 'float' 
    }

    @FIELDTYPE_FLOAT = "FLOAT32"

    @accessTypeMappings = {
      'readonly' => 'ACCESS_READONLY',
      'readwrite' => 'ACCESS_READWRITE'
    }

    @updateModeMappings = {
      'manual' => 'UPDATE_MODE_MANUAL',
      'periodic' => 'UPDATE_MODE_PERIODIC',
      'onchange' => 'UPDATE_MODE_ONCHANGE'
    }

    puts "Generating FMU code..."

    # Find all object definition files
    specFiles = findSpecificationFiles(OBJECT_DEFINITON_FILES_DIR)
    # Generate all VTOL objects code
    generateObjectFiles(specFiles)
    # Generate object initialization code
    generateObjectInitFiles(specFiles)

    # FIXME: Quick hack!
    modifyFMUProject = true

    # Modify the Visual Studio project file
    if modifyFMUProject
      puts "Adding generated code to Visual Studio..."
      modifyVisualStudioProject()
    end

    # Done
    puts "FMU code generation done!"
  end

  # This function finds all object definition files and returns it
  def findSpecificationFiles(directory)
    pattern = File.join(directory, "*.xml")
    return Dir.glob(pattern)
  end

  # This function generates all VTOL object files. For every VTOL object
  # definition file
  def generateObjectFiles(specFiles)
    specFiles.each do |file|
      puts "Processing \"#{File.basename(file)}\"..."

      doc = REXML::Document.new(File.open(file))
      doc.root.each_element("object") do |obj|
        ####
        # Common tags
        ####

        # Try to find the right ID for this specification file
        if !@ids.has_key?(file)
          puts "Can't find ID for " + file
          exit(-1)
        end
        obj_id = @ids[file]

        xmlfile = File.basename(file)
        name = obj.attributes()['name']
        namecp = name.capitalize
        nameuc = name.upcase
        namelc = name.downcase
        objnamecp = name + "Object"
        objnameuc = objnamecp.upcase
        objnamelc = namelc + "_object"
        issingleinst = (obj.attributes()['singleinstance'] == "true") ? 1 : 0
        issettings = (obj.attributes()['settings'] == "true") ? 1 : 0 
        description = obj.elements["description"].get_text.value
        # <% flightaccess %>
        flightaccess = @accessTypeMappings[obj.elements['access'].attributes()['flight']]
        # <% gcsaccess %>
        gcsaccess = @accessTypeMappings[obj.elements['access'].attributes()['gcs']]
        # <% teleacked %>
        teleacked = (obj.elements['telemetryflight'].attributes()['acked'] == "true") ? 1 : 0
        # <% gcsteleacked %>
        gcsteleacked = (obj.elements['telemetrygcs'].attributes()['acked'] == "true") ? 1 : 0
        # <% teleupdatemode %>
        teleupdatemode = @updateModeMappings[obj.elements['telemetryflight'].attributes()['updatemode']]
        # <% gcsteleupdatemode %>
        gcsteleupdatemode = @updateModeMappings[obj.elements['telemetrygcs'].attributes()['updatemode']]
        # <% flighttele_updateperiod %>
        flighttele_updateperiod = obj.elements['telemetryflight'].attributes()['period']
        # <% gcstele_updateperiod %>
        gcstele_updateperiod = obj.elements['telemetrygcs'].attributes()['period']
        # <% logging_updateperiod %>
        logging_updateperiod = obj.elements['logging'].attributes()['period']

        ####
        # Field tags
        ####

        # Tags that will be replaced in the loop
        numbytes = 0
        datafields = ""
        enums = []
        initfields = []
        setgetfunctions = []
        setgetfunctionsextern = []

        # Parse all field elements and replace all template tags
        obj.each_element("field") do |field|
          fieldname = field.attributes()['name']
          fieldnamecp = fieldname.capitalize
          fieldnamedc = fieldname.downcase

          # <% numbytes %> tag
          numbytes += @typeSizes[field.attributes()['type']]

          # <% datafields %> tag
          if field.attributes()['type'] == "ENUM"
            datafields += "\t#{namecp}#{fieldname}Options #{fieldnamedc};\n"
          else
            datafields += "\t" + @typeMappings[field.attributes()['type']] + " #{fieldnamedc};\n"
          end

          # <% enums %> tag
          if field.attributes()['type'] == "ENUM"
            enumStr = "typedef enum {\n"
            # Go through each option
            options = field.attributes()['options'].split(',')
            m = 0
            options.each do |option|
              enumStr += "\t#{nameuc}_#{fieldname.upcase}_#{option.upcase} = #{m},"
              m += 1
              if m >= options.length
                enumStr = enumStr.chop
              end
              enumStr += "\n"
            end
            enumStr += "} #{namecp}#{fieldname}Options;"
            enums.push(enumStr)
          end

          # <% initfields %> tag
          if field.attributes()['defaultvalue'] != nil
            defaultvalue = field.attributes()['defaultvalue']
            # Handle ENUM types
            if field.attributes()['type'] == "ENUM"
              value = "#{nameuc}_#{fieldname.upcase}_#{defaultvalue.upcase}"
            else
              value = defaultvalue
            end
            data = "data->#{fieldnamedc} = #{value};"
            initfields.push(data)
          end

          # <% setgetfunctions %> tag
          # Handle enum types for the field type
          if field.attributes()['type'] == "ENUM"
            fieldtype = "#{namecp}#{field.attributes()['name']}Options"
          else
            fieldtype = @typeMappings[field.attributes()['type']]
          end

          # Create setter
          setString = "void #{namelc}_#{fieldname}_set(#{fieldtype} *new#{fieldnamecp})\n"
          setString += "{\n"
          setString += "\tvtol_set_data_field(#{objnamelc}_handle(), (void*)new#{fieldnamecp}, offsetof(#{namecp}Data_t, #{fieldnamedc}), sizeof(#{fieldtype}));\n"
          setString += "}\n"
          setgetfunctions.push(setString)

          # Create getter
          getString = "void #{namelc}_#{fieldname}_get(#{fieldtype} *new#{fieldnamecp})\n"
          getString += "{\n"
          getString += "\tvtol_get_data_field(#{objnamelc}_handle(), (void*)new#{fieldnamecp}, offsetof(#{namecp}Data_t, #{fieldnamedc}), sizeof(#{fieldtype}));\n"
          getString += "}\n"
          setgetfunctions.push(getString)

          # <% setgetfunctionsextern %> tag
          # Handle enum types for the field type
          if field.attributes()['type'] == "ENUM"
            fieldtype = "#{namecp}#{field.attributes()['name']}Options"
          else
            fieldtype = @typeMappings[field.attributes()['type']]
          end

          # Create setter
          setString = "void #{namelc}_#{fieldname}_set(#{fieldtype} *new#{fieldnamecp});"
          setgetfunctionsextern.push(setString)

          # Create getter
          getString = "void #{namelc}_#{fieldname}_get(#{fieldtype} *new#{fieldnamecp});"
          setgetfunctionsextern.push(getString)

        end
       
        # Save the initialize function name for the initialization file
        @object_init_functions.push("#{objnamelc}_initialize();")

        # Generate all source code files from the template files
        @objectTemplateFiles.each do |template|
          # Build filename
          filename = namelc
          if template.include? ".h"
            filename = filename + ".h"
          elsif template.include? ".c"
            filename = filename + ".c"
          end

          # Header file name
          headerfile = name.downcase + ".h"
          @header_files.push(headerfile)
          # Name of the include define
          incdefine = name.upcase + "_" + "OBJECT"

          erb = ERB.new(File.open(template).read)
          new_code = erb.result(binding)
          puts "Creating #{filename}"
          @generated_files.push(filename)
          outputFileName = File.join(@outputDir, "#{filename}")
          File.open(outputFileName, "w").write(new_code)
          puts "Done"
        end
      end
    end

  end

  # This function generates the initialization files
  def generateObjectInitFiles(specFiles)
    @objectInitTemplateFiles.each do |template, outputfile|
      # Header files
      headers = ""
      @header_files.uniq.each do |file|
        headers += "#include \"" + file + "\"\n"
      end

      # Initialization function names
      init_functions = ""
      @object_init_functions.each do |func|
        init_functions += "\t" + func + "\n"
      end
      init_functions = init_functions.chop

      filename = outputfile
      erb = ERB.new(File.open(template).read)
      new_code = erb.result(binding)
      puts "Creating #{filename}"
      @generated_files.push(filename)
      outputFileName = File.join(@outputDir, "#{filename}")
      File.open(outputFileName, "w").write(new_code)
      puts "Done"
    end
  end

  # This function adds all genreated VTOL object code to the Visual Studio project
  def modifyVisualStudioProject()
    doc = REXML::Document.new(File.open(FMU_VS_PROJECT_FILE))

    ##
    # Add the folder that contains the generated source code to the Visual
    # Studio project file
    ##

    folder_exists = false
    source_folder = "src\\" + TARGET_FOLDER + "\\"
    doc.root.elements.each("ItemGroup/Folder") { |element|
      if element.attributes["Include"] == source_folder
        folder_exists = true
      end
    }

    # Add new source folder if not already added
    if !folder_exists
      puts "Adding folder"

      # Find parent element
      doc.root.elements.each("ItemGroup") { |element|
        element.elements.each { |subelement|
          if subelement.to_s.include? "Folder"
            folderElt = element.add_element("Folder")           
            folderElt.attributes["Include"] = source_folder
            break
          end
        }
      }
    end

    ##
    # Add all generated source files to the ItemGroup element
    ##

    generated_files = @generated_files
    generated_files.each do |genFile|
      include_name = "src\\" + TARGET_FOLDER + "\\" + genFile
      # Check if the file is already added to the project
      found = false
      doc.root.elements.each("ItemGroup/Compile") { |element|
        if element.attributes["Include"] == include_name
          found = true
          break
        end
      }

      if !found
        # Find parent element
        doc.root.elements.each("ItemGroup") { |element|
          element.elements.each { |subelement|
            # Add file
            if subelement.to_s.include? "Compile"
              compElt = element.add_element("Compile")  
              compElt.attributes["Include"] = source_folder + genFile
              subTypeElt = compElt.add_element("SubType")
              subTypeElt.text = "compile"
              break
            end
          }
        }
      end
    end

    ##
    # Add source folder to the compiler include flag (debug and release)
    ##

    debugFound = false
    releaseFound = false

    # Find PropertyGroup for debug avrgcc include paths
    doc.root.elements.each("PropertyGroup") { |element|
      # Check for debug configuration
      if element.attributes['Condition'].to_s.include? "Debug"
        # Check if folder is already included
        element.elements.each("ToolchainSettings/AvrGcc/avrgcc.compiler.directories.IncludePaths/ListValues/Value") { |valueElement|
          if valueElement.to_s.include? "vtol_objects"
            debugFound = true
            break
          end
        }

        # If not found add it to the project file
        if !debugFound
          includeElt = element.elements["ToolchainSettings/AvrGcc/avrgcc.compiler.directories.IncludePaths/ListValues"].add_element("Value")
          includeElt.text = "../src/" + TARGET_FOLDER
        end
      end
      
      # Check for Release configuration
      if element.attributes['Condition'].to_s.include? "Release"
        # Check if folder is already included
        element.elements.each("ToolchainSettings/AvrGcc/avrgcc.compiler.directories.IncludePaths/ListValues/Value") { |valueElement|
          if valueElement.to_s.include? "vtol_objects"
            releaseFound = true
            break
          end
        }

        # If not found add it to the project file
        if !releaseFound
          includeElt = element.elements["ToolchainSettings/AvrGcc/avrgcc.compiler.directories.IncludePaths/ListValues"].add_element("Value")
          includeElt.text = "../src/" + TARGET_FOLDER
        end
      end
    }

    # Write the new xml document
    File.open(FMU_VS_PROJECT_FILE, "w") do |file|
      file<<doc.to_s
    end
  end

end


# Code generator for the Ground Control Stations
class GcsCodeGenerator

  # Initializes an instance of this class
  #
  # outputDir - The directory where the generated code files will be put
  #
  # Returns an instance of GcsCodeGenerator.
  def initialize(outputDir="", ids)
    if outputDir == ""
      # Use default output directory
      @outputDir = File.join("..", "Matunus", "src", "vtolobjects")
    else
      @outputDir = outputDir
    end

    if not File.exists? @outputDir
      Dir.mkdir(@outputDir)
    end

    @ids = ids

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
    module_names = []
    files.each do |file|
      puts "Processing \"#{File.basename(file)}\"..."

      doc = REXML::Document.new(File.open(file))
      doc.root.each_element("object") do |obj|
        ####
        # Common tags
        ####

        # Try to find the right ID for this specification file
        if !@ids.has_key?(file)
          puts "Can't find ID for " + file
          exit(-1)
        end
        obj_id = @ids[file]

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
