mmachine_files = { "src/*.cpp", "src/*.hpp" }

mmachine_apps = {
  "test_player",
  "test_terrain",
  "test_quad"
}

for i, name in ipairs(mmachine_apps) do
  project(name)
  	language "C++"
  	kind "ConsoleApp"
    location "build"
  	targetdir "bin"
    includedirs "src"
  	files ( gkit_files )
  	files ( mmachine_files )
  	files { "app/" .. name .. ".cpp" }
end



