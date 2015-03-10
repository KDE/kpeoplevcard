add_library(KPeopleVCard SHARED kpeoplevcard.cpp main.cpp)
target_link_libraries(KPeopleVCard KF5::PeopleBackend KF5::CoreAddons KF5::Contacts)

install(TARGETS KPeopleVCard DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kpeoplevcard_source_plugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})
