/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef CNTFILTERRELATIONSHIP_H_
#define CNTFILTERRELATIONSHIP_H_

#include "cntabstractcontactfilter.h"
#include "cntsymbiansrvconnection.h"
#include "cntdbinfo.h"

#include <qmobilityglobal.h>

QTM_USE_NAMESPACE

class CntFilterRelationship : public CntAbstractContactFilter
{
public:
    CntFilterRelationship(CContactDatabase& contactDatabase, CntSymbianSrvConnection &cntServer,CntDbInfo& dbInfo);
    ~CntFilterRelationship();
    QList<QContactLocalId> contacts(
            const QContactFilter &filter,
            const QList<QContactSortOrder> &sortOrders,
            bool &filterSupported,
            QContactManager::Error* error);
    bool filterSupported(const QContactFilter& filter) ;
    
    //bool isFilterSupported(const QContactFilter& filter) const;

    void CntFilterRelationship::getSqlquery( const QContactRelationshipFilter& relationfilter,
                                                   QString& sqlquery ,
                                                   QContactManager::Error* error) const;
    void CntFilterRelationship::createSelectQuery(const QContactFilter& detailFilter,
                                  QString& sqlQuery,
                                  QContactManager::Error* error);
protected:
    CContactDatabase& m_contactdatabase;
    CntSymbianSrvConnection &m_srvConnection;
    CntDbInfo& m_dbInfo;
};

#endif //CNTFILTERRELATIONSHIP_H_
