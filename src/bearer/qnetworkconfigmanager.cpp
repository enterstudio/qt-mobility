/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qnetworkconfigmanager.h"

#ifdef Q_OS_SYMBIAN
#include "qnetworkconfigmanager_s60_p.h"
#else
#include "qnetworkconfigmanager_p.h"
#endif

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QNetworkConfigurationManagerPrivate, connManager);

/*!
    \class QNetworkConfigurationManager

    \brief The QNetworkConfigurationManager class manages the network configurations provided
    by the system.

    \inmodule QtNetwork
    \ingroup bearer

    QNetworkConfigurationManager provides access to the network configurations known to the system and
    enables applications to detect the system capabilities (with regards to network sessions) at runtime.

    A QNetworkConfiguration abstracts a set of configuration options describing how a
    network interface has to be configured to connect to a particular target network.
    QNetworkConfigurationManager maintains and updates the global list of
    QNetworkConfigurations. Applications can access and filter this list via
    allConfigurations(). If a new configuration is added or an existing one is removed or changed
    the configurationAdded(), configurationRemoved() and configurationChanged() signals are emitted
    respectively.

    The defaultConfiguration() can be used when intending to immediately create a new
    network session without caring about the particular configuration. It returns
    a \l QNetworkConfiguration::Discovered configuration. If there are not any
    discovered ones an invalid configuration is returned.

    Some configuration updates may require some time to perform updates. A WLAN scan is
    such an example. Unless the platform performs internal updates it may be required to
    manually trigger configuration updates via QNetworkConfigurationManager::updateConfigurations().
    The completion of the update process is indicted by emitting the updateCompleted()
    signal. The update process ensures that every existing QNetworkConfiguration instance
    is updated. There is no need to ask for a renewed configuration list via allConfigurations().

    \sa QNetworkConfiguration
*/

/*! 
    \fn void QNetworkConfigurationManager::configurationAdded(const QNetworkConfiguration& config)

    This signal is emitted whenever a new network configuration is added to the system. The new
    configuration is specified by \a config.
*/

/*!
    \fn void QNetworkConfigurationManager::configurationRemoved(const QNetworkConfiguration& configuration)

    This signal is emitted when a configuration is about to be removed from the system. The removed
    \a configuration is invalid but retains name and identifier.
*/

/*!
    \fn void QNetworkConfigurationManager::updateCompleted()

    This signal is emitted when the configuration update has been completed. Such an update can
    be initiated via \l updateConfigurations().
*/

/*! \fn void QNetworkConfigurationManager::configurationChanged(const QNetworkConfiguration& config)

    This signal is emitted when the \l {QNetworkConfiguration::state()}{state} of \a config changes.
*/

/*!
    \fn void QNetworkConfigurationManager::onlineStateChanged(bool isOnline)

    This signal is emitted when the device changes from online to offline mode or vice versa.
    \a isOnline represents the new state of the device.

    The state is considered to be online for as long as
    \l{allConfigurations()}{allConfigurations}(QNetworkConfiguration::Active) returns a list with
    at least one entry.
*/

/*!
    \enum QNetworkConfigurationManager::CapabilityFlag

    Specifies the system capabilities of the bearer API. The possible values are:

    \value BearerManagement         Network sessions and their underlying access points can be
                                    started and stopped. If this flag is not set QNetworkSession
                                    can only monitor but not influence the state of access points.
                                    On some platforms this feature may require elevated user
                                    permissions. This option is platform specific and may not
                                    always be available.
    \value DirectConnectionRouting  Network sessions and their sockets can be bound to a
                                    particular network interface. Any packet that passes through
                                    the socket goes to the specified network interface and thus
                                    disregards standard routing table entries. This may be useful
                                    when two interfaces can reach overlapping IP ranges or an
                                    application has specific needs in regards to target networks.
                                    This option is platform specific and may not always be
                                    available.
    \value SystemSessionSupport     If this flag is set the underlying platform ensures that a
                                    network interface is not shut down until the last network
                                    session has been \l{QNetworkSession::close()}{closed()}. This
                                    works across multiple processes. If the platform session
                                    support is missing this API can only ensure the above behavior
                                    for network sessions within the same process.
                                    In general mobile platforms (such as Symbian/S60) have such
                                    support whereas most desktop platform lack this capability.
    \value ApplicationLevelRoaming  The system gives applications control over the systems roaming
                                    behavior. Applications can initiate roaming (in case the
                                    current link is not suitable) and are consulted if the system
                                    has identified a more suitable access point.
    \value ForcedRoaming            The system disconnects an existing access point and reconnects
                                    via a more suitable one. The application does not have any
                                    control over this process and has to reconnect its active
                                    sockets.
    \value DataStatistics           If this flag is set QNetworkSession can provide statistics
                                    about transmitted and received data.
*/

/*!
    Constructs a QNetworkConfigurationManager with the given \a parent.
*/
QNetworkConfigurationManager::QNetworkConfigurationManager( QObject* parent )
    : QObject(parent)
{
    QNetworkConfigurationManagerPrivate* priv = connManager();
    connect(priv, SIGNAL(configurationAdded(QNetworkConfiguration)),
            this, SIGNAL(configurationAdded(QNetworkConfiguration)));
    connect(priv, SIGNAL(configurationRemoved(QNetworkConfiguration)),
            this, SIGNAL(configurationRemoved(QNetworkConfiguration)));
    connect(priv, SIGNAL(configurationUpdateComplete()),
            this, SIGNAL(updateCompleted()));
    connect(priv, SIGNAL(onlineStateChanged(bool)), 
            this, SIGNAL(onlineStateChanged(bool)));
    connect(priv, SIGNAL(configurationChanged(QNetworkConfiguration)),
            this, SIGNAL(configurationChanged(QNetworkConfiguration)));
}

/*!
    Frees the resources associated with the QNetworkConfigurationManager object.
*/
QNetworkConfigurationManager::~QNetworkConfigurationManager()
{
}


/*!
    Returns the default configuration to be used. This function always returns a discovered
    configuration; otherwise an invalid configuration.

    In some cases it may be required to call updateConfigurations() and wait for the
    updateCompleted() signal before calling this function.

    \sa allConfigurations()
*/
QNetworkConfiguration QNetworkConfigurationManager::defaultConfiguration() const
{
    return connManager()->defaultConfiguration();
}

/*!
    Returns the list of configurations which comply with the given \a filter.

    By default this function returns all (defined and undefined) configurations.

    A wireless network with a particular SSID may only be accessible in a
    certain area despite the fact that the system has a valid configuration
    for it. Therefore the filter flag may be used to limit the list to
    discovered and possibly connected configurations only.

    If \a filter is set to zero this function returns all possible configurations.

    Note that this function returns the states for all configurations as they are known at
    the time of this function call. If for instance a configuration of type WLAN is defined
    the system may have to perform a WLAN scan in order to determine whether it is
    actually available. To obtain the most accurate state updateConfigurations() should
    be used to update each configuration's state. Note that such an update may require
    some time. It's completion is signalled by updateCompleted(). In the absence of a
    configuration update this function returns the best estimate at the time of the call.
*/
QList<QNetworkConfiguration> QNetworkConfigurationManager::allConfigurations(QNetworkConfiguration::StateFlags filter) const
{
    QList<QNetworkConfiguration> result;
    QNetworkConfigurationManagerPrivate* conPriv = connManager();
    QList<QString> cpsIdents = conPriv->accessPointConfigurations.keys();

    //find all InternetAccessPoints
    foreach( QString ii, cpsIdents) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> p = 
            conPriv->accessPointConfigurations.value(ii);
        if ( (p->state & filter) == filter ) {
            QNetworkConfiguration pt;
            pt.d = conPriv->accessPointConfigurations.value(ii);
            result << pt;
        }
    }

    //find all service networks
    cpsIdents = conPriv->snapConfigurations.keys();
    foreach( QString ii, cpsIdents) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> p = 
            conPriv->snapConfigurations.value(ii);
        if ( (p->state & filter) == filter ) {
            QNetworkConfiguration pt;
            pt.d = conPriv->snapConfigurations.value(ii);
            result << pt;
        }
    }

    return result;
}

/*!
    Returns the QNetworkConfiguration for \a identifier; otherwise returns an
    invalid QNetworkConfiguration.

    \sa QNetworkConfiguration::identifier()
*/
QNetworkConfiguration QNetworkConfigurationManager::configurationFromIdentifier(const QString& identifier) const
{
    QNetworkConfigurationManagerPrivate* conPriv = connManager();
    QNetworkConfiguration item;
    if (conPriv->accessPointConfigurations.contains(identifier))
        item.d = conPriv->accessPointConfigurations.value(identifier);
    else if (conPriv->snapConfigurations.contains(identifier))
        item.d = conPriv->snapConfigurations.value(identifier);
    else if (conPriv->userChoiceConfigurations.contains(identifier))
        item.d = conPriv->userChoiceConfigurations.value(identifier);
    return item;

}

/*!
    Returns the capabilities supported by the current platform.
*/
QNetworkConfigurationManager::CapabilityFlags QNetworkConfigurationManager::capabilities() const
{
    return connManager()->capFlags;
}

/*!
    Initiates an update of all configurations. This may be used to initiate WLAN scans or other
    time consuming updates which may be required to obtain the correct state for configurations.

    This call is asynchronous. On completion of this update the updateCompleted() signal is
    emitted. If new configurations are discovered or old ones were removed or changed the update
    process may trigger the emission of one or multiple configurationAdded(),
    configurationRemoved() and configurationChanged() signals.

    If a configuration state changes as a result of this update all existing QNetworkConfiguration
    instances are updated automatically.

    \sa allConfigurations()
*/
void QNetworkConfigurationManager::updateConfigurations()
{
    connManager()->performAsyncConfigurationUpdate();
}

#include "moc_qnetworkconfigmanager.cpp"

QTM_END_NAMESPACE
