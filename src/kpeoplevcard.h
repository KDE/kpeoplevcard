/*
    Copyright (C) 2015 Aleix Pol Gonzalez <aleixpol@kde.org>
    Copyright (C) 2015 Martin Klapetek <mklapetek@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef KPEOPLEVCARD_H
#define KPEOPLEVCARD_H

#include <KDirWatch>
#include <KPeopleBackend/AllContactsMonitor>
#include <KPeopleBackend/AbstractContact>

class Q_DECL_EXPORT KPeopleVCard : public KPeople::AllContactsMonitor
{
    Q_OBJECT

public:
    KPeopleVCard();
    virtual ~KPeopleVCard();

    QMap<QString, KPeople::AbstractContact::Ptr> contacts() override;
    static QString contactsVCardPath();

private:
    void processVCard(const QString &path);
    void deleteVCard(const QString &path);

    QMap<QString, KPeople::AbstractContact::Ptr> m_contactForUri;
    KDirWatch* m_fs;
};

#endif