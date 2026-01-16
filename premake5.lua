-- Options

-- Global settings
local workspacePath = path.join("build", _ACTION)  -- e.g. build/vs2022, build/gmake
local rootBinDir = path.join(_MAIN_SCRIPT_DIR, "bin")
local externalDir = path.join(_MAIN_SCRIPT_DIR, "external")

-- Filters
local filter_msvc = "toolset:msc*"
local filter_make = "action:gmake"
local filter_x64 = "platforms:x64"
local filter_debug =  "configurations:Debug*"
local filter_release =  "configurations:Release*"
local filter_windows = "system:Windows"

workspace ("SweetCrush")
	configurations { "Debug", "Release" }
	platforms { "x64" }
	language "C++"
	location (workspacePath)
	characterset "MBCS"
	flags   { "MultiProcessorCompile", } --"ConformanceMode", }
	cppdialect "c++20"
	exceptionhandling "On"
	rtti "Off"
	startproject "SweetCrush"

filter "platforms:x64"
	architecture "x86_64"

filter { filter_msvc }
	defines { "_HAS_EXCEPTIONS=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1", 
		"_ENABLE_EXTENDED_ALIGNED_STORAGE", }
	buildoptions { 
		"/permissive-", 
		"/Zc:__cplusplus",    -- __cplusplus will now report 202002L (for C++20)
	}
	disablewarnings { "4100" } -- unreferenced formal parameter
	system "Windows"

filter { filter_windows, filter_x64, }
	defines { "WIN64", "_WIN64", }

filter { filter_windows, filter_debug, }
	defines {   }

filter { filter_msvc, filter_release, }
	defines { "_ITERATOR_DEBUG_LEVEL=0", "_SECURE_SCL=0", }
	
filter "configurations:Debug*"
	defines { "_DEBUG", "DEBUG", }
	flags   { "NoManifest", }
	optimize("Off")
	inlining "Default"
	warnings "Extra"
	symbols "Full"
	runtime "Debug"

filter "configurations:Release*"
	defines { "NDEBUG", }
	flags   { "NoManifest", "NoBufferSecurityCheck", "NoRuntimeChecks", }
	optimize("Full")
	inlining "Auto"
	warnings "Extra"
	symbols "Off"
	runtime "Release"
	linktimeoptimization "On"

filter { "toolset:gcc" }
    -- https://stackoverflow.com/questions/39236917/using-gccs-link-time-optimization-with-static-linked-libraries
    buildoptions { "-ffat-lto-objects", "-Wno-unused-parameter", }

filter { "toolset:clang", "configurations:Debug*" }
	buildoptions { "/fsanitize=address" }

filter {}

project("Engine")
	kind "StaticLib"
	files { "external/engine/*.*", }
	filter "system:Windows"
		includedirs { "external/SDL/include","external/SDL_mixer/include", "external/SDL_image/include", }
		files {"external/engine/windows/**.*"}
	filter "system:linux"
		includedirs { "/usr/include/SDL3",}
		files {"external/engine/linux/**.*"}
	filter {}
	includedirs { "external", }

project("inih")
	kind "StaticLib"
	files { "external/inih/ini.c", "external/inih/ini.h", }
	includedirs { }

project("gameData")
	kind "SharedLib"
	targetdir "bin"
	files { "SweetCrush/gameData/**.*", }
	includedirs { "SweetCrush", }

project("SweetCrush")
	targetdir "bin"
	filter { "system:Windows" }
		kind "WindowedApp"
	filter { "system:linux" }
		kind "ConsoleApp"
	filter {}
	files { "SweetCrush/src/*.*", }
	includedirs { ".", "external", "SweetCrush/src", }
	-- Use precompiled libs
	filter { filter_msvc }
		libdirs { "external/precompiled/windows/%{cfg.platform}" } 
	filter {}
	filter { "system:linux" }
		includedirs { "/usr/include/SDL3",}
		links { "GL", "SDL3", "SDL3_image", "SDL3_mixer", "Engine", "inih", "dl", }
	filter { filter_windows }
		includedirs { "external/SDL/include", "external/SDL_Mixer/include", }
		links { "opengl32", "glew32", "SDL3", "SDL3_image", "SDL3_mixer", "Engine", "inih", "gameData", }
	filter {}
	debugdir "bin"
	
	local precompiledDir = path.join(externalDir, "precompiled/windows/%{cfg.platform}")
	filter { filter_msvc }
		postbuildcommands {
			"{ECHO}, Copying precompiled DLLS to target folder "..rootBinDir,
			"{COPYFILE} "..precompiledDir.."/*.dll "..rootBinDir,
			"{COPYFILE} "..precompiledDir.."/*.txt "..rootBinDir, -- licenses
		}
	filter {}