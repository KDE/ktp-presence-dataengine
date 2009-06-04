/*
 * Copyright (C) 2008 George Goldberg <grundleborg@googlemail.com>
 * Copyright (C) 2009 Collabora Ltd <http://www.collabora.co.uk>
 * Copyright (C) 2009 Andre Moreira Magalhaes <andrunko@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License version 2 as
 * published by the Free Software Foundation
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

#include "presence.h"

#include "presencesource.h"

#include <KDebug>

#include <Plasma/Service>

#include <TelepathyQt4/Account>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>

PresenceEngine::PresenceEngine(QObject *parent, const QVariantList &args)
    : Plasma::DataEngine(parent, args)
{
    // Register custom TelepathyQt4 types
    Tp::registerTypes();
}

PresenceEngine::~PresenceEngine()
{
}

Plasma::Service *PresenceEngine::serviceForSource(const QString &name)
{
    // Get the data source and then from that, we can get the service
    PresenceSource *source =
        dynamic_cast<PresenceSource*>(containerForSource(name));
    if (!source) {
        kWarning() << "PresenceEngine::serviceForSource: service does not "
            "exist for the source with name:" << name;
        return Plasma::DataEngine::serviceForSource(name);
    }

    Plasma::Service *service = source->createService();
    service->setParent(this);
    return service;
}

void PresenceEngine::init()
{
    // Check we are connected to the session bus.
    if (!QDBusConnection::sessionBus().isConnected()) {
        kWarning() << "PresenceEngine::init: cannot connect to session bus.";
        return;
    }

    // Construct the AccountManager
    m_accountManager =
        Tp::AccountManager::create(QDBusConnection::sessionBus());
    // Get te AccountManager ready
    connect(m_accountManager->becomeReady(),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onAccountManagerReady(Tp::PendingOperation*)));

    // Connect the signals from the AccountManager indicating that Accounts
    // have changed to slots to handle these changes
    connect(m_accountManager.data(),
            SIGNAL(accountCreated(const QString &)),
            SLOT(onAccountCreated(const QString &)));
    connect(m_accountManager.data(),
            SIGNAL(accountValidityChanged(const QString &, bool)),
            SLOT(onAccountValidityChanged(const QString &, bool)));
    connect(m_accountManager.data(),
            SIGNAL(accountRemoved(const QString &)),
            SLOT(onAccountRemoved(const QString &)));
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
    QList<Tp::AccountPtr> accounts = m_accountManager->validAccounts();
    // Iterate over all the accounts and create a source for each of them
    foreach (const Tp::AccountPtr &account, accounts) {
        addAccount(account);
    }
}

void PresenceEngine::onAccountCreated(const QString &path)
{
    // Get the AccountPtr from the object path and create a source for it
    Tp::AccountPtr account = m_accountManager->accountForPath(path);
    addAccount(account);
}

void PresenceEngine::onAccountRemoved(const QString &path)
{
    // Get the AccountPtr from the object path and remove the corresponding
    // source
    Tp::AccountPtr account = m_accountManager->accountForPath(path);
    if (sources().contains(account->uniqueIdentifier())) {
        removeSource(account->uniqueIdentifier());
    } else {
        kWarning() << "PresenceEngine::onAccountRemoved: source "
            "does not exist for account:" << account->uniqueIdentifier();
    }
}

void PresenceEngine::onAccountValidityChanged(const QString &path, bool valid)
{
    if (valid) {
        onAccountCreated(path);
    }
    else {
        onAccountRemoved(path);
    }
}

void PresenceEngine::addAccount(const Tp::AccountPtr &account)
{
    if (!sources().contains(account->uniqueIdentifier())) {
        addSource(new PresenceSource(account, this));
    } else {
        kWarning() << "PresenceEngine::addAccount: source "
            "already exists for account:" << account->uniqueIdentifier();
    }
}

#include "presence.moc"

