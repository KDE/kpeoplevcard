/*
    Copyright (C) 2015 Aleix Pol Gonzalez <aleixpol@kde.org>

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

#include <QDir>
#include <QObject>
#include <QTest>
#include <QStandardPaths>
#include <QSignalSpy>
#include <KContacts/VCardConverter>
#include <../src/kpeoplevcard.h>

class KPeopleVCardTest : public QObject
{
Q_OBJECT
public:
    KPeopleVCardTest()
        : m_source(new VCardDataSource(this, {}))
    {}

    QByteArray createContact(const KContacts::Addressee& addressee)
    {
        KContacts::VCardConverter conv;
        return conv.exportVCard(addressee, KContacts::VCardConverter::v3_0);
    }

    void writeContact(const KContacts::Addressee& addressee, const QString& path)
    {
        QFile f(path);
        bool b = f.open(QIODevice::WriteOnly);
        Q_ASSERT(b);

        f.write(createContact(addressee));
    }

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);
        m_vcardsDir = QDir(KPeopleVCard::contactsVCardPath());

        QDir().temp().mkpath(KPeopleVCard::contactsVCardPath());

        QVERIFY(m_vcardsDir.exists());

        foreach(const QFileInfo & entry, m_vcardsDir.entryInfoList(QDir::Files)) {
            QFile::remove(entry.absoluteFilePath());
        }

        QDir(KPeopleVCard::contactsVCardWritePath()).removeRecursively();
        QDir().temp().mkpath(KPeopleVCard::contactsVCardPath() + "/subdir");
        QCOMPARE(m_vcardsDir.count(), uint(3)); //. and .. and subdir/

        m_backend = dynamic_cast<KPeopleVCard*>(m_source->createAllContactsMonitor());
    }

    void emptyTest()
    {
        QVERIFY(m_backend->contacts().isEmpty());
    }

    void crudTest()
    {
//      ENSURE EMPTY
        QVERIFY(m_backend->contacts().isEmpty());

        const QString name = QStringLiteral("aaa"), name2 = QStringLiteral("bbb");
        KContacts::Addressee addr;
        addr.setName(name);
        const QString path = m_vcardsDir.absoluteFilePath("X");
        const QString pathInSubdir = m_vcardsDir.absoluteFilePath("subdir/a");
        // CREATE
        {
            QSignalSpy spy(m_backend, &KPeople::AllContactsMonitor::contactAdded);
            writeContact(addr, path);
            QVERIFY(spy.wait());

            // write the same contact into a subdir
            writeContact(addr, pathInSubdir);
            QVERIFY(spy.wait());
        }

        // READ
        {
            KPeople::AbstractContact::Ptr ptr = m_backend->contacts().first();
            QCOMPARE(ptr->customProperty(KPeople::AbstractContact::NameProperty).toString(), name);

            // the contact in subdir
            ptr = m_backend->contacts().last();
            QCOMPARE(ptr->customProperty(KPeople::AbstractContact::NameProperty).toString(), name);
        }

        // UPDATE
        {
            addr.setName(name2);
            QSignalSpy spy(m_backend, &KPeople::AllContactsMonitor::contactChanged);
            writeContact(addr, path);
            QVERIFY(spy.wait());

            writeContact(addr, pathInSubdir);
            QVERIFY(spy.wait());
        }

        // READ
        {
            KPeople::AbstractContact::Ptr ptr = m_backend->contacts().first();
            QCOMPARE(ptr->customProperty(KPeople::AbstractContact::NameProperty).toString(), name2);

            ptr = m_backend->contacts().last();
            QCOMPARE(ptr->customProperty(KPeople::AbstractContact::NameProperty).toString(), name2);
        }

        // REMOVE
        {
            QSignalSpy spy(m_backend, &KPeople::AllContactsMonitor::contactRemoved);
            QFile::remove(path);
            QVERIFY(spy.wait());

            QFile::remove(pathInSubdir);
            QVERIFY(spy.wait());
        }

//      ENSURE EMPTY
        QVERIFY(m_backend->contacts().isEmpty());
    }

    void editableInterface() {
        KContacts::Addressee addr;
        addr.setName(QStringLiteral("Potato Person"));

        {
            QSignalSpy spy(m_backend, &KPeople::AllContactsMonitor::contactAdded);
            QVERIFY(m_source->addContact({ {"vcard", createContact(addr) } }));
            QVERIFY(spy.wait());
        }
    }

private:
    KPeopleVCard* m_backend;
    VCardDataSource* m_source;
    QDir m_vcardsDir;
};

QTEST_GUILESS_MAIN(KPeopleVCardTest)

#include "kpeoplevcardtest.moc"

