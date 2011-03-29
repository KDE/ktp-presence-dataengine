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
    Tp::SimplePresence rp = parametersToSimplePresence(parameters());
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

Tp::SimplePresence SetRequestedPresenceJob::parametersToSimplePresence(const QVariantMap& parameters)
{
    Tp::SimplePresence rp;
    switch (parameters["type_id"].toUInt()) {
        case 1:
            rp.status = "available";
            rp.type = Tp::ConnectionPresenceTypeAvailable;
            break;
        case 2:
            rp.status = "busy";
            rp.type = Tp::ConnectionPresenceTypeBusy;
            break;
        case 3:
            rp.status = "away";
            rp.type = Tp::ConnectionPresenceTypeAway;
            break;
        case 4:
            rp.status = "invisible";
            rp.type = Tp::ConnectionPresenceTypeHidden;
            break;
        case 5:
            rp.status = "offline";
            rp.type = Tp::ConnectionPresenceTypeOffline;
            break;
        default:
            rp.type = Tp::ConnectionPresenceTypeError;
            break;
    }

    return rp;
}

#include "setrequestedpresencejob.moc"
