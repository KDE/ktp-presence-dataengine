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

#include "setnicknamejob.h"

#include "presencesource.h"

#include <KDebug>

#include <TelepathyQt4/Account>
#include <TelepathyQt4/Constants>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/Types>

SetNicknameJob::SetNicknameJob(PresenceSource *source,
        const QMap< QString, QVariant > &parameters,
        QObject *parent)
    : Plasma::ServiceJob(source->objectName(), "setNickname", parameters, parent)
    , m_account(source->account())
{

}

void SetNicknameJob::start()
{
    // Call the appropriate method on the Account object
    connect(m_account->setNickname(parameters()["nickname"].toString()),
            SIGNAL(finished(Tp::PendingOperation*)),
            this,
            SLOT(onSetNicknameFinished(Tp::PendingOperation*)));
}

void SetNicknameJob::onSetNicknameFinished(Tp::PendingOperation* op)
{
    setError(op->isError());
    QString errorText;
    errorText.append(op->errorName());
    errorText.append(" : ");
    errorText.append(op->errorMessage());
    setErrorText(errorText);
    setResult(op->isValid());
}

#include "setnicknamejob.moc"
