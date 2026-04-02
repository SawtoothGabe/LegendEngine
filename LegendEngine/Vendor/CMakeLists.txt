cmake_minimum_required (VERSION 3.10)

include(FetchContent)

# Config
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_LIST_DIR})

function(DeclareGitRepo libName gitRepo)
    FetchContent_Declare(
        ${libName}
        GIT_REPOSITORY ${gitRepo}
        SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/${libName}
        BINARY_DIR ${CMAKE_BINARY_DIR}/Vendor/${libName}-build
        SUBBUILD_DIR ${CMAKE_BINARY_DIR}/Vendor/${libName}-subbuild
    )
endfunction()

function(DeclareLibs)
    DeclareGitRepo(Tether https://github.com/SawtoothGabe/Tether)

	cmake_policy(SET CMP0135 NEW) # Fix DOWNLOAD_EXTRACT_TIMESTAMP warning
	FetchContent_Declare(googletest
			URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
			SOURCE_DIR ${FETCHCONTENT_BASE_DIR}/googletest
			BINARY_DIR ${CMAKE_BINARY_DIR}/Vendor/googletest-build
			SUBBUILD_DIR ${CMAKE_BINARY_DIR}/Vendor/googletest-subbuild
	)
endfunction()

function(MakeLibsAvailable)
	if (LE_BUILD_TESTS)
		# For Windows: Prevent overriding the parent project's compiler/linker settings
		set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

		FetchContent_MakeAvailable(googletest)
	endif()

    if (NOT LE_TETHER_EXTERN)
        if (LE_VULKAN_API)
            option(TETHER_RENDERING_VULKAN_API ON)
            set(TETHER_RENDERING_VULKAN_API ON)
        endif()
        option(TETHER_BUILD_RENDERING ON)
        set(TETHER_BUILD_RENDERING ON)

        FetchContent_MakeAvailable(Tether)
    endif()
endfunction()

DeclareLibs()
MakeLibsAvailable()