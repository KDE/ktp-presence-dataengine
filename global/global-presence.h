/*
 * Copyright (C) 20011 David Edmundson <david@davidedmundson.co.uk>
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

#ifndef GLOBALPRESENCE_H
#define GLOBALPRESENCE_H

#include <QObject>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/AccountSet>


/** This class handles the presence between all enabled accounts
 * It shows the highest current available presence, indicates if any accounts are changing, and what they are changing to.
*/

class GlobalPresence : public QObject
{
    Q_OBJECT
public:

    explicit GlobalPresence(QObject *parent = 0);

    /** Set the account manager to use
      * @param accountManager should be ready.
      */
    void setAccountManager(const Tp::AccountManagerPtr &accountManager);


    /** The most online presence of any account*/
    Tp::Presence currentPresence() const;

    /** The most online presence requested for any account if any of the accounts are changing state.
      otherwise returns current presence*/
    Tp::Presence requestedPresence() const;

    /** Returns true if any account is changing state (i.e connecting*/
    bool isChangingPresence() const;

    /** Set all enabled accounts to the specified presence*/
    void setPresence(const Tp::Presence &presence);

signals:
    void requestedPresenceChanged(const Tp::Presence &customPresence);
    void currentPresenceChanged(const Tp::Presence &presence);
    void changingPresence(bool isChanging);

public slots:

private slots:
    void onCurrentPresenceChanged();
    void onRequestedPresenceChanged();
    void onChangingPresence();

    void onAccountAdded(const Tp::AccountPtr &account);

private:
    Tp::AccountSetPtr m_enabledAccounts;

    /** A cache of the last sent requested presence, to avoid resignalling*/
    Tp::Presence m_requestedPresence;
    /** A cache of the last sent presence*/
    Tp::Presence m_currentPresence;
    bool m_changingPresence;

    /// Sets the sorting order of presences
    QHash<uint, int> m_presenceSorting;
};

#endif // GLOBALPRESENCE_H