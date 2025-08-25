#!/usr/bin/env ruby

# This script is used to extract the font information from the FontAwesome icons.json file
# It extracts all font definitions/names and generated the included header files,
# automating the process for generating the font names
#
# It generates the following files:
# - QtAwesomEnumGenerated.h
# - QtAwesomeGenerated.h
#
# To use it make sure the fontawesome icons.json is place in the the QtAwesome/fonts/pro/metadata folder
require 'erb'
require 'json'

class Icons
  attr_reader :icons_common,
              :icons_brands,
              :icons_pro,
              :icons_regular_free

  def initialize(icon_file)
    @icons_common = {}
    @icons_brands = {}
    @icons_pro = {}
    @icons_regular_free = {}
    build_maps(JSON.parse(File.read(icon_file)))
  end

  def id_name(str)
    "fa_#{str.downcase.gsub(/[^a-z0-9_]/, '_')}"
  end

  def namespaced_id_name(str)
    "fa::#{id_name(str)}"
  end

  def string_name(str)
    name = str.gsub("\"", "\\\"")
    "\"#{name}\""
  end

  def build_maps(icons)
    icons.each do |key, data|
      if data['free'].length > 0
        build_map_free(key, data)
      else
        build_map_pro(key, data)
      end
    end
  end

  def build_map_free(key, data)
    if data['free'].first == 'brands'
      @icons_brands[key] = data['unicode']
      add_aliasses(@icons_brands, data)
    else
      if data['free'].include?('regular')
        @icons_regular_free[key] = data['unicode']
        add_aliasses(@icons_regular_free, data)
      end
      @icons_common[key] = data['unicode']
      add_aliasses(@icons_common, data)
    end
  end

  def build_map_pro(key, data)
    @icons_pro[key] = data['unicode']
    add_aliasses(@icons_pro, data)
  end

  def add_aliasses(hash, data)
    data.dig('aliases', 'names')&.each do |alias_key|
      raise "Duplicatie key found for alias!" if hash.key?(alias_key)

      hash[alias_key] = data['unicode']
    end
  end

  def generate(template)
    ERB.new(template, trim_mode: '-').result(binding)
  end
end

icon_file = ARGV[1] || "#{__dir__}/../QtAwesome/fonts/pro/metadata/icons.json"
output_path = "#{__dir__}/../QtAwesome"

source_names = [
  'QtAwesomeEnumGenerated.h',
  'QtAwesomeStringGenerated.h'
]

icons = Icons.new(icon_file)

# generate the templates
source_names.each do |source_name|
  result = icons.generate(File.read("#{__dir__}/#{source_name}.erb"))
  File.write("#{output_path}/#{source_name}", result)
end
