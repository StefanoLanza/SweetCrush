-- Options

-- Global settings
local workspacePath = path.join("build", _ACTION)  -- e.g. build/vs2022, build/gmake
local rootBinDir = path.join(_MAIN_SCRIPT_DIR, "bin")
local externalDir = path.join(_MAIN_SCRIPT_DIR, "external")

-- Filters
local filter_vs = "action:vs*"
local filter_make = "action:gmake"
local filter_x86 = "platforms:x86"
local filter_x64 = "platforms:x86_64"
local filter_debug =  "configurations:Debug*"
local filter_release =  "configurations:Release*"

workspace ("SweetCrush")
	configurations { "Debug", "Release" }
	platforms { "x86", "x64" }
	language "C++"
	location (workspacePath)
	characterset "MBCS"
	flags   { "MultiProcessorCompile", } --"ConformanceMode", }
	cppdialect "c++17"
	exceptionhandling "On"
	rtti "Off"
	startproject "SweetCrush"

filter "platforms:x86"
	architecture "x86"

filter "platforms:x64"
	architecture "x86_64"

filter { filter_vs }
	defines { "_HAS_EXCEPTIONS=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1", "_ENABLE_EXTENDED_ALIGNED_STORAGE", }
	buildoptions { 
		"/permissive-", 
	}
	disablewarnings { "4100" } -- unreferenced formal parameter
	system "Windows"

filter { filter_vs, filter_x86, }
	defines { "WIN32", "_WIN32", }

filter { filter_vs, filter_x64, }
	defines { "WIN64", "_WIN64", }

filter { filter_vs, filter_debug, }
	defines {   }

filter { filter_vs, filter_release, }
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
	flags   { "NoManifest", "LinkTimeOptimization", "NoBufferSecurityCheck", "NoRuntimeChecks", }
	optimize("Full")
	inlining "Auto"
	warnings "Extra"
	symbols "Off"
	runtime "Release"

filter { "toolset:gcc" }
    -- https://stackoverflow.com/questions/39236917/using-gccs-link-time-optimization-with-static-linked-libraries
    buildoptions { "-ffat-lto-objects", "-Wno-unused-parameter", }

filter { "toolset:clang", "configurations:Debug*" }
	buildoptions { "/fsanitize=address" }

project("Engine")
	kind "StaticLib"
	files { "external/engine/*.*", }
	filter "system:Windows"
		files {"external/engine/windows/**.*"}
	filter "system:linux"
		files {"external/engine/linux/**.*"}
	filter {}
	includedirs { "external", "external/SDL/include", }

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
	includedirs { ".", "external", "SweetCrush/src", "external/SDL/include",}
	-- Use precompiled libs
	filter { filter_vs }
		libdirs { "external/precompiled/windows/%{cfg.platform}" } 
	filter {}
	filter { "system:linux" }
		links { "GL", "SDL2", "SDL2_image", "SDL2main", "SDL2_mixer", "Engine", "inih", }
	filter { "system:Windows" }
	links { "opengl32", "glew32", "SDL2", "SDL2_image", "SDL2main", "SDL2_mixer", "Engine", "inih", }
	filter {}
	debugdir "bin"
	
	local precompiledDir = path.join(externalDir, "precompiled/windows/%{cfg.platform}")
	filter { filter_vs }
		postbuildcommands {
			"{ECHO}, Copying precompiled DLLS to target folder "..rootBinDir,
			"{COPYFILE} "..precompiledDir.."/*.dll "..rootBinDir,
			"{COPYFILE} "..precompiledDir.."/*.txt "..rootBinDir, -- licenses
		}
	filter {}