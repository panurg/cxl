cmake_minimum_required (VERSION 3.0)
project (cxl VERSION 0.1.0)
find_package(X11 REQUIRED COMPONENTS Xkbfile)
add_executable(${PROJECT_NAME} cxl.c)
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES} ${X11_Xkbfile_LIB})
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
find_program(A2X NAMES a2x a2x.py)
find_program(BZIP2 bzip2)
set(MAN ${PROJECT_NAME}.1)
set(MAN_TXT ${MAN}.txt)
set(MAN_TXT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${MAN_TXT})
set(MAN_COMPRESSED ${MAN}.bz2)
add_custom_command(
    OUTPUT ${MAN_COMPRESSED}
    COMMAND ${CMAKE_COMMAND} -E copy ${MAN_TXT_SOURCE} ${MAN_TXT}
    COMMAND ${A2X} -f manpage ${MAN_TXT}
    COMMAND ${CMAKE_COMMAND} -E remove ${MAN_TXT}
    COMMAND ${BZIP2} -f ${MAN}
    DEPENDS ${MAN_TXT_SOURCE}
    COMMENT "Preparing man page ${MAN_COMPRESSED}"
    VERBATIM
)
add_custom_target(man ALL DEPENDS ${MAN_COMPRESSED})
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${MAN_COMPRESSED}
    DESTINATION ${CMAKE_INSTALL_MANDIR}
)
set(CPACK_GENERATOR "DEB;RPM;TBZ2")
set(CPACK_PACKAGE_CONTACT "Alexander Abrosimov")
include(CPack)
