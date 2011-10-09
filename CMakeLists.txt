project(plasma-dataengine-presence)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

find_package(KDE4 REQUIRED)
find_package(TelepathyQt4 REQUIRED)

include(KDE4Defaults)

add_definitions(${KDE4_DEFINITIONS})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${KDE4_INCLUDES}
    ${TELEPATHY_QT4_INCLUDE_DIR}
)

set(presence_engine_SRCS
    global/global-presence.cpp
    global/globalpresencesource.cpp
    presence.cpp
    presencesource.cpp
    presenceservice.cpp
    setavatarjob.cpp
    setnicknamejob.cpp
    setrequestedpresencejob.cpp
)

kde4_add_plugin(plasma_engine_presence ${presence_engine_SRCS})
target_link_libraries(plasma_engine_presence
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${TELEPATHY_QT4_LIBRARIES}
)

install(TARGETS plasma_engine_presence DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-presence.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES presence.operations DESTINATION ${DATA_INSTALL_DIR}/plasma/services)
