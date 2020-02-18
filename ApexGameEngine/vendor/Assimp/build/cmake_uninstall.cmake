IF(NOT EXISTS "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/install_manifest.txt")
  MESSAGE(FATAL_ERROR "Cannot find install manifest: \"E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/install_manifest.txt\"")
ENDIF(NOT EXISTS "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/install_manifest.txt")

FILE(READ "E:/Athang-Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/install_manifest.txt" files)
STRING(REGEX REPLACE "\n" ";" files "${files}")
FOREACH(file ${files})
  MESSAGE(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
  EXEC_PROGRAM(
    "E:/Athang-Work/cmake-3.17.0-rc1-win64-x64/bin/cmake.exe" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
    OUTPUT_VARIABLE rm_out
    RETURN_VALUE rm_retval
    )
  IF(NOT "${rm_retval}" STREQUAL 0)
    MESSAGE(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
  ENDIF(NOT "${rm_retval}" STREQUAL 0)
ENDFOREACH(file)
