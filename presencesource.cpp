/*
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

#include "presencesource.h"

#include "presenceservice.h"

#include <kdebug.h>

#include <TelepathyQt4/Client/Feature>
#include <TelepathyQt4/Client/PendingOperation>
#include <TelepathyQt4/Client/PendingReady>
#include <TelepathyQt4/Constants>

PresenceSource::PresenceSource(Telepathy::Client::AccountPtr account, QObject *parent)
 : Plasma::DataContainer(parent),
   m_account(account)
{
    Q_ASSERT(m_account);

    kDebug() << "Presence Source created for Account:" << account->uniqueIdentifier();

    // Set the object name (which will be the name of the source)
    setObjectName(m_account->uniqueIdentifier());

    // Specify the features we want the account to become ready with
    QSet<Telepathy::Client::Feature> features;
    features << Telepathy::Client::Account::FeatureCore;
    // features << Telepathy::Client::Account::FeatureAvatar;  // FIXME: Uncomment me once t-a-k supports avatars
    features << Telepathy::Client::Account::FeatureProtocolInfo;

    // Make the account become ready with the desired features.
    connect(m_account.data()->becomeReady(features),
            SIGNAL(finished(Telepathy::Client::PendingOperation*)),
            this, SLOT(onAccountReady(Telepathy::Client::PendingOperation*)));
}

PresenceSource::~PresenceSource()
{
    kDebug() << "Destroying source for Account:" << objectName();
}

Plasma::Service * PresenceSource::createService()
{
    kDebug();   // Output the method we are in.

    return new PresenceService(this);
}

Telepathy::Client::AccountPtr PresenceSource::account()
{
    return m_account;
}

void PresenceSource::onAccountReady(Telepathy::Client::PendingOperation *op)
{
    kDebug();   // Output the method we are in.

    // Check if the operation succeeded or not.
    if(op->isError())
    {
        kWarning() << "Readying Account failed: " << op->errorName() << ":" << op->errorMessage();
        return;
    }

    // FIXME: Is it necessary to check that all the desired features were
    // become-readied successfully?

    // Check that the account is valid - and emit a warning otherwise
    Q_ASSERT(m_account->isValidAccount());
    if(!m_account->isValidAccount())
    {
        kWarning() << "Invalid account in source:" << objectName();
    }

    connect(m_account.data(), SIGNAL(currentPresenceChanged(const Telepathy::SimplePresence &)),
            this, SLOT(onAccountCurrentPresenceChanged(const Telepathy::SimplePresence &)));
    // FIXME: Should we connect to signals for any other type of information than just current presence?

    // Force initial setting of the current presence
    onAccountCurrentPresenceChanged(m_account->currentPresence());
}

void PresenceSource::onAccountCurrentPresenceChanged(const Telepathy::SimplePresence & presence)
{
    kDebug();   // Output the method we are in.

    // Update the data of this source
    setData("current_presence_type", presenceTypeToString(presence.type));
    setData("current_presence_status", presence.status);
    setData("current_presence_status_message", presence.statusMessage);
    // FIXME: Make things other than just current presence available for the source.

    // Required to trigger emission of update signal after changing data
    checkForUpdate();
}

QString PresenceSource::presenceTypeToString(uint type)
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


#include "presencesource.moc"

