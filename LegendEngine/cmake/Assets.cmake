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

function (le_add_shaders_target name shaders)
    foreach (shader IN LISTS shaders)
        # Get the name of the file without path (with extension)
        get_filename_component(FILENAME ${shader} NAME)

        add_custom_command(
            OUTPUT "${BUILD_SHADER_DIR}/${FILENAME}.cpp"
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
