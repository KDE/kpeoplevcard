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

#include "kpeoplevcard.h"
#include <QDebug>
#include <QImage>
#include <QDir>
#include <QStandardPaths>
#include <KContacts/VCardConverter>
#include <KContacts/Picture>
#include <KPeopleBackend/BasePersonsDataSource>

#include <KPluginFactory>
#include <KPluginLoader>

using namespace KPeople;

Q_GLOBAL_STATIC_WITH_ARGS(QString, vcardsLocation, (QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + ("/kpeoplevcard")))

class VCardContact : public AbstractContact
{
public:
    VCardContact() {}
    VCardContact(const KContacts::Addressee& addr) : m_addressee(addr) {}
    void setAddressee(const KContacts::Addressee& addr) { m_addressee = addr; }

    QVariant customProperty(const QString & key) const override
    {
        QVariant ret;
        if (key == NameProperty) {
            const QString name = m_addressee.realName();
            if (!name.isEmpty()) {
                return name;
            }
            if (!m_addressee.preferredEmail().isEmpty()) {
                return m_addressee.preferredEmail();
            }
            if (!m_addressee.phoneNumbers().isEmpty()) {
                return m_addressee.phoneNumbers().at(0).number();
            }
            return QVariant();
        } else if (key == EmailProperty)
            return m_addressee.preferredEmail();
        else if (key == PictureProperty)
            return m_addressee.photo().data();
        else if (key == AllPhoneNumbersProperty) {
            QVariantList numbers;
            Q_FOREACH (const KContacts::PhoneNumber &phoneNumber, m_addressee.phoneNumbers()) {
                // convert from KContacts specific format to QString
                numbers << phoneNumber.toString();
            }
            return numbers;
        } else if (key == PhoneNumberProperty) {
            return m_addressee.phoneNumbers().isEmpty() ? QVariant() : m_addressee.phoneNumbers().at(0).number();
        }

        return ret;
    }

    static QString createUri(const QString& path) {
        return QStringLiteral("vcard:") + path;
    }
private:
    KContacts::Addressee m_addressee;
};

class VCardDataSource : public KPeople::BasePersonsDataSource
{
public:
    VCardDataSource(QObject *parent, const QVariantList &data);
    virtual ~VCardDataSource();
    virtual QString sourcePluginId() const;

    virtual KPeople::AllContactsMonitor* createAllContactsMonitor();
};

KPeopleVCard::KPeopleVCard()
    : KPeople::AllContactsMonitor()
    , m_fs(new KDirWatch(this))
{
    QDir().mkpath(*vcardsLocation);

    QDir dir(*vcardsLocation);
    const QStringList subdirs = dir.entryList(QDir::AllDirs | QDir::NoDotDot); // includes '.', ie. vcards from no subdir
    QStringList entries;

    foreach(const QString &subdirName, subdirs) {
        QDir subdir(dir.absoluteFilePath(subdirName));
        QFileInfoList subdirVcards = subdir.entryInfoList({"*.vcard", "*.vcf"});
        foreach(const QFileInfo &vcardFile, subdirVcards) {
            entries << vcardFile.absoluteFilePath();
        }
    }

    foreach(const QString& entry, entries) {
        processVCard(entry);
    }

    m_fs->addDir(dir.absolutePath(), KDirWatch::WatchDirOnly | KDirWatch::WatchSubDirs);
    connect(m_fs, &KDirWatch::dirty, this, [this](const QString& path){ if (QFileInfo(path).isFile()) processVCard(path); });
    connect(m_fs, &KDirWatch::created, this, &KPeopleVCard::processVCard);
    connect(m_fs, &KDirWatch::deleted, this, &KPeopleVCard::deleteVCard);
}

KPeopleVCard::~KPeopleVCard()
{}

QMap<QString, AbstractContact::Ptr> KPeopleVCard::contacts()
{
    return m_contactForUri;
}

void KPeopleVCard::processVCard(const QString &path)
{
    m_fs->addFile(path);

    QFile f(path);
    bool b = f.open(QIODevice::ReadOnly);
    if (!b) {
        qWarning() << "error: couldn't open:" << path;
        return;
    }

    KContacts::VCardConverter conv;
    const KContacts::Addressee addressee = conv.parseVCard(f.readAll());

    QString uri = VCardContact::createUri(path);
    auto it = m_contactForUri.find(uri);
    if (it != m_contactForUri.end()) {
        static_cast<VCardContact*>(it->data())->setAddressee(addressee);
        Q_EMIT contactChanged(uri, *it);
    } else {
        KPeople::AbstractContact::Ptr contact(new VCardContact(addressee));
        m_contactForUri.insert(uri, contact);
        Q_EMIT contactAdded(uri, *it);
    }
}

void KPeopleVCard::deleteVCard(const QString &path)
{
    QString uri = VCardContact::createUri(path);
    int r = m_contactForUri.remove(uri);
    Q_ASSERT(r);
    Q_EMIT contactRemoved(uri);
}

QString KPeopleVCard::contactsVCardPath()
{
    return *vcardsLocation;
}

VCardDataSource::VCardDataSource(QObject *parent, const QVariantList &args)
: BasePersonsDataSource(parent)
{
    Q_UNUSED(args);
}

VCardDataSource::~VCardDataSource()
{
}

QString VCardDataSource::sourcePluginId() const
{
    return QStringLiteral("vcard");
}

AllContactsMonitor* VCardDataSource::createAllContactsMonitor()
{
    return new KPeopleVCard();
}

K_PLUGIN_FACTORY_WITH_JSON( VCardDataSourceFactory, "kpeoplevcard.json", registerPlugin<VCardDataSource>(); )
K_EXPORT_PLUGIN( VCardDataSourceFactory("kpeoplevcard") )

#include "kpeoplevcard.moc"
