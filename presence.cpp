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

#include <TelepathyQt4/Client/AccountManager>
#include <TelepathyQt4/Client/PendingReady>

#include <KDebug>
#include <KLocale>
#include <KUrl>

#include <QtCore/QDateTime>
#include <QtCore/QTimer>

/**
 * \class PresenceEngine
 * \ingroup presence
 * \headerfile <presence.h>
 *
 * Object representing a Presence data source.
 */

/**
 * Construct a new PresenceEngine object.
 *
 * \param parent Object parent.
 * \param args QVariantList arguments.
 */
PresenceEngine::PresenceEngine(QObject * parent, const QVariantList & args)
  : Plasma::DataEngine(parent, args)
{
    kDebug();
    // Register custom types:
    Telepathy::registerTypes();
    setIcon(QString());
}

/**
 * Class destructor
 */
PresenceEngine::~PresenceEngine()
{
    kDebug();
}

/**
 * Initialize Presence.
 */
void PresenceEngine::init()
{
    kDebug();
    /*
     * check that we are connected to the session
     * bus OK.
     */
    m_accountManager = 0;

    if (!QDBusConnection::sessionBus().isConnected())
    {
        kDebug() << "PresenceEngine::init(): cannot connect to session bus.";
        return;
    }

   /*
    * set up the dbus accountmanager object
    * which will provide all the data to this
    * data engine.
    */
    m_accountManager =
        new Telepathy::Client::AccountManager(QDBusConnection::sessionBus());

    /*
     * connect signal from the account manager
     * to waiting when it's ready
     */
    connect(m_accountManager->becomeReady(),
    SIGNAL(finished(Telepathy::Client::PendingOperation*)),
           this,SLOT(onAccountReady(Telepathy::Client::PendingOperation*)));

    /*
     * connect signals from the account manager
     * to slots within this data engine.
     *
     * we intentionally do this before processing
     * the accounts that are already created so
     * that if another is created while we are
     * processing them, we don't miss out on it.
     */
    connect(m_accountManager, SIGNAL(accountCreated(const QString &)),
            this, SLOT(accountCreated(const QString &)));
    connect(m_accountManager, SIGNAL(accountValidityChanged(const QString &, bool)),
            this, SLOT(accountValidityChanged(const QString &, bool)));
    connect(m_accountManager, SIGNAL(accountRemoved(const QString &)),
            this, SLOT(accountRemoved(const QString &)));
}

/**
 * Return whether source exist.
 *
 * \return \c true if source exists.
 */
bool PresenceEngine::sourceRequestEvent(const QString & name)
{
    kDebug();
    /*
     * if the visualisation requests a
     * source that is not already there
     * then it doesn't exist, so return
     * false
     */
    Q_UNUSED(name);
    return false;
}

void PresenceEngine::onAccountReady(Telepathy::Client::PendingOperation *operation)
{
    kDebug();
    if(isOperationError(operation))
        return;

    QStringList pathList = m_accountManager->allAccountPaths();
    kDebug() << "All Account Paths: " << pathList.size();

    /*
     * get a list of all the accounts that
     * are all ready there
     */
    foreach (const QString &path, m_accountManager->allAccountPaths())  {
        createAccountDataSource(path);
    }
}

/**
 *  Slot for new account.
 *
 * \param path QDBusObjectPath to created account.
 */
void PresenceEngine::accountCreated(const QString &path)
{
    kDebug();
    // Load the data for the new account. To avoid duplicating code, we treat
    // this just as if an account was updated, and call the method to handle
    // that.
    createAccountDataSource(path);
}

/**
 * Slot for account data changed.
 *
 * \param QDBusObjectPath Name of the account path.
 * \param valid true if the account is valid.
 */
void PresenceEngine::accountValidityChanged(const QString &path, bool valid)
{
    Q_UNUSED(valid);
    kDebug();
    /*
     * slot called when an account has
     * been updated.
     */
    createAccountDataSource(path);
}

/**
 * Slot for account removed.
 *
 * \param QDBusObjectPath Name of the account path.
 */
void PresenceEngine::accountRemoved(const QString &path)
{
    kDebug();
    /*
     * slot called when an account has been deleted
     *
     * remove that source.
     */
    removeAccountDataSource(path);
}

void PresenceEngine::createAccountDataSource(const QString &path)
{
    kDebug() << path;
    Telepathy::Client::AccountPtr account = accountFromPath(path);
    QObject::connect(account.data(), SIGNAL(currentPresenceChanged(const Telepathy::SimplePresence &)),
        this, SLOT(currentPresenceChanged(const Telepathy::SimplePresence &)));
    QObject::connect(account->becomeReady(), SIGNAL(finished(Telepathy::Client::PendingOperation *)),
        this, SLOT(onExistingAccountReady(Telepathy::Client::PendingOperation *)));
}

void PresenceEngine::onExistingAccountReady(Telepathy::Client::PendingOperation *operation)
{
    kDebug();

    if(isOperationError(operation))
        return;

    Telepathy::Client::Account *account = qobject_cast<Telepathy::Client::Account *>(operation->parent());
    if(!account)
        return;

    QString source;
    source = account->uniqueIdentifier();
    Telepathy::SimplePresence sp = account->currentPresence();
    QVariant vsp;
    vsp.setValue(sp);
    setData(source, "current_presence", vsp);
}

void PresenceEngine::removeAccountDataSource(const QString &path)
{
    kDebug() << path;

    Telepathy::Client::AccountPtr account = accountFromPath(path);
    QString identifier = account->uniqueIdentifier();
    removeSource(identifier);
}

Telepathy::Client::AccountPtr PresenceEngine::accountFromPath(const QString &path)
{
    kDebug();
    return m_accountManager->accountForPath(path);
}

bool PresenceEngine::isOperationError(Telepathy::Client::PendingOperation *operation)
{
    if (operation->isError()) {
        kDebug() << operation->errorName() << ": " << operation->errorMessage();
        return true;
    }

    return false;
}

void PresenceEngine::currentPresenceChanged(const Telepathy::SimplePresence & presence)
{
    QVariant vsp;
    vsp.setValue(presence);
    foreach (QString source, sources()) {
       setData(source, "current_presence", vsp);
    }
    updateAllSources();
}

#include "presence.moc"

