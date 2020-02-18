# Install script for directory: E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/code/Debug/assimp-vc142-mtd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/code/Release/assimp-vc142-mt.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/code/MinSizeRel/assimp-vc142-mt.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/code/RelWithDebInfo/assimp-vc142-mt.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/code/Debug/assimp-vc142-mtd.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/code/Release/assimp-vc142-mt.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/code/MinSizeRel/assimp-vc142-mt.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/code/RelWithDebInfo/assimp-vc142-mt.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/anim.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/aabb.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/ai_assert.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/camera.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/color4.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/color4.inl"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/code/../include/assimp/config.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/defs.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Defines.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/cfileio.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/light.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/material.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/material.inl"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/matrix3x3.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/matrix3x3.inl"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/matrix4x4.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/matrix4x4.inl"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/mesh.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/pbrmaterial.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/postprocess.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/quaternion.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/quaternion.inl"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/scene.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/metadata.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/texture.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/types.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/vector2.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/vector2.inl"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/vector3.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/vector3.inl"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/version.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/cimport.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/importerdesc.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Importer.hpp"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/DefaultLogger.hpp"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/ProgressHandler.hpp"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/IOStream.hpp"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/IOSystem.hpp"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Logger.hpp"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/LogStream.hpp"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/NullLogger.hpp"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/cexport.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Exporter.hpp"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/DefaultIOStream.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/DefaultIOSystem.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/ZipArchiveIOSystem.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/SceneCombiner.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/fast_atof.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/qnan.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/BaseImporter.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Hash.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/MemoryIOWrapper.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/ParsingUtils.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/StreamReader.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/StreamWriter.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/StringComparison.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/StringUtils.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/SGSpatialSort.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/GenericProperty.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/SpatialSort.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/SkeletonMeshBuilder.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/SmoothingGroups.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/SmoothingGroups.inl"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/StandardShapes.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/RemoveComments.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Subdivision.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Vertex.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/LineSplitter.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/TinyFormatter.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Profiler.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/LogAux.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Bitmap.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/XMLTools.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/IOStreamBuffer.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/CreateAnimMesh.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/irrXMLWrapper.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/BlobIOSystem.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/MathFunctions.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Macros.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Exceptional.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/ByteSwapper.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Compiler/pushpack1.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Compiler/poppack1.h"
    "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/src/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/code/Debug/assimp-vc142-mtd.pdb")
  endif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/code/RelWithDebInfo/assimp-vc142-mt.pdb")
  endif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
endif()

