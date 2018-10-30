#!lua

pd = "../Project"

-- A solution contains projects, and defines the available configurations
solution "Project"
  configurations { "debug", "release" }
  platforms { "native", "x64", "x32" }
  location ( "./" ) -- Location of the solutions
  
  --buildoptions("-std=c++11")
  buildoptions_cpp("-std=c++14")
  buildoptions_objcpp("-std=c++14")

  includedirs { 
    pd .. "/common/include", 
    pd .. "/common/dependencies",
    pd .. "/common/dependencies/SFML/include",
    pd .. "/Server/include",
    pd .. "/Client/include"
  }

  libdirs {
    pd .. "/common/dependencies/SFML/lib"
  }

  -- Project Server
  local sbp1 = "Server"
  project "Server"
    kind "ConsoleApp"
    language "C++"
    location ( pd .. "/" .. sbp1 ) 
    targetdir (pd .. "/" .. sbp1 .. "/bin/")
    --objdir ("obj/" .. sbp1)
    
    files{ 
      group = "include", 
      pd .. "/common/include/**.h",
      pd .. "/" .. sbp1 .. "/include/**.h"
    } -- include filter and get the files
    files{ 
      group = "src", 
      pd .. "/common/src/**.cpp",
      pd .. "/" .. sbp1 .. "/src/**.cpp"
    } -- src filter and get the files
  
    configuration { "windows" }
      defines { "__PLATFORM_WINDOWS__" }
      buildoptions_cpp("/Y-")
      windowstargetplatformversion "10.0.15063.0"
       
    configuration { "macosx" }
      links  {
        "Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework",
      }
      linkoptions { "-framework Cocoa","-framework QuartzCore", "-framework OpenGL", "-framework OpenAL" }
      defines { "__PLATFORM_MACOSX__" }
       
    configuration { "linux" }
      links {
        "X11", "Xrandr", "Xcursor", "Xinerama", "Xi", "Xxf86vm", "rt", "pthread", "GL", "glut", "GLU", "m"
      }
      includedirs {
        "/usr/include/GL/"
      }
      libdirs {
        "/usr/bin/",
        "/usr/lib/"
      }
      defines { "__PLATFORM_LINUX__" }

    configuration "debug"
      defines { "DEBUG" }
      flags { "Symbols", "ExtraWarnings" }
      links {
        "sfml-system-d",
        "sfml-window-d",
        "sfml-graphics-d",
        "sfml-audio-d",
        "sfml-network-d",
      }

    configuration "release"
      defines { "NDEBUG" }
      flags { "Optimize", "ExtraWarnings" }
      links {
        "sfml-system",
        "sfml-window",
        "sfml-graphics",
        "sfml-audio",
        "sfml-network",
      }


  -- Project Client
  local sbp2 = "Client"
  project "Client"
    kind "ConsoleApp"
    language "C++"
    location ( pd .. "/" .. sbp2 ) 
    targetdir (pd .. "/" .. sbp2 .. "/bin/")
    --objdir ("obj/" .. sbp2)
    
    files{ 
      group = "include", 
      pd .. "/common/include/**.h",
      pd .. "/" .. sbp2 .. "/include/**.h"
    } -- include filter and get the files
    files{ 
      group = "src", 
      pd .. "/common/src/**.cpp",
      pd .. "/" .. sbp2 .. "/src/**.cpp"
    } -- src filter and get the files
  
    configuration { "windows" }
      defines { "__PLATFORM_WINDOWS__" }
      buildoptions_cpp("/Y-")
      windowstargetplatformversion "10.0.15063.0"
       
    configuration { "macosx" }
      links  {
        "Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework",
      }
      linkoptions { "-framework Cocoa","-framework QuartzCore", "-framework OpenGL", "-framework OpenAL" }
      defines { "__PLATFORM_MACOSX__" }
       
    configuration { "linux" }
      links {
        "X11", "Xrandr", "Xcursor", "Xinerama", "Xi", "Xxf86vm", "rt", "pthread", "GL", "glut", "GLU", "m"
      }
      includedirs {
        "/usr/include/GL/"
      }
      libdirs {
        "/usr/bin/",
        "/usr/lib/"
      }
      defines { "__PLATFORM_LINUX__" }


    configuration "debug"
      defines { "DEBUG" }
      flags { "Symbols", "ExtraWarnings" }
      links {
        -- "sfml-system-d",
        -- "sfml-window-d",
        -- "sfml-graphics-d",
        -- "sfml-audio-d",
        -- "sfml-network-d",
        "sfml-system",
        "sfml-window",
        "sfml-graphics",
        "sfml-audio",
        "sfml-network",
      }

    configuration "release"
      defines { "NDEBUG" }
      flags { "Optimize", "ExtraWarnings" }
      links {
        "sfml-system",
        "sfml-window",
        "sfml-graphics",
        "sfml-audio",
        "sfml-network",
      }