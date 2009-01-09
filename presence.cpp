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
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "presence.h"

#include <TelepathyQt4/Client/Account>
#include <TelepathyQt4/Client/AccountManager>

#include <KDebug>
#include <KLocale>
#include <KUrl>

#include <QtCore/QDateTime>
#include <QtCore/QTimer>

class PresenceEngine::PresenceEnginePrivate
{
public:
	Telepathy::Client::AccountManager * m_accountManager;
};

PresenceEngine::PresenceEngine(QObject * parent, const QVariantList & args)
  : Plasma::DataEngine(parent, args),
  d(new PresenceEnginePrivate())
{
    // Register custom types:
    Telepathy::registerTypes();
}

PresenceEngine::~PresenceEngine()
{
	delete d->m_accountManager;
	delete d;
}

void
PresenceEngine::init()
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
    d->m_accountManager = 
    	new Telepathy::Client::AccountManager(QDBusConnection::sessionBus());
    
    /*
     * get a list of all the accounts that
     * are all ready there
     */
    QList<Telepathy::Client::Account *> accounts = d->m_accountManager->allAccounts();

    /*
     * connect signals from the account manager
     * to slots within this data engine.
     *
     * we intentionally do this before processing
     * the accounts that are already created so
     * that if another is created while we are
     * processing them, we don't miss out on it.
     */
    connect(d->m_accountManager, SIGNAL(accountCreated(const QDBusObjectPath &)),
            this, SLOT(accountCreated(const QDBusObjectPath &)));
    connect(d->m_accountManager, SIGNAL(accountValidityChanged(const QDBusObjectPath &, bool)),
            this, SLOT(accountValidityChanged(const QDBusObjectPath &, bool)));
    connect(d->m_accountManager, SIGNAL(accountRemoved(const QDBusObjectPath &)),
            this, SLOT(accountRemoved(const QDBusObjectPath &)));

    /*
     * create a datasource for each
     * of the accounts we got in the list.
     */
    foreach(Telepathy::Client::Account *account, accounts)
    {
        //accountCreated(account);
    }
}

bool
PresenceEngine::sourceRequestEvent(const QString & name)
{
    /*
     * if the visualisation requests a
     * source that is not already there
     * then it doesn't exist, so return
     * false
     */
    Q_UNUSED(name);
    return false;
}

void
PresenceEngine::accountCreated(const QDBusObjectPath &path)
{
    kDebug() << "accountCreated() called";
    // Load the data for the new account. To avoid duplicating code, we treat
    // this just as if an account was updated, and call the method to handle
    // that.
    accountValidityChanged(path, true);
}

void
PresenceEngine::accountValidityChanged(const QDBusObjectPath &path, bool valid)
{
    kDebug() << "accountValidityChanged() called";
    /*
     * slot called when an account has
     * been updated.
     */
/*    QString source;
    source.setNum(handle);
    QVariantMap accountData = m_accountManager->queryAccount(handle);
    QMap<QString, QVariant>::const_iterator end( accountData.constEnd() );
    for(QMap<QString, QVariant>::const_iterator itr(accountData.constBegin()); itr != end; ++itr)
    {
        if(itr.key() == Decibel::name_current_presence)
        {
            QtTapioca::PresenceState ps = qdbus_cast<QtTapioca::PresenceState>(itr.value().value<QDBusArgument>());
            QVariant psv;
            psv.setValue(ps);
            setData(source, "current_presence", psv);
            continue;
        }
        else if(itr.key() == Decibel::name_presence_parameters)
        {
            setData(source, "status_message", itr.value().toMap().value("status_message").toString());
        }
    }*/
}

void
PresenceEngine::accountRemoved(const QDBusObjectPath &path)
{
    kDebug() << "uint handle() called";
    /*
     * slot called when an account has been deleted
     *
     * remove that source.
     */
/*
    QString source;
    source.setNum(handle);
    removeSource(source);
*/
}


#include "presence.moc"

