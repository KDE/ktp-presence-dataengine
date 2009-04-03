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

#include <KDebug>

#include <TelepathyQt4/Client/AccountManager>
#include <TelepathyQt4/Client/Feature>
#include <TelepathyQt4/Client/PendingAccount>
#include <TelepathyQt4/Client/PendingReady>
#include <TelepathyQt4/Constants>


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
    if(isOperationError(op))
    {
        return;
    }

    // Get all the valid accounts from the AccountManager.
    m_accounts = m_accountManager->validAccounts();

    // Specify the features we want the accounts to become ready with
    QSet<Telepathy::Client::Feature> features;
    features << Telepathy::Client::Account::FeatureCore;
    // features << Telepathy::Client::Account::FeatureAvatar;  // FIXME: Uncomment me once t-a-k supports avatars
    features << Telepathy::Client::Account::FeatureProtocolInfo;

    // Iterate over all the accounts and have them become ready.
    Q_FOREACH(Telepathy::Client::AccountPtr account, m_accounts)
    {
        connect(account.data()->becomeReady(features),
                SIGNAL(finished(Telepathy::Client::PendingOperation*)),
                this, SLOT(onAccountReady(Telepathy::Client::PendingOperation*)));
    }
}

void PresenceEngine::onAccountReady(Telepathy::Client::PendingOperation *op)
{
    kDebug();   // Output the method we are in.

    // Check if the operation succeeded or not.
    if(isOperationError(op))
    {
        return;
    }

    Telepathy::Client::Account *account = qobject_cast<Telepathy::Client::Account*>(op->parent());

    // We should check the account is valid, and if not, remove it from the
    // list of accounts and the return.
    if(!account->isValidAccount())
    {
        Q_FOREACH(Telepathy::Client::AccountPtr accountPtr, m_accounts)
        {
            if(accountPtr->uniqueIdentifier() == account->uniqueIdentifier())
            {
                accountPtr->disconnect();
                removeSource(accountPtr->uniqueIdentifier());
                m_accounts.removeAll(accountPtr);
                return;
            }
        }
        return;
    }

    connect(account, SIGNAL(currentPresenceChanged(const Telepathy::SimplePresence &)),
            this, SLOT(onAccountCurrentPresenceChanged(const Telepathy::SimplePresence &)));
    // FIXME: Should we connect to signals for any other type of information than just current presence?

    // Now we should set up a data source for this account.
    QString source;
    source = account->uniqueIdentifier();
    Telepathy::SimplePresence currentPresence = account->currentPresence();
    setData(source, "current_presence_type", presenceTypeToString(currentPresence.type));
    setData(source, "current_presence_status", currentPresence.status);
    setData(source, "current_presence_status_message", currentPresence.statusMessage);
    // FIXME: Make things other than just current presence available for the source.
}

void PresenceEngine::onAccountCreated(const QString &path)
{
    kDebug();   // Output the method we are in.

    // Get an AccountPtr from the string we receive
    Telepathy::Client::AccountPtr account = m_accountManager->accountForPath(path);

    // Check if this account is already in the list.
    Q_FOREACH(Telepathy::Client::AccountPtr accountPtr, m_accounts)
    {
        if(accountPtr->uniqueIdentifier() == account->uniqueIdentifier())
        {
            // We already have that account.
            return;
        }
    }

    // Add the account to the list.
    m_accounts.append(account);

    // Specify the features we want the accounts to become ready with
    QSet<Telepathy::Client::Feature> features;
    features << Telepathy::Client::Account::FeatureCore;
    // features << Telepathy::Client::Account::FeatureAvatar;  // FIXME: Uncomment me once t-a-k supports avatars
    features << Telepathy::Client::Account::FeatureProtocolInfo;

    // Account is not in the list already, so we should get it ready.
    connect(account.data()->becomeReady(features),
            SIGNAL(finished(Telepathy::Client::PendingOperation*)),
            this, SLOT(onAccountReady(Telepathy::Client::PendingOperation*)));
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

    // Remove the account from the list, disconnect it and remove the data source
    Q_FOREACH(Telepathy::Client::AccountPtr accountPtr, m_accounts)
    {
        if(accountPtr->uniqueIdentifier() == account->uniqueIdentifier())
        {
            accountPtr->disconnect();
            removeSource(accountPtr->uniqueIdentifier());
            m_accounts.removeAll(accountPtr);
            return;
        }
    }
}

bool PresenceEngine::isOperationError(Telepathy::Client::PendingOperation *operation)
{
    kDebug();   // Output the method we are in.

    // Checks if a pending operation was successful and outputs debug if not.
    if (operation->isError()) {
        kDebug() << operation->errorName() << ": " << operation->errorMessage();
        return true;
    }

    return false;
}

void PresenceEngine::onAccountCurrentPresenceChanged(const Telepathy::SimplePresence & presence)
{
    kDebug();   // Output the method we are in.

    // Get the account which triggered this call.
    Telepathy::Client::Account *account = qobject_cast<Telepathy::Client::Account*>(sender());
    Q_ASSERT(account);

    // Set the presence of this account
    QString source;
    source = account->uniqueIdentifier();
    Telepathy::SimplePresence currentPresence = account->currentPresence();
    setData(source, "current_presence_type", presenceTypeToString(currentPresence.type));
    setData(source, "current_presence_status", currentPresence.status);
    setData(source, "current_presence_status_message", currentPresence.statusMessage);
    // FIXME: Make things other than just current presence available for the source.
}

QString PresenceEngine::presenceTypeToString(uint type)
{
    // This method converts a presence type from a telepathy SimplePresence
    // struct to a string representation for data sources.
    QString ret;

    switch(type)
    {
    case Telepathy::ConnectionPresenceTypeUnset:
        ret = "unset";
        break;
    case Telepathy::ConnectionPresenceTypeOffline:
        ret = "offline";
        break;
    case Telepathy::ConnectionPresenceTypeAvailable:
        ret = "available";
        break;
    case Telepathy::ConnectionPresenceTypeAway:
        ret = "away";
        break;
    case Telepathy::ConnectionPresenceTypeExtendedAway:
        ret = "xa";
        break;
    case Telepathy::ConnectionPresenceTypeHidden:
        ret = "invisible";
        break;
    case Telepathy::ConnectionPresenceTypeBusy:
        ret = "busy";
        break;
    case Telepathy::ConnectionPresenceTypeError:
        ret = "error";
        break;
    default:
        ret = "unknown";
        break;
    }

    return ret;
}


#include "presence.moc"

