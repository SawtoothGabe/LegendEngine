set(SHADER_OUTPUT_DIR Shaders)

set (BUILD_SHADER_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_OUTPUT_DIR}"
)

file(MAKE_DIRECTORY ${BUILD_SHADER_DIR})

if (LE_DX11_API OR LE_DX12_API)
    set(DXIL_OPTION --dxil)
endif ()

if (LE_OPENGL_API)
    set(GLSL_OPTION --GLSL)
endif ()

if (LE_VULKAN_API)
    set(SPIRV_OPTION --spirv)
endif ()

if (LE_WEBGPU_API)
    set(WGSL_OPTION --wgsl)
endif ()

function(get_shader_dependencies out_var)
    set(all_deps "")

    # Loop through all shader files passed as arguments
    foreach(shader_file ${ARGN})
        file(READ "${shader_file}" shader_content)

        # Find all #include "..." lines
        string(REGEX MATCHALL "#include[ \t]+\"([^\"]+)\"" includes "${shader_content}")

        foreach(include_line ${includes})
            string(REGEX REPLACE "#include[ \t]+\"([^\"]+)\"" "\\1" include_file "${include_line}")

            # Resolve relative to shader file's directory
            get_filename_component(shader_dir "${shader_file}" DIRECTORY)
            set(include_path "${shader_dir}/${include_file}")
            get_filename_component(include_path "${include_path}" ABSOLUTE)

            list(APPEND all_deps "${include_path}")
        endforeach()
    endforeach()

    # Remove duplicates
    list(REMOVE_DUPLICATES all_deps)

    set(${out_var} ${all_deps} PARENT_SCOPE)
endfunction()

function (le_add_shaders_target name shaders)
    foreach (shader IN LISTS shaders)
        # Get the name of the file without path (with extension)
        get_filename_component(FILENAME ${shader} NAME)

        # Get just the extension
        get_filename_component(EXTENSION ${shader} EXT)

        if (${EXTENSION} MATCHES "json")
            get_filename_component(config_dir ${shader} DIRECTORY)

            # Read JSON file
            file(READ "${shader}" json_content)

            string(JSON moduleCount LENGTH ${json_content} "modules")
            set(modules "")
            if(moduleCount GREATER 0)
                math(EXPR last_index "${moduleCount} - 1")
                foreach(i RANGE ${last_index})
                    string(JSON item GET ${json_content} "modules" ${i})

                    set(module_path "${config_dir}/${item}")
                    get_filename_component(module_path "${module_path}" ABSOLUTE)
                    list(APPEND modules ${module_path})
                endforeach()
            endif()
            
            get_shader_dependencies(includes ${modules})
        else ()
            get_shader_dependencies(includes ${shader})
        endif()

        add_custom_command(
            OUTPUT
                "${BUILD_SHADER_DIR}/${FILENAME}.cpp"
            DEPENDS
                ${shader}
                ${modules}
                ${includes}
            COMMAND
                lesh
                ${DXIL_OPTION}
                ${GLSL_OPTION}
                ${SPIRV_OPTION}
                ${WGSL_OPTION}
                # Output file
                -o \"${BUILD_SHADER_DIR}/${FILENAME}.cpp\"
                # Source file
                \"${shader}\"
        )

        list(APPEND SHADER_CPP_FILES "${BUILD_SHADER_DIR}/${FILENAME}.cpp")
    endforeach ()

    add_library(${name} STATIC ${SHADER_CPP_FILES})
    target_include_directories(${name} PUBLIC "${LEGENDENGINE_BASE_DIR}/LegendShaders/include")
endfunction()

file(GLOB_RECURSE SHADER_CONFIGS "${LEGENDENGINE_BASE_DIR}/LegendEngine/Assets/Shaders/*.json")
le_add_shaders_target(compile-shaders "${SHADER_CONFIGS}")
