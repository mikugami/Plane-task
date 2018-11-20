cmake_minimum_required(VERSION 3.5)
project(main)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        common.h
        glad.c
        main.cpp
        Camera.h
        Camera.cpp
        ShaderProgram.h
        ShaderProgram.cpp
        mesh.h
        mesh.cpp
        drawcall.cpp
        drawcall.h
        ImportSceneObject.cpp
        ImportSceneObject.h
        stb_image.h
        dependencies/include/Hydra/HydraVSGFExport.cpp
        dependencies/include/Hydra/HydraVSGFExport.h
        dependencies/include/TinyXML2/tinyxml2.cpp
        dependencies/include/TinyXML2/tinyxml2.h)

set(ADDITIONAL_INCLUDE_DIRS
        dependencies/include/GLAD)
set(ADDITIONAL_LIBRARY_DIRS
        dependencies/lib)   
set(ADDITIONAL_RUNTIME_LIBRARY_DIRS
        dependencies/bin)   

set (CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG}")

if(WIN32)
  set(ADDITIONAL_INCLUDE_DIRS 
        ${ADDITIONAL_INCLUDE_DIRS}
        dependencies/include)
  link_directories(${ADDITIONAL_LIBRARY_DIRS})
else()
	find_package(glfw3 REQUIRED)
endif()

include_directories(${ADDITIONAL_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)

add_executable(main ${SOURCE_FILES})

target_include_directories(main PRIVATE ${OPENGL_INCLUDE_DIR})

if (EXISTS "${PROJECT_BINARY_DIR}/shaders")
    message(STATUS "Shaders Setup: shaders folder already in place")
else()
    message(STATUS "Shaders Setup: shaders folder not in place")
    if (EXISTS "${PROJECT_SOURCE_DIR}/shaders")
        message(STATUS "Shaders Setup: Moving shaders folder to executable directory")
        file(COPY "${PROJECT_SOURCE_DIR}/shaders" DESTINATION "${PROJECT_BINARY_DIR}")
        file(REMOVE_RECURSE "${PROJECT_SOURCE_DIR}/shaders")
    else()
        message(FATAL_ERROR "Shaders Setup: Failed to find shaders folder! Try getting shaders folder from original source and place it where CMakelists.txt is located")
    endif()
endif()

if (EXISTS "${PROJECT_BINARY_DIR}/assets")
    message(STATUS "assets Setup: assets folder already in place")
else()
    message(STATUS "assets Setup: assets folder not in place")
    if (EXISTS "${PROJECT_SOURCE_DIR}/assets")
        message(STATUS "assets Setup: Moving assets folder to executable directory")
        file(COPY "${PROJECT_SOURCE_DIR}/assets" DESTINATION "${PROJECT_BINARY_DIR}")
        file(REMOVE_RECURSE "${PROJECT_SOURCE_DIR}/assets")
    else()
        message(FATAL_ERROR "assets Setup: Failed to find assets folder! Try getting assets folder from original source and place it where CMakelists.txt is located")
    endif()
endif()

if(WIN32)
  add_custom_command(TARGET main POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/dependencies/bin" $<TARGET_FILE_DIR:main>)
  #set(CMAKE_MSVCIDE_RUN_PATH ${ADDITIONAL_RUNTIME_LIBRARY_DIRS})
  target_compile_options(main PRIVATE)
  target_link_libraries(main LINK_PUBLIC ${OPENGL_gl_LIBRARY} glfw3dll)
else()
  target_compile_options(main PRIVATE -Wnarrowing)
  target_link_libraries(main LINK_PUBLIC ${OPENGL_gl_LIBRARY} glfw rt dl)
endif()

