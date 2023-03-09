# SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

kcoreaddons_add_plugin(KPeopleVCard SOURCES kpeoplevcard.cpp INSTALL_NAMESPACE "kpeople/datasource")
target_link_libraries(KPeopleVCard KF${QT_MAJOR_VERSION}::PeopleBackend KF${QT_MAJOR_VERSION}::CoreAddons KF${QT_MAJOR_VERSION}::Contacts KF${QT_MAJOR_VERSION}::I18n)
