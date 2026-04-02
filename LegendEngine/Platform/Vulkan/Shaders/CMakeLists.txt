cmake_minimum_required (VERSION 3.10)

set(COMPILED_SHADERS_DIR "${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders")
set(SHADER_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/ShaderData")
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}")

define_property(DIRECTORY PROPERTY SPV_FILES)
set_property(GLOBAL APPEND PROPERTY SPV_HEADERS_DIR "${SHADER_HEADER_DIR}")

function(Compile)
	add_custom_target(compile_vulkan_shaders)

	# Find all shader files
	file(GLOB_RECURSE SHADERS "${SHADER_SOURCE_DIR}/*.vert" "${SHADER_SOURCE_DIR}/*.frag")

	# Make the shader bin directory (if it doesn't exist)
	file(MAKE_DIRECTORY ${COMPILED_SHADERS_DIR})

	# Compile all shaders with glslc
	foreach(source IN LISTS SHADERS)
		# Get the name of the file without path (with extension)
		get_filename_component(FILENAME ${source} NAME)

		message(STATUS "Adding command to compile shader ${FILENAME}")

		# Run glslc for each shader
		add_custom_command(
				TARGET compile_vulkan_shaders
				PRE_BUILD
				COMMAND
				glslc --target-env=vulkan1.3
				# Output file
				-o \"${COMPILED_SHADERS_DIR}/${FILENAME}.spv\"
				# Source file
				\"${source}\"
		)

		set_property(DIRECTORY APPEND PROPERTY SPV_FILES "${COMPILED_SHADERS_DIR}/${FILENAME}.spv")
	endforeach()
endfunction()

function(ConvertToCpp)
	add_custom_target(spv2cpp)
	add_dependencies(spv2cpp bin2cpp compile_vulkan_shaders)

	file(MAKE_DIRECTORY ${SHADER_HEADER_DIR})

	get_property(spvFiles DIRECTORY PROPERTY SPV_FILES)
	foreach(source IN LISTS spvFiles)
		# Get the name of the file without path (with extension)
		get_filename_component(FILENAME ${source} NAME)

		message(STATUS "Adding command to convert ${FILENAME} to C++")

		add_custom_command(
			TARGET spv2cpp
			PRE_BUILD
			COMMAND
				bin2cpp
				-s ${source}
				-o \"${SHADER_HEADER_DIR}/${FILENAME}.h\"
				-n le::vk
		)
	endforeach()
endfunction()

Compile()
ConvertToCpp()
