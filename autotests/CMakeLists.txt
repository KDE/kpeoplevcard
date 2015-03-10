find_package(Qt5Test)

ecm_add_test(kpeoplevcardtest LINK_LIBRARIES KF5::People Qt5::Test KF5::Contacts KPeopleVCard)
