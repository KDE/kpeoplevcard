kcoreaddons_add_plugin(KPeopleVCard SOURCES kpeoplevcard.cpp JSON "kpeoplevcard.json" INSTALL_NAMESPACE "kpeople/datasource")
target_link_libraries(KPeopleVCard KF5::PeopleBackend KF5::CoreAddons KF5::Contacts)
