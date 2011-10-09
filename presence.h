/*
 * Copyright (C) 2008 George Goldberg <grundleborg@googlemail.com>
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

#ifndef PLASMA_DATAENGINE_PRESENCE_PRESENCE_H
#define PLASMA_DATAENGINE_PRESENCE_PRESENCE_H

#include <Plasma/DataEngine>

#include <QDBusAbstractAdaptor>

#include <TelepathyQt4/Types>

namespace Plasma
{
    class Service;
}

namespace Tp
{
    class PendingOperation;
}

class GlobalPresenceSource;
class PresenceEngine;

class DBusExporter : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Telepathy.PresenceEngine")

public:
    DBusExporter(QObject *parent = 0);
    ~DBusExporter();
};


class PresenceEngine : public Plasma::DataEngine
{
    Q_OBJECT

public:
    PresenceEngine(QObject *parent, const QVariantList &args);
    ~PresenceEngine();

    Plasma::Service *serviceForSource(const QString &name);

protected:
    void init();
    bool sourceRequestEvent(const QString &name);

private Q_SLOTS:
    void onAccountManagerReady(Tp::PendingOperation *op);
    void addAccount(const Tp::AccountPtr &account);
    void onAccountRemoved(const QString &path = QString());
    void onAccountValidityChanged(bool valid);

private:
    Tp::AccountManagerPtr m_accountManager;
    DBusExporter *m_dbusExporter;
    GlobalPresenceSource *m_globalPresenceSource;
};

K_EXPORT_PLASMA_DATAENGINE(presence, PresenceEngine)

#endif

