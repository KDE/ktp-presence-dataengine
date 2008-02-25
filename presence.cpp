/*
 *   Copyright (C) 2008 George Goldberg <grundleborg@googlemail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "presence.h"

#include <QDateTime>
#include <QTimer>
#include <QtDBus/QDBusConnection>

#include <KDebug>
#include <KLocale>
#include <KUrl>

#include <Decibel/AccountManager>
#include <Decibel/DBusNames>
#include <Decibel/Types>

PresenceEngine::PresenceEngine(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args)
{
    // Register custom types:
    Decibel::registerTypes();
}

PresenceEngine::~PresenceEngine()
{
}

void PresenceEngine::init()
{
    kDebug() << "init() started";
    /*
     * check that we are connected to the session
     * bus OK.
     */
    if (!QDBusConnection::sessionBus().isConnected())
    {
        kDebug() << "PresenceEngine::init(): cannot connect to session bus.";
    }

   /*
    * set up the dbus accountmanager object
    * which will provide all the data to this
    * data engine.
    */
    m_accountManager = new de::basyskom::Decibel::AccountManager
                                (Decibel::daemon_service,
                                 Decibel::daemon_accountmanager_path,
                                 QDBusConnection::sessionBus());
    /*
     * get a list of all the accounts that
     * are all ready there
     */
    QList<uint> accounts = m_accountManager->listAccounts();

    /*
     * connect signals from the account manager
     * to slots within this data engine.
     *
     * we intentionally do this before processing
     * the accounts that are already created so
     * that if another is created while we are
     * processing them, we don't miss out on it.
     */
    connect(m_accountManager, SIGNAL(accountCreated(const uint)), this, SLOT(accountCreated(const uint)));
    connect(m_accountManager, SIGNAL(accountUpdated(const uint)), this, SLOT(accountUpdated(const uint)));
    connect(m_accountManager, SIGNAL(accountDeleted(const uint)), this, SLOT(accountDeleted(const uint)));

    /*
     * create a datasource for each
     * of the accounts we got in the list.
     */
    uint handle;
    foreach(handle, accounts)
    {
        accountCreated(handle);
    }
}

bool PresenceEngine::sourceRequested(const QString &name)
{
    /*
     * if the visualisation requests a
     * source that is not already there
     * then it doesn't exist, so return
     * false
     */

    return false;
}

void PresenceEngine::accountCreated(const uint handle)
{
    kDebug() << "accountCreated() called";
    /*
     * slot called when a new account is
     * created.
     *
     * we should get the data for that
     * account from decibel and then
     * setData with it.
     */
    QString source;
    source.setNum(handle);
    QVariantMap accountData = m_accountManager->queryAccount(handle);
    QMap<QString, QVariant>::const_iterator end( accountData.constEnd() );
    for ( QMap<QString, QVariant>::const_iterator itr( accountData.constBegin() ); itr != end; ++itr )
    {
        setData(source, itr.key(), itr.value());
    }
}

void PresenceEngine::accountUpdated(const uint handle)
{
    kDebug() << "accountUpdated() called";
    /*
     * slot called when an account has
     * been updated.
     */
    QString source;
    source.setNum(handle);
    QVariantMap accountData = m_accountManager->queryAccount(handle);
    QMap<QString, QVariant>::const_iterator end( accountData.constEnd() );
    for ( QMap<QString, QVariant>::const_iterator itr( accountData.constBegin() ); itr != end; ++itr )
    {
        setData(source, itr.key(), itr.value());
    }
}

void PresenceEngine::accountDeleted(const uint handle)
{
    kDebug() << "accountDeleted() called";
    /*
     * slot called when an account has been deleted
     *
     * remove that source.
     */
    QString source;
    source.setNum(handle);
    removeSource(source);
}


#include "presence.moc"
