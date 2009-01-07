set(presence_engine_SRCS
    presence.cpp
)

kde4_add_plugin(plasma_engine_presence ${presence_engine_SRCS})
target_link_libraries(plasma_engine_presence ${KDE4_KDECORE_LIBS} decibel ${KDE4_PLASMA_LIBS} QtTapioca)

install(TARGETS plasma_engine_presence DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-presence.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
