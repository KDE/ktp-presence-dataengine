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

#ifndef PLASMA_DATAENGINE_PRESENCE_H
#define PLASMA_DATAENGINE_PRESENCE_H

#include <plasma/dataengine.h>

class QDBusObjectPath;

class PresenceEngine : public Plasma::DataEngine
{
    Q_OBJECT

public:
    PresenceEngine(QObject * parent, const QVariantList & args);
    ~PresenceEngine();

protected:
    void init();
    bool sourceRequestEvent(const QString & name);

private Q_SLOTS:
    void accountCreated(const QDBusObjectPath &path);
    void accountRemoved(const QDBusObjectPath &path);
    void accountValidityChanged(const QDBusObjectPath &path, bool valid);

private:
	class PresenceEnginePrivate;
	PresenceEnginePrivate * const d;
};

K_EXPORT_PLASMA_DATAENGINE(presence, PresenceEngine)

#endif

