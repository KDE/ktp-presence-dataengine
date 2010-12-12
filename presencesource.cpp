/*
 * Copyright (C) 2009-2010 Collabora Ltd <http://www.collabora.co.uk>
 * Copyright (C) 2009 Andre Moreira Magalhaes <andrunko@gmail.com>
 * Copyright (C) 2010 Dario Freddi <drf@kde.org>
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
#include <KTemporaryFile>

#include <TelepathyQt4/Account>
#include <TelepathyQt4/Constants>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>

PresenceSource::PresenceSource(const Tp::AccountPtr &account, QObject *parent)
    : Plasma::DataContainer(parent),
      m_account(account)
{
    kDebug() << "PresenceSource created for account:" <<
        account->objectPath();

    // Set the object name (which will be the name of the source)
    setObjectName(m_account->objectPath());

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
    setData("PresenceTypeID", presenceTypeToID(presence.type));
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
    // Is the data empty?
    if (avatar.avatarData.isEmpty()) {
        // Set an empty string
        setData("AccountAvatar", "");
    } else {
        // Create a temp file and use it to feed the engine
        if (!m_tempAvatar.isNull()) {
            m_tempAvatar.data()->deleteLater();
        }
        m_tempAvatar = new KTemporaryFile();
        m_tempAvatar.data()->setAutoRemove(true);
        m_tempAvatar.data()->open();
        m_tempAvatar.data()->write(avatar.avatarData);
        m_tempAvatar.data()->flush();

        QFileInfo info(*(m_tempAvatar.data()));

        setData("AccountAvatar", info.absoluteFilePath());
    }

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

uint PresenceSource::presenceTypeToID(uint type)
{
    uint ret;
    switch (type) {
    case Tp::ConnectionPresenceTypeUnset:
        ret = 6;
        break;
    case Tp::ConnectionPresenceTypeOffline:
        ret = 5;
        break;
    case Tp::ConnectionPresenceTypeAvailable:
        ret = 1;
        break;
    case Tp::ConnectionPresenceTypeAway:
    case Tp::ConnectionPresenceTypeExtendedAway:
        ret = 3;
        break;
    case Tp::ConnectionPresenceTypeHidden:
        ret = 4;
        break;
    case Tp::ConnectionPresenceTypeBusy:
        ret = 2;
        break;
    case Tp::ConnectionPresenceTypeError:
    default:
        ret = 99;
        break;
    }

    return ret;
}

#include "presencesource.moc"

