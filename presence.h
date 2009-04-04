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

#ifndef PLASMA_DATAENGINE_PRESENCE_PRESENCE_H
#define PLASMA_DATAENGINE_PRESENCE_PRESENCE_H

#include <Plasma/DataEngine>

namespace Plasma {
    class Service;
}

namespace Telepathy
{
    namespace Client
    {
        class AccountManager;
        class PendingOperation;
    }
}

class PresenceEngine : public Plasma::DataEngine
{
    Q_OBJECT

public:
    PresenceEngine(QObject * parent, const QVariantList & args);
    ~PresenceEngine();

    Plasma::Service * serviceForSource(const QString& name);

protected:
    void init();
    bool sourceRequestEvent(const QString & name);

private Q_SLOTS:
    void onAccountManagerReady(Telepathy::Client::PendingOperation *op);
    void onAccountCreated(const QString &path);
    void onAccountRemoved(const QString &path);
    void onAccountValidityChanged(const QString &path, bool valid);

private:
    Telepathy::Client::AccountManager *m_accountManager;
};

K_EXPORT_PLASMA_DATAENGINE(presence, PresenceEngine)


#endif // Include guard

