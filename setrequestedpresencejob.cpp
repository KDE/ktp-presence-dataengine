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

#include "setrequestedpresencejob.h"

#include "presencesource.h"

#include <KDebug>

#include <TelepathyQt4/Account>
#include <TelepathyQt4/Constants>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/Types>

SetRequestedPresenceJob::SetRequestedPresenceJob(PresenceSource *source,
        const QMap<QString, QVariant> &parameters,
        QObject *parent)
    : Plasma::ServiceJob(source->objectName(), "setPresence",
                         parameters, parent),
      m_account(source->account())
{
}

void SetRequestedPresenceJob::start()
{
    // Call the appropriate method on the Account object
    Tp::SimplePresence rp;
    rp.status = parameters().value("status").toString();
    rp.type = presenceStringToType(rp.status);
    if (rp.type == Tp::ConnectionPresenceTypeError) {
        kWarning() << "SetRequestedPresenceJob::start: invalid presence "
            "status:" << rp.status;
        return;
    }
    rp.statusMessage = parameters().value("status_message").toString();

    connect(m_account->setRequestedPresence(rp),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onSetRequestedPresenceFinished(Tp::PendingOperation*)));
}

void SetRequestedPresenceJob::onSetRequestedPresenceFinished(
        Tp::PendingOperation *op)
{
    setError(op->isError());
    QString errorText;
    errorText.append(op->errorName());
    errorText.append(" : ");
    errorText.append(op->errorMessage());
    setErrorText(errorText);
    setResult(op->isValid());
}

uint SetRequestedPresenceJob::presenceStringToType(const QString &status)
{
    // This method converts a presence status to a uint representing the
    // Telepathy presence type
    if (status == "available") {
        return Tp::ConnectionPresenceTypeAvailable;
    } else if (status == "offline") {
        return Tp::ConnectionPresenceTypeOffline;
    } else if (status == "away") {
        return Tp::ConnectionPresenceTypeAway;
    } else if (status == "xa") {
        return Tp::ConnectionPresenceTypeExtendedAway;
    } else if (status == "invisible") {
        return Tp::ConnectionPresenceTypeHidden;
    } else if (status == "busy") {
        return Tp::ConnectionPresenceTypeBusy;
    }
    return Tp::ConnectionPresenceTypeError;
}

#include "setrequestedpresencejob.moc"

