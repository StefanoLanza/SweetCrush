-- Options

-- Global settings
local workspacePath = path.join("build", _ACTION)  -- e.g. build/vs2022, build/gmake

-- Filters
local filter_vs = "action:vs*"
local filter_make = "action:gmake"
local filter_x86 = "platforms:x86"
local filter_x64 = "platforms:x86_64"
local filter_debug =  "configurations:Debug*"
local filter_release =  "configurations:Release*"

workspace ("SweetCrush")
	configurations { "Debug", "Release" }
	-- TODO Support 64 bit
	platforms { "x86", } -- "x86_64" }
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

filter "platforms:x86_64"
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
	includedirs { "external", "external/SDL2/include", }

project("inih")
	kind "StaticLib"
	files { "external/inih-master/ini.c", }
	includedirs { }

project("SweetCrush")
	targetdir "bin"
	kind "WindowedApp"
	files { "SweetCrush/src/*.*", }
	includedirs { ".", "external", "SweetCrush/src", "external/SDL2/include",}
	filter { filter_vs, filter_x64 }
		libdirs { "external/libs/x64", "external/SDL2_image/lib/x64", "external/SDL2_mixer/lib/x64" } 
	filter { filter_vs, filter_x86 }
		libdirs { "external/libs/x86", "external/glew/lib", "external/SDL2_image/lib/x86", "external/SDL2_mixer/lib/x86" } 
	links { "opengl32", "glew32", "SDL2", "SDL2_image", "SDL2main", "SDL2_mixer", "Engine", "inih", }
	--local targetDir = "%{cfg.buildtarget.directory}"
	debugdir "bin" --(path.join(workspacePath, targetDir))
