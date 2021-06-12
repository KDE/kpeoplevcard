# SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

kcoreaddons_add_plugin(KPeopleVCard SOURCES kpeoplevcard.cpp JSON "kpeoplevcard.json" INSTALL_NAMESPACE "kpeople/datasource")
target_link_libraries(KPeopleVCard KF5::PeopleBackend KF5::CoreAddons KF5::Contacts KF5::I18n)
