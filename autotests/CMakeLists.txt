# SPDX-FileCopyrightText: 2015 Martin Klapetek <mklapetek@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

find_package(Qt${QT_MAJOR_VERSION}Test)

ecm_add_test(kpeoplevcardtest.cpp ../src/kpeoplevcard.cpp TEST_NAME kpeoplevcardtest LINK_LIBRARIES KF${QT_MAJOR_VERSION}::PeopleBackend KF${QT_MAJOR_VERSION}::CoreAddons Qt::Test KF${QT_MAJOR_VERSION}::Contacts KF${QT_MAJOR_VERSION}::I18n)
