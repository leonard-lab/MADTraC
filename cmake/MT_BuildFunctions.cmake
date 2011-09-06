function(MT_MAKE_MODULE_HEADER dir mod_name src_list)

  set(header_file ${CMAKE_BINARY_DIR}/include/${mod_name}.h)

  file(WRITE ${header_file}
    "#ifndef  MT_HAVE_${mod_name}
#define  MT_HAVE_${mod_name}

/*
 *  ${mod_name}.h
 *  MADTraC
 *
 *  Header for the ${mod_name} module of MADTraC.  This header should
 *  include any other headers you need and define the proper
 *  preprocessor definitions.
 *
 *  Created by Daniel Swain on 2/20/10.
 *    - modified to be generated by CMake, DTS 9/26/10
 *
 */

#include \"MT_Config.h\"

")

  if(MSVC)
    file(APPEND ${header_file}
      "#ifdef _MSC_VER
#pragma comment(lib, \"opengl32.lib\")
#pragma comment(lib, \"glu32.lib\")
#pragma comment(lib, \"kernel32.lib\")
#pragma comment(lib, \"user32.lib\")
#pragma comment(lib, \"gdi32.lib\")
#pragma comment(lib, \"comdlg32.lib\")
#pragma comment(lib, \"winspool.lib\")
#pragma comment(lib, \"winmm.lib\")
#pragma comment(lib, \"shell32.lib\")
#pragma comment(lib, \"comctl32.lib\")
#pragma comment(lib, \"ole32.lib\")
#pragma comment(lib, \"oleaut32.lib\")
#pragma comment(lib, \"uuid.lib\")
#pragma comment(lib, \"rpcrt4.lib\")
#pragma comment(lib, \"advapi32.lib\")
#pragma comment(lib, \"wsock32.lib\")
#pragma comment(lib, \"wininet.lib\")

#pragma comment(lib, \"MT_Core.lib\")
#endif /* _MSC_VER */

")
  endif(MSVC)

  get_directory_property(DEFS DIRECTORY ${dir} COMPILE_DEFINITIONS)
  foreach(def ${DEFS})
    string(REGEX REPLACE "=.*" "" def_name "${def}")
    string(REGEX REPLACE "=" " " def "${def}")
    file(APPEND ${header_file} "#ifndef ${def_name}\n    #define ${def}\n#endif\n\n")
  endforeach()

  foreach(sfile ${src_list})
    if(${sfile} MATCHES ".*\\.h")
      string(REPLACE "./" "MT/${mod_name}/" sfile ${sfile})
      file(APPEND ${header_file} "#include \"${sfile}\"\n")
    endif()
  endforeach()

  file(APPEND ${header_file} "\n#endif /* MT_HAVE_${mod_name} */\n")

endfunction(MT_MAKE_MODULE_HEADER)


function(MT_TRANSPOSE_FILE_IN_TREE module_name filename src_path dest_path)
  get_filename_component(tree_part "${filename}" PATH)
  # using a custom command here makes sure the headers get copied whenever they change
  add_custom_command(TARGET "${module_name}"
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${dest_path}/${tree_part}"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${filename}" "${dest_path}/${tree_part}/"
    )
endfunction(MT_TRANSPOSE_FILE_IN_TREE)

function(MT_COPY_MODULE_HEADERS module_name src_list src_path dest_path)
  foreach(sfile ${src_list})
    if(${sfile} MATCHES ".*\\.h")
      MT_TRANSPOSE_FILE_IN_TREE(${module_name} ${sfile} ${src_path} ${dest_path})
    endif()
  endforeach()
endfunction(MT_COPY_MODULE_HEADERS)