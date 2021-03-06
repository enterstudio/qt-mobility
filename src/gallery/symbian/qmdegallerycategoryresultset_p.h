/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMDEGALLERYCATEGORYRESULTSET_P_H
#define QMDEGALLERYCATEGORYRESULTSET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qgalleryresultset.h"

#include "qgalleryfilter.h"

#include <mdelogiccondition.h>
#include <mdenamespacedef.h>
#include <mdeobjectquery.h>


QTM_BEGIN_NAMESPACE

class QMdeSession;

class QMDEGalleryCategoryResultSet : public QGalleryResultSet, public MMdEQueryObserver
{
    Q_OBJECT
public:
    static int appendScopeCondition(
            CMdELogicCondition *condition,
            const QVariant &rootItem,
            CMdENamespaceDef &namespaceDef);

    static int createTypeQuery(
            QScopedPointer<CMdEObjectQuery> *query,
            QMdeSession *session,
            const QString &itemType,
            MMdEQueryObserver *observer);

    static bool isCategoryType(const QString &itemType);

    static QString itemIdType(const QString &itemId);

    QMDEGalleryCategoryResultSet(
            QMdeSession *session,
            const QString &itemType,
            const QStringList &propertyNames,
            const QStringList &sortPropertyNames,
            const QVariant &rootItem,
            const QGalleryFilter &filter,
            int offset,
            int limit,
            QObject *parent = 0);
    ~QMDEGalleryCategoryResultSet();

    int propertyKey(const QString &property) const;
    QGalleryProperty::Attributes propertyAttributes(int key) const;
    QVariant::Type propertyType(int key) const;

    int itemCount() const;

    bool isValid() const;

    QVariant itemId() const;
    QUrl itemUrl() const;
    QString itemType() const;
    QList<QGalleryResource> resources() const;

    QVariant metaData(int key) const;
    bool setMetaData(int key, const QVariant &value);

    int currentIndex() const;
    bool fetch(int index);

    void cancel();

    void HandleQueryNewResults(CMdEQuery &aQuery, TInt aFirstNewItemIndex, TInt aNewItemCount);
    void HandleQueryCompleted(CMdEQuery& aQuery, TInt aError);


private:
    const QString m_itemType;
    const QStringList m_propertyNames;
    const QStringList m_sortPropertyNames;
    const QVariant m_rootItem;
    const QGalleryFilter m_filter;
    const int m_offset;
    const int m_limit;
    int m_count;
    int m_currentIndex;
    QScopedPointer<CMdEObjectQuery> m_query;

};

QTM_END_NAMESPACE

#endif
