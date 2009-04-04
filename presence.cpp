/*
 *   Copyright (C) 2008 George Goldberg <grundleborg@googlemail.com>
 *   Copyright (C) 2009 Collabora Ltd <http://www.collabora.co.uk>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "presence.h"

#include "presencesource.h"

#include <KDebug>

#include <Plasma/Service>

#include <TelepathyQt4/Client/Account>
#include <TelepathyQt4/Client/AccountManager>
#include <TelepathyQt4/Client/Feature>
#include <TelepathyQt4/Client/PendingOperation>
#include <TelepathyQt4/Client/PendingReady>


PresenceEngine::PresenceEngine(QObject * parent, const QVariantList & args)
  : Plasma::DataEngine(parent, args),
    m_accountManager(0)
{
    kDebug();   // Output the method we are in.

    // Register custom TelepathyQt4 types
    Telepathy::registerTypes();

    // Set the icon to be nothing for now.
    setIcon(QString());
}

PresenceEngine::~PresenceEngine()
{
    kDebug();   // Output the method we are in.

    // Delete the AccountManager.
    if(m_accountManager != 0)
    {
        delete m_accountManager;
    }
}

Plasma::Service * PresenceEngine::serviceForSource(const QString &name)
{
    kDebug();   // Output which method we are in.

    // Get the data source and then from that, we can get the service
    PresenceSource *source = dynamic_cast<PresenceSource*>(containerForSource(name));

    if(!source)
    {
        kWarning() << "Service does not exist for the source with name:" << name;
        return Plasma::DataEngine::serviceForSource(name);
    }

    Plasma::Service *service = source->createService();
    service->setParent(this);
    return service;
}


void PresenceEngine::init()
{
    kDebug();   // Output the method we are in.

    // Check we are connected to the session bus.
    if(!QDBusConnection::sessionBus().isConnected())
    {
        kWarning() << "PresenceEngine::init(): cannot connect to session bus.";
        return;
    }

    // Construct the Telepathy AccountManager
    m_accountManager = new Telepathy::Client::AccountManager(QDBusConnection::sessionBus());

    // Get te AccountManager ready with the desired features.
    QSet<Telepathy::Client::Feature> features;
    features << Telepathy::Client::AccountManager::FeatureCore;

    connect(m_accountManager->becomeReady(features),
            SIGNAL(finished(Telepathy::Client::PendingOperation*)),
            this, SLOT(onAccountManagerReady(Telepathy::Client::PendingOperation*)));

    // Connect the signals from the AccountManager indicating that Accounts
    //  have changed to slots to handle these changes.
    connect(m_accountManager, SIGNAL(accountCreated(const QString &)),
            this, SLOT(onAccountCreated(const QString &)));
    connect(m_accountManager, SIGNAL(accountValidityChanged(const QString &, bool)),
            this, SLOT(onAccountValidityChanged(const QString &, bool)));
    connect(m_accountManager, SIGNAL(accountRemoved(const QString &)),
            this, SLOT(onAccountRemoved(const QString &)));
}


bool PresenceEngine::sourceRequestEvent(const QString & name)
{
    kDebug();   // Output the method we are in.

    Q_UNUSED(name);

    // This method is only called if a source is requested that does *not*
    // already exist. Since it makes no sense to allow creation of new sources,
    // we just automatically return false here.
    return false;
}

void PresenceEngine::onAccountManagerReady(Telepathy::Client::PendingOperation *op)
{
    kDebug();   // Output the method we are in.

    // Check if the operation succeeded or not.
    if(op->isError())
    {
        kWarning() << "Readying Account Manager failed: " << op->errorName() << ":" << op->errorMessage();
        return;
    }

    // Get all the valid accounts from the AccountManager.
    QList<Telepathy::Client::AccountPtr> accounts = m_accountManager->validAccounts();

    // Iterate over all the accounts and create a source for each of them
    Q_FOREACH(Telepathy::Client::AccountPtr account, accounts)
    {
        // There should not already be a source for this account.
        Q_ASSERT(!sources().contains(account->uniqueIdentifier()));
        if(!sources().contains(account->uniqueIdentifier()))
        {
            addSource(new PresenceSource(account, this));
        }
        else
        {
            kWarning() << "Source already exists for account:" << account->uniqueIdentifier();
        }
    }
}

void PresenceEngine::onAccountCreated(const QString &path)
{
    kDebug();   // Output the method we are in.

    // Get an AccountPtr from the string we receive
    Telepathy::Client::AccountPtr account = m_accountManager->accountForPath(path);

    // Check if a source already exists for that account, and if not, add one.
    Q_ASSERT(!sources().contains(account->uniqueIdentifier()));
    if(!sources().contains(account->uniqueIdentifier()))
    {
        addSource(new PresenceSource(account, this));
    }
    else
    {
        kWarning() << "Source already exists for account:" << account->uniqueIdentifier();
    }
}

void PresenceEngine::onAccountValidityChanged(const QString &path, bool valid)
{
    kDebug();   // Output the method we are in.

    // Use the other two methods to avoid code duplication
    if(valid)
    {
        onAccountCreated(path);
    }
    else
    {
        onAccountRemoved(path);
    }
}

void PresenceEngine::onAccountRemoved(const QString &path)
{
    kDebug();   // Output the method we are in.

    // Get the AccountPtr from the path.
    Telepathy::Client::AccountPtr account = m_accountManager->accountForPath(path);

    // There should be a source for this account. If there is, remove it.
    Q_ASSERT(sources().contains(account->uniqueIdentifier()));
    if(sources().contains(account->uniqueIdentifier()))
    {
        removeSource(account->uniqueIdentifier());
    }
    else
    {
        kWarning() << "Source does not exists for account:" << account->uniqueIdentifier();
    }
}


#include "presence.moc"

