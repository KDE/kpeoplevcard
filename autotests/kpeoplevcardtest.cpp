/*
    SPDX-FileCopyrightText: 2015 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <../src/kpeoplevcard.h>
#include <KContacts/VCardConverter>
#include <QDir>
#include <QObject>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>

class KPeopleVCardTest : public QObject
{
    Q_OBJECT
public:
    KPeopleVCardTest()
        : m_source(new VCardDataSource(this, {}))
    {
    }

    QByteArray createContact(const KContacts::Addressee &addressee)
    {
        KContacts::VCardConverter conv;
        return conv.exportVCard(addressee, KContacts::VCardConverter::v3_0);
    }

    void writeContact(const KContacts::Addressee &addressee, const QString &path)
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

        foreach (const QFileInfo &entry, m_vcardsDir.entryInfoList(QDir::Files)) {
            QFile::remove(entry.absoluteFilePath());
        }

        QDir(KPeopleVCard::contactsVCardWritePath()).removeRecursively();
        QDir().temp().mkpath(KPeopleVCard::contactsVCardPath() + "/subdir");
        QCOMPARE(m_vcardsDir.count(), uint(3)); //. and .. and subdir/

        m_backend = dynamic_cast<KPeopleVCard *>(m_source->createAllContactsMonitor());
    }

    void emptyTest()
    {
        QVERIFY(m_backend->contacts().isEmpty());
    }

    void crudTest()
    {
        // ENSURE EMPTY
        QVERIFY(m_backend->contacts().isEmpty());

        const QString name = QStringLiteral("aaa"), name2 = QStringLiteral("bbb");
        KContacts::Addressee addr;
        addr.setName(name);
        const QString path = m_vcardsDir.absoluteFilePath("X.vcf");
        const QString pathInSubdir = m_vcardsDir.absoluteFilePath("subdir/a.vcf");
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

        // ENSURE EMPTY
        QVERIFY(m_backend->contacts().isEmpty());
    }

    void editableInterface()
    {
        KContacts::Addressee addr;
        addr.setName(QStringLiteral("Potato Person"));
        QString uri;
        KPeople::AbstractContact::Ptr ptr;

        // CREATE
        {
            QSignalSpy spy(m_backend, &KPeople::AllContactsMonitor::contactAdded);
            QVERIFY(m_source->addContact({{"vcard", createContact(addr)}}));
            QVERIFY(spy.wait());

            uri = spy.constFirst().constFirst().toString();
            ptr = spy.constFirst().constLast().value<KPeople::AbstractContact::Ptr>();

            QVERIFY(m_backend->contacts().contains(uri));
            QCOMPARE(QDir(KPeopleVCard::contactsVCardWritePath()).count(), 3); //. .. and the potato person
        }

        // READ
        {
            QCOMPARE(ptr->customProperty(KPeople::AbstractContact::NameProperty), addr.name());
        }

        // UPDATE
        {
            KPeople::AbstractEditableContact *editable = dynamic_cast<KPeople::AbstractEditableContact *>(ptr.data());
            QVERIFY(editable);
            QCOMPARE(QDir(KPeopleVCard::contactsVCardWritePath()).count(), 3); //. .. and the potato person
            QVERIFY(m_backend->contacts().contains(uri));

            addr.setName("Tomato Person");
            QSignalSpy spy(m_backend, &KPeople::AllContactsMonitor::contactChanged);
            QVERIFY(editable->setCustomProperty("vcard", createContact(addr)));
            QVERIFY(spy.wait());
            QCOMPARE(spy.constFirst().constFirst(), uri);
        }

        {
            QSignalSpy spy(m_backend, &KPeople::AllContactsMonitor::contactRemoved);
            QVERIFY(m_source->deleteContact(uri));
            QVERIFY(spy.wait());

            QCOMPARE(spy.constFirst().constFirst().toString(), uri);
        }
        QCOMPARE(QDir(KPeopleVCard::contactsVCardWritePath()).count(), 2); //. .. and the potato person
        QVERIFY(!m_backend->contacts().contains(uri));
    }

private:
    KPeopleVCard *m_backend;
    VCardDataSource *m_source;
    QDir m_vcardsDir;
};

QTEST_GUILESS_MAIN(KPeopleVCardTest)

#include "kpeoplevcardtest.moc"
