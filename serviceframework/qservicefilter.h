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

#ifndef QSERVICEFILTER_H
#define QSERVICEFILTER_H

#include <QStringList>

#include "qserviceglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDataStream;
class QServiceFilterPrivate;
class Q_SFW_EXPORT QServiceFilter
{
public:
    enum VersionMatchRule {
        ExactVersionMatch = 0,
        MinimumVersionMatch
    };

    enum CapabilityMatchRule {
        MatchAll = 0,
        MatchLoadable
    };

    QServiceFilter();
    ~QServiceFilter();
    QServiceFilter(const QServiceFilter& other);
    QServiceFilter(const QString& interfaceName,
                   const QString& version = QString(),
                   QServiceFilter::VersionMatchRule rule = QServiceFilter::MinimumVersionMatch);

    QServiceFilter& operator=(const QServiceFilter& other);

    void setInterface(const QString& interfaceName, const QString& version = QString(), 
            QServiceFilter::VersionMatchRule rule = QServiceFilter::MinimumVersionMatch);
    void setServiceName(const QString& serviceName);


    QString serviceName() const;
    QString interfaceName() const;
    int interfaceMajorVersion() const;
    int interfaceMinorVersion() const;
    VersionMatchRule versionMatchRule() const;

    QStringList customPropertyKeys() const;
    QString customProperty(const QString& key) const;
    void setCustomProperty(const QString& key, const QString& value);
    void removeCustomProperty(const QString &key);
    void clearCustomProperties();

    void setCapabilities(QServiceFilter::CapabilityMatchRule, const QStringList& capabilities = QStringList() );
    QStringList capabilities() const;
    CapabilityMatchRule capabilityMatchRule() const;

private:
    QServiceFilterPrivate *d;
    friend class QServiceManager;
    //friend class ServiceDatabase;
#ifndef QT_NO_DATASTREAM
    friend Q_SFW_EXPORT QDataStream &operator<<(QDataStream &, const QServiceFilter &);
    friend Q_SFW_EXPORT QDataStream &operator>>(QDataStream &, QServiceFilter &);
#endif
};

#ifndef QT_NO_DATASTREAM
Q_SFW_EXPORT QDataStream &operator<<(QDataStream &, const QServiceFilter &);
Q_SFW_EXPORT QDataStream &operator>>(QDataStream &, QServiceFilter &);
#endif

QT_END_NAMESPACE

QT_END_HEADER
#endif //QSERVICEFILTER_H