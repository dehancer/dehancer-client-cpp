
# -fobjc-arc ?
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ObjC++ -fobjc-arc -Wno-unused-command-line-argument")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -F/Library/Frameworks -framework Metal ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework CoreImage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework AppKit")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework ServiceManagement")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Foundation ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Cocoa")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Security")
