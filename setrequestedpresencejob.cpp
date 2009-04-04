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

#include "setrequestedpresencejob.h"

#include "presencesource.h"

#include <KDebug>

#include <TelepathyQt4/Client/PendingOperation>
#include <TelepathyQt4/Types>

SetRequestedPresenceJob::SetRequestedPresenceJob(PresenceSource *source,
                                                 const QMap<QString, QVariant> &parameters,
                                                 QObject *parent)
 : Plasma::ServiceJob(source->objectName(), "setRequestedPresence", parameters, parent),
   m_account(source->account())
{
    kDebug();   // Output the method we are in.

    // FIXME: Check the parameters passed are valid here.
}

void SetRequestedPresenceJob::start()
{
    kDebug();   // Output the method we are in.
    
    // Call the appropriate method on the Account object
    Telepathy::SimplePresence rp;
    rp.status = parameters().value("status").toString();
    rp.statusMessage = parameters().value("statusMessage").toString();

    // FIXME: Will all hell break lose here if we don't set the presence type?
    // FIXME: What happens if there was a status message before, but we set without one this time. Does it stay the same?

    connect(m_account->setRequestedPresence(rp),
            SIGNAL(finished(Telepathy::Client::PendingOperation*)),
            this,
            SLOT(onSetRequestedPresenceFinished(Telepathy::Client::PendingOperation*)));
}

void SetRequestedPresenceJob::onSetRequestedPresenceFinished(Telepathy::Client::PendingOperation *op)
{
    setError(op->isError());
    QString errorText;
    errorText.append(op->errorName());
    errorText.append(" : ");
    errorText.append(op->errorMessage());
    setErrorText(errorText);
    setResult(op->isValid());
}


#include "setrequestedpresencejob.moc"

