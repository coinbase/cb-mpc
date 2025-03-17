macro(set_flags var flag)
  set(${var} "${${var}} ${flag}")
endmacro()

macro(set_cxx_flags flag)
  set_flags(CMAKE_CXX_FLAGS ${flag})
endmacro()

macro(set_asm_flags flag)
  set_flags(CMAKE_ASM_FLAGS ${flag})
endmacro()

macro(set_link_flags flag)
  set_flags(CMAKE_SHARED_LINKER_FLAGS ${flag})
  set_flags(CMAKE_EXE_LINKER_FLAGS ${flag})
endmacro()
