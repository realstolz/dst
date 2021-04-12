if (NOT BINARY)
  message (FATAL_ERROR "BINARY file to check not specified!")
endif ()
file (STRINGS "${BINARY}" strings REGEX "This text should be stripped out")
if (strings)
  message (FATAL_ERROR "Text not stripped from binary like it should be: ${BINARY}")
endif ()
 