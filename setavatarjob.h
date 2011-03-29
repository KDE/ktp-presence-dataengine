/*
 * Copyright (C) 2009-2011 Collabora Ltd <http://www.collabora.co.uk>
 * Copyright (C) 2011 Dario Freddi <drf@kde.org>
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

#ifndef PLASMA_DATAENGINE_PRESENCE_SETAVATARJOB_H
#define PLASMA_DATAENGINE_PRESENCE_SETAVATARJOB_H

#include <Plasma/ServiceJob>

#include <TelepathyQt4/Types>

class PresenceSource;

namespace KIO {
    class Job;
}

namespace Tp {
    class PendingOperation;
}

class SetAvatarJob : public Plasma::ServiceJob
{
    Q_OBJECT

public:
    SetAvatarJob(PresenceSource *source,
            const QMap< QString, QVariant > &parameters,
            QObject *parent = 0);
    virtual void start();

private Q_SLOTS:
    void onDataFromJob(KIO::Job *job, const QByteArray &data);
    void onMimeTypeRetrieved(KIO::Job *job, const QString &mimetype);
    void onJobFinished(KJob *job);
    void onSetAvatarFinished(Tp::PendingOperation *op);

private:
    void populateMimeTypeAndStart();

    Tp::AccountPtr m_account;
    Tp::Avatar m_avatar;
};

#endif
