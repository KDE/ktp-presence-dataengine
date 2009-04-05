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

#ifndef PLASMA_DATAENGINE_PRESENCE_PRESENCESOURCE_H
#define PLASMA_DATAENGINE_PRESENCE_PRESENCESOURCE_H

#include <plasma/datacontainer.h>

#include <QtCore/QObject>

#include <TelepathyQt4/Client/Account>

namespace Plasma {
    class Service;
}

namespace Telepathy {
    namespace Client {
        class PendingOperation;
    }
}

class PresenceSource : public Plasma::DataContainer
{
    Q_OBJECT

public:
    explicit PresenceSource(Telepathy::Client::AccountPtr account, QObject *parent = 0);
    ~PresenceSource();

    Plasma::Service * createService();

    Telepathy::Client::AccountPtr account();

private Q_SLOTS:
    void onAccountReady(Telepathy::Client::PendingOperation *op);
    void onAccountCurrentPresenceChanged(const Telepathy::SimplePresence &);

private:
    QString presenceTypeToString(uint type);

    Telepathy::Client::AccountPtr m_account;
};


#endif // Header guard

