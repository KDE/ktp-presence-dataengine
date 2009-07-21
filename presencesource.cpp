/*
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

#include "presencesource.h"

#include "presenceservice.h"

#include <KDebug>

#include <TelepathyQt4/Account>
#include <TelepathyQt4/Constants>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>

PresenceSource::PresenceSource(const Tp::AccountPtr &account, QObject *parent)
    : Plasma::DataContainer(parent),
      m_account(account)
{
    kDebug() << "PresenceSource created for account:" <<
        account->uniqueIdentifier();

    // Set the object name (which will be the name of the source)
    setObjectName(m_account->uniqueIdentifier());

    // Make the account become ready with the desired features
    connect(m_account->becomeReady(
            Tp::Account::FeatureProtocolInfo|Tp::Account::FeatureAvatar),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onAccountReady(Tp::PendingOperation*)));
}

PresenceSource::~PresenceSource()
{
}

Plasma::Service *PresenceSource::createService()
{
    return new PresenceService(this);
}

Tp::AccountPtr PresenceSource::account() const
{
    return m_account;
}

void PresenceSource::onAccountReady(Tp::PendingOperation *op)
{
    // Check if the operation succeeded or not
    if (op->isError()) {
        kWarning() << "PresenceSource::onAccountReady: readying "
            "Account failed:" << op->errorName() << ":" << op->errorMessage();
        return;
    }

    // Check that the account is valid
    if (!m_account->isValidAccount()) {
        // TODO should source be removed?
        kWarning() << "Invalid account in source:" << objectName();
        return;
    }

    connect(m_account.data(),
            SIGNAL(currentPresenceChanged(const Tp::SimplePresence &)),
            SLOT(onAccountCurrentPresenceChanged(const Tp::SimplePresence &)));
    connect(m_account.data(),
            SIGNAL(nicknameChanged(const QString &)),
            SLOT(onNicknameChanged(const QString &)));
    connect(m_account.data(),
            SIGNAL(displayNameChanged(const QString &)),
            SLOT(onDisplayNameChanged(const QString &)));
    connect(m_account.data(),
            SIGNAL(avatarChanged(const Tp::Avatar &)),
            SLOT(onAvatarChanged(const Tp::Avatar &)));

    // Force initial settings
    onAccountCurrentPresenceChanged(m_account->currentPresence());
    onNicknameChanged(m_account->nickname());
    onDisplayNameChanged(m_account->displayName());
    onAvatarChanged(m_account->avatar());
}

void PresenceSource::onAccountCurrentPresenceChanged(
        const Tp::SimplePresence &presence)
{
    // Update the data of this source
    setData("PresenceType", presenceTypeToString(presence.type));
    setData("PresenceStatus", presence.status);
    setData("PresenceStatusMessage", presence.statusMessage);

    // Required to trigger emission of update signal after changing data
    checkForUpdate();
}

void PresenceSource::onNicknameChanged(
        const QString &nickname)
{
    // Update the data of this source
    setData("Nickname", nickname);

    // Required to trigger emission of update signal after changing data
    checkForUpdate();
}

void PresenceSource::onDisplayNameChanged(
        const QString &displayName)
{
    // Update the data of this source
    setData("DisplayName", displayName);

    // Required to trigger emission of update signal after changing data
    checkForUpdate();
}

void PresenceSource::onAvatarChanged(
        const Tp::Avatar &avatar)
{
    // Update the data of this source
    setData("AccountAvatar", avatar.avatarData);

    // Required to trigger emission of update signal after changing data
    checkForUpdate();
}

QString PresenceSource::presenceTypeToString(uint type)
{
    // This method converts a presence type from a telepathy SimplePresence
    // struct to a string representation for data sources.
    QString ret;

    switch (type) {
    case Tp::ConnectionPresenceTypeUnset:
        ret = "unset";
        break;
    case Tp::ConnectionPresenceTypeOffline:
        ret = "offline";
        break;
    case Tp::ConnectionPresenceTypeAvailable:
        ret = "available";
        break;
    case Tp::ConnectionPresenceTypeAway:
    case Tp::ConnectionPresenceTypeExtendedAway:
        ret = "away";
        break;
    case Tp::ConnectionPresenceTypeHidden:
        ret = "invisible";
        break;
    case Tp::ConnectionPresenceTypeBusy:
        ret = "busy";
        break;
    case Tp::ConnectionPresenceTypeError:
        ret = "error";
        break;
    default:
        ret = "unknown";
        break;
    }

    return ret;
}

#include "presencesource.moc"

