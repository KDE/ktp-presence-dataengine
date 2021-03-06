/*
 * Copyright (C) 2009 Collabora Ltd <http://www.collabora.co.uk>
 * Copyright (C) 2009 Andre Moreira Magalhaes <andrunko@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#ifndef PLASMA_DATAENGINE_PRESENCE_PRESENCESOURCE_H
#define PLASMA_DATAENGINE_PRESENCE_PRESENCESOURCE_H

#include <Plasma/DataContainer>

#include <QtCore/QObject>

#include <TelepathyQt4/Presence>
#include <TelepathyQt4/Types>

class KTemporaryFile;

namespace Plasma
{
    class Service;
}

namespace Tp
{
    class PendingOperation;
}

class PresenceSource : public Plasma::DataContainer
{
    Q_OBJECT

public:
    explicit PresenceSource(const Tp::AccountPtr &account, QObject *parent = 0);
    ~PresenceSource();

    Plasma::Service *createService();

    Tp::AccountPtr account() const;

private Q_SLOTS:
    void onAccountReady(Tp::PendingOperation *op);
    void onAccountCurrentPresenceChanged(const Tp::Presence &presence);
    void onNicknameChanged(const QString &nickname);
    void onDisplayNameChanged(const QString &displayName);
    void onAvatarChanged(const Tp::Avatar &avatar);
    void onIconNameChanged(const QString &iconName);
    void onStateChanged(bool state);

private:
    QString presenceTypeToString(uint type);
    uint presenceTypeToID(uint type);

    Tp::AccountPtr m_account;
    QWeakPointer< KTemporaryFile > m_tempAvatar;
};

#endif

