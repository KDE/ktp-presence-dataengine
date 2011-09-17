/*
 * Copyright (C) 2009-2011 Collabora Ltd <http://www.collabora.co.uk>
 * Copyright (C) 2011 Dario Freddi <drf@kde.org>
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

#include "setavatarjob.h"

#include "presencesource.h"

#include <KDebug>
#include <KIO/Job>
#include <KMimeType>

#include <TelepathyQt4/Account>
#include <TelepathyQt4/Constants>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/Types>

SetAvatarJob::SetAvatarJob(PresenceSource *source,
        const QMap< QString, QVariant > &parameters,
        QObject *parent)
    : Plasma::ServiceJob(source->objectName(), "setAvatar", parameters, parent)
    , m_account(source->account())
{

}

void SetAvatarJob::start()
{
    if (parameters().contains("url")) {
        KUrl url(parameters().value("url").toString());

        KIO::TransferJob *job = KIO::get(url);

        connect(job, SIGNAL(data(KIO::Job*,QByteArray)),
                this, SLOT(onDataFromJob(KIO::Job*,QByteArray)));
        connect(job, SIGNAL(mimetype(KIO::Job*,QString)),
                this, SLOT(onMimeTypeRetrieved(KIO::Job*,QString)));
        connect(job, SIGNAL(result(KJob*)),
                this, SLOT(onJobFinished(KJob*)));
    } else if (parameters().contains("rawdata")) {
        m_avatar.avatarData = parameters().value("rawdata").toByteArray();

        populateMimeTypeAndStart();
    }
}

void SetAvatarJob::onMimeTypeRetrieved(KIO::Job *job, const QString &mimetype)
{
    Q_UNUSED(job)

    m_avatar.MIMEType = mimetype;
}

void SetAvatarJob::onDataFromJob(KIO::Job *job, const QByteArray& data)
{
    Q_UNUSED(job)

    m_avatar.avatarData.append(data);
}

void SetAvatarJob::onJobFinished(KJob *job)
{
    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
        setResult(false);
    } else {
        populateMimeTypeAndStart();
    }
}

void SetAvatarJob::populateMimeTypeAndStart()
{
    if (m_avatar.MIMEType.isEmpty()) {
        KMimeType::Ptr mimeType = KMimeType::findByContent(m_avatar.avatarData);
        m_avatar.MIMEType = mimeType->name();
    }

    connect(m_account->setAvatar(m_avatar),
            SIGNAL(finished(Tp::PendingOperation*)),
            this,
            SLOT(onSetAvatarFinished(Tp::PendingOperation*)));
}

void SetAvatarJob::onSetAvatarFinished(Tp::PendingOperation* op)
{
    setError(op->isError());
    QString errorText;
    errorText.append(op->errorName());
    errorText.append(" : ");
    errorText.append(op->errorMessage());
    setErrorText(errorText);
    setResult(op->isValid());
}

#include "setavatarjob.moc"
