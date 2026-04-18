include(FetchContent)

function(DeclareGitRepo libName gitRepo)
    FetchContent_Declare(
        ${libName}
        GIT_REPOSITORY ${gitRepo}
        GIT_TAG main
    )
endfunction()

function(DeclareLibs)
    DeclareGitRepo(Tether https://github.com/SawtoothGabe/Tether)
	FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb/)
	FetchContent_Declare(VMA GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)

	cmake_policy(SET CMP0135 NEW) # Fix DOWNLOAD_EXTRACT_TIMESTAMP warning
	FetchContent_Declare(googletest
			URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
	)
endfunction()

function(MakeStbInterface)
	FetchContent_GetProperties(stb SOURCE_DIR stb_SOURCE_DIR)

	add_library(stb INTERFACE)
	target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endfunction()

function(MakeLibsAvailable)
	if (LE_BUILD_TESTS)
		# For Windows: Prevent overriding the parent project's compiler/linker settings
		set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

		FetchContent_MakeAvailable(googletest)
	endif()

	if (NOT LE_HEADLESS)
    	if (NOT LE_TETHER_EXTERN)
    	    FetchContent_MakeAvailable(Tether)
		endif()

		if (NOT TETHER_BUILD_RENDERING)
			FetchContent_MakeAvailable(stb)
			MakeStbInterface()
		endif()

		if (LE_VULKAN_API)
			FetchContent_MakeAvailable(VMA)
		endif ()
	endif()
endfunction()

DeclareLibs()
MakeLibsAvailable()