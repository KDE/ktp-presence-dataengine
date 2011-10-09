/*
 * Copyright (C) 2008 George Goldberg <grundleborg@googlemail.com>
 * Copyright (C) 2009 Collabora Ltd <http://www.collabora.co.uk>
 * Copyright (C) 2009 Andre Moreira Magalhaes <andrunko@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "global/globalpresencesource.h"
#include "presence.h"
#include "presencesource.h"

#include <KDebug>

#include <Plasma/Service>

#include <TelepathyQt4/Account>
#include <TelepathyQt4/AccountSet>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>

PresenceEngine::PresenceEngine(QObject *parent, const QVariantList &args)
    : Plasma::DataEngine(parent, args)
    , m_globalPresenceSource(new GlobalPresenceSource())
{
    // Register custom TelepathyQt4 types
    Tp::registerTypes();
    m_dbusExporter = new DBusExporter(this);
    QDBusConnection::sessionBus().registerObject("/PresenceEngineActive", this, QDBusConnection::ExportAdaptors);
    QDBusConnection::sessionBus().registerService("org.kde.Telepathy.PresenceEngineActive");
}

PresenceEngine::~PresenceEngine()
{
    QDBusConnection::sessionBus().unregisterObject("/PresenceEngineActive");
    QDBusConnection::sessionBus().unregisterService("org.kde.Telepathy.PresenceEngineActive");
}

Plasma::Service *PresenceEngine::serviceForSource(const QString &name)
{
    // Get the data source and then from that, we can get the service
    PresenceSource *source = qobject_cast<PresenceSource*>(containerForSource(name));
    GlobalPresenceSource *globalSource = qobject_cast<GlobalPresenceSource*>(containerForSource(name));

    if (source) {
        Plasma::Service *service = source->createService();
        service->setParent(this);
        return service;
    } else if (globalSource) {
        Plasma::Service *service = globalSource->createService();
        service->setParent(this);
        return service;
    } else {
        kWarning() << "PresenceEngine::serviceForSource: service does not "
        "exist for the source with name:" << name;
        return Plasma::DataEngine::serviceForSource(name);
    }
}

void PresenceEngine::init()
{
    // Check we are connected to the session bus.
    if (!QDBusConnection::sessionBus().isConnected()) {
        kWarning() << "PresenceEngine::init: cannot connect to session bus.";
        return;
    }

    // Construct the AccountManager
    m_accountManager = Tp::AccountManager::create(QDBusConnection::sessionBus());
    // Get te AccountManager ready
    connect(m_accountManager->becomeReady(),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onAccountManagerReady(Tp::PendingOperation*)));
}

bool PresenceEngine::sourceRequestEvent(const QString &name)
{
    Q_UNUSED(name);

    // This method is only called if a source is requested that does *not*
    // already exist. Since it makes no sense to allow creation of new sources,
    // we just automatically return false here.
    return false;
}

void PresenceEngine::onAccountManagerReady(Tp::PendingOperation *op)
{
    // Check if the operation succeeded or not
    if (op->isError()) {
        kWarning() << "PresenceEngine::onAccountManagerReady: readying "
            "AccountManager failed:" <<
            op->errorName() << ":" << op->errorMessage();
        return;
    }

    // Get all the valid accounts from the AccountManager
    Tp::AccountSetPtr accounts = m_accountManager->validAccounts();
    // Iterate over all the accounts and create a source for each of them
    foreach (const Tp::AccountPtr &account, accounts->accounts()) {
        addAccount(account);
    }

    // Connect the signals from the AccountManager indicating that Accounts
    // have changed to slots to handle these changes
    connect(m_accountManager.data(),
            SIGNAL(newAccount(Tp::AccountPtr)),
            this,
            SLOT(addAccount(Tp::AccountPtr)));

    // set account manager for the global presence object
    m_globalPresenceSource->setGlobalPresenceAccountManager(m_accountManager);
    addSource(m_globalPresenceSource);
}

void PresenceEngine::onAccountRemoved(const QString &path)
{
    QString realPath;
    if (path.isEmpty()) {
        // Get the AccountPtr from the object path and remove the corresponding
        // source
        Tp::Account *account = qobject_cast< Tp::Account* >(sender());
        realPath = account->objectPath();
    } else {
        realPath = path;
    }
    if (sources().contains(realPath)) {
        removeSource(realPath);
    } else {
        kWarning() << "PresenceEngine::onAccountRemoved: source "
            "does not exist for account:" << realPath;
    }
}

void PresenceEngine::onAccountValidityChanged(bool valid)
{
    Tp::Account *account = qobject_cast< Tp::Account* >(sender());
    if (valid) {
        Tp::AccountPtr accountPtr = m_accountManager->accountForPath(account->objectPath());
        addAccount(accountPtr);
    }
    else {
        onAccountRemoved(account->objectPath());
    }
}

void PresenceEngine::addAccount(const Tp::AccountPtr &account)
{
    if (!sources().contains(account->objectPath())) {
        addSource(new PresenceSource(account, this));
        connect(account.data(),
                SIGNAL(removed()),
                this,
                SLOT(onAccountRemoved()));
        connect(account.data(),
                SIGNAL(validityChanged(bool)),
                this,
                SLOT(onAccountValidityChanged(bool)));
    } else {
        kWarning() << "PresenceEngine::addAccount: source "
            "already exists for account:" << account->objectPath();
    }
}

DBusExporter::DBusExporter(QObject* parent) : QDBusAbstractAdaptor(parent)
{

}

DBusExporter::~DBusExporter()
{

}

#include "presence.moc"
