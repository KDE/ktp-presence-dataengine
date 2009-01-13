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
	PresenceEngine *parent;
public:
	PresenceEnginePrivate(PresenceEngine *p) : parent(p) {}
	
	Telepathy::Client::AccountManager * m_accountManager;
	
	void createAccountDataSource(const QString &path)
	{
		// \todo: FIXME
		kDebug() << "createAccountDataSource called";
		kDebug() << path;
	    Telepathy::Client::Account *account = accountFromObjectPath(path);

	    QString source;
	    // \todo: FIXME
	    // source = account.uniqueIdentifier();
	    source = path;
	    
	    Telepathy::SimplePresence sp = account->currentPresence();
	    QVariant vsp;
	    vsp.setValue(sp);
	    parent->setData(source, "current_presence", vsp);
	    
		// \todo: remove
/*
			QString source;
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
		    }
*/
	}
	
	void removeAccountDataSource(const QString &path)
	{
		kDebug() << "removeAccountDataSource called";
		kDebug() << path;
		
		// \todo: FIXME
/*
		QString identifier = account->uniqueIdentifier();
		parent->removeSource(identifier);
		emit parent->sourceRemoved(identifier);
*/
		QString identifier = path;
		parent->removeSource(identifier);
	}
	
	Telepathy::Client::Account *accountFromObjectPath(const QString &path)
	{
		return m_accountManager->accountForPath(path);
	}
};

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
  : Plasma::DataEngine(parent, args),
  d(new PresenceEnginePrivate(this))
{
    // Register custom types:
    Telepathy::registerTypes();
    setIcon(QString());
}

/**
 * Class destructor
 */
PresenceEngine::~PresenceEngine()
{
	// \todo: FIXME. Why there is a problem?
//	delete d->m_accountManager;
	delete d;
}

/**
 * Initialize Presence.
 */
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
    d->m_accountManager = 
    	new Telepathy::Client::AccountManager(QDBusConnection::sessionBus());
    
    /*
     * connect signal from the account manager
     * to waiting when it's ready
     */
    connect(d->m_accountManager->becomeReady(),
    		SIGNAL(finished(Telepathy::Client::PendingOperation*)),
    		this,
    		SLOT(onAccountReady(Telepathy::Client::PendingOperation*))
    		);
    
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
}

/**
 * Return whether source exist.
 * 
 * \return \c true if source exists.
 */
bool PresenceEngine::sourceRequestEvent(const QString & name)
{
	kDebug() << "sourceRequestEvent() called";
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
	kDebug() << "onAccountReady() called";
	if(operation->isError())
	{
		kDebug() << operation->errorName() << ": " << operation->errorMessage();
		return;
	}
	
    QStringList pathList = d->m_accountManager->allAccountPaths();
    kDebug() << "All Account Paths: " << pathList.size();
    
    /*
     * get a list of all the accounts that
     * are all ready there
     */
    QList<Telepathy::Client::Account *> accounts = d->m_accountManager->allAccounts();
    kDebug() << "accounts: " << accounts.size();
    
    /*
     * create a datasource for each
     * of the accounts we got in the list.
     */
    foreach(const QString &path, pathList)
    {
        d->createAccountDataSource(path);
    }
}

/**
 *  Slot for new account.
 * 
 * \param path QDBusObjectPath to created account.
 */
void PresenceEngine::accountCreated(const QString &path)
{
    kDebug() << "accountCreated() called";
    // Load the data for the new account. To avoid duplicating code, we treat
    // this just as if an account was updated, and call the method to handle
    // that.
    d->createAccountDataSource(path);
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
    kDebug() << "accountValidityChanged() called";
    /*
     * slot called when an account has
     * been updated.
     */
    d->createAccountDataSource(path);
}

/**
 * Slot for account removed.
 * 
 * \param QDBusObjectPath Name of the account path.
 */
void PresenceEngine::accountRemoved(const QString &path)
{
    kDebug() << "accountRemoved() called";
    /*
     * slot called when an account has been deleted
     *
     * remove that source.
     */
    d->removeAccountDataSource(path);
}

#include "presence.moc"

