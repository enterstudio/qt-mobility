/****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QLANDMARKUTILITY_SYMBIAN_H
#define QLANDMARKUTILITY_SYMBIAN_H

#include <qlandmarkmanagerengine.h>

#include <EPos_Landmarks.h>
#include <EPos_CPosLandmark.h>
#include <Epos_CposLandmarkDatabase.h>
#include <EPos_CPosLmCategoryManager.h> 
#include <EPos_CPosLandmarkCategory.h>
#include <EPos_TPosLmDatabaseEvent.h>
#include <lbsposition.h> 
#include <e32math.h>

#include <qlandmark.h>
#include <qlandmarkid.h>
#include <qlandmarkcategory.h>
#include <qlandmarkcategoryid.h>
#include <qgeoplace.h>
#include <qgeoaddress.h>
#include <qgeocoordinate.h>

QTM_USE_NAMESPACE

/*
 * TODO : The naming conventions needed to be change. 
 * all methods should return the bool values, except some methods.
 * all conversion and setting will be done by passing by values and references
 * wherever required.
 */

/*
 *
 * Landmark Utility To convert qt landmark data class to symbian landmark data classes
 * 
 */
class LandmarkUtility
{
public:

    /*
     * validates the lattitude values
     * with globally defined range
     */
    inline static bool isValidLat(double lat)
    {
        return lat >= -90 && lat <= 90;
    }

    /*
     * validates the longitude values
     * with globally defined range
     */
    inline static bool isValidLong(double lng)
    {
        return lng >= -180 && lng <= 180;
    }

    /*
     * returns new qt landmark object converted from symbian landmark object.
     */
    static QLandmark* convertToQtLandmark(QString mangerUri, CPosLandmark* symbianLandmark);

    /*
     * returns new symbian landmark converted from qt landmark object. 
     */
    static CPosLandmark* convertToSymbianLandmarkL(QLandmark* qtLandmark);

    /*
     * set landmark attributes to existing symbian landmark object from 
     * qt landmark object.
     */
    static void setSymbianLandmarkL(CPosLandmark& symbianLandmark, QLandmark* qtLandmark);

    /*
     * returns new qt landmark id object converted from symbian landmark id object.
     */
    static QLandmarkId convertToQtLandmarkId(QString mangerUri, TPosLmItemId symbianLandmarkId);

    /*
     * returns list of qt landmarks ids converted from symbian landmark id list.
     */
    static QList<QLandmarkId> convertToQtLandmarkIds(QString mangerUri,
        RArray<TPosLmItemId>& symbianLandmarkIds);

    /*
     * returns new object of symbian landmark id converted from qt landmark id. 
     */
    static TPosLmItemId convertToSymbianLandmarkId(QLandmarkId qtLandmarkId);

    /*
     * returns symbian landmark ids from qt landmark id list. 
     */
    static RArray<TPosLmItemId> getSymbianLandmarkIds(QList<QLandmarkId>& qtLandmarkIds);

    /*
     * returns new object of qt landmark category converted from symbian landmark category. 
     */
    static QLandmarkCategory* convertToQtLandmarkCategory(QString mangerUri,
        CPosLandmarkCategory* symbianLandmarkCategory);

    /*
     * returns the symbian landmark category converted from qt landmark category.
     */
    static CPosLandmarkCategory
    * convertToSymbianLandmarkCategoryL(QLandmarkCategory* qtLandmarkCategory);

    /*
     * sets landmark category attributes to existing symbian category objects
     * from qt landmark category object. 
     */
    static void setSymbianLandmarkCategoryL(CPosLandmarkCategory& symbianLmCategory,
        QLandmarkCategory* qtLandmarkCategory);

    /*
     * returns new object of qt category id coverted from symbian category object.
     */
    static QLandmarkCategoryId convertToQtLandmarkCategoryId(QString mangerUri,
        TPosLmItemId symbianLandmarkCategoryId);

    /*
     * returns list of category ids converted from symbian category ids.
     */
    static QList<QLandmarkCategoryId> convertToQtLandmarkCategoryIds(QString mangerUri, RArray<
        TPosLmItemId>& symbianCategoryIds);

    /*
     * returns symbian category id converted from qt landmark category id.
     */
    static TPosLmItemId
        convertToSymbianLandmarkCategoryId(QLandmarkCategoryId qtLandmarkCategoryId);

    /*
     * returns symbian coordinate object converted from qt coordinate object.
     */
    static TCoordinate getCoordinate(QGeoCoordinate qtCoordinate);

    /*
     * returns landmarks name in symbian desciptor format from qt landmark object.
     */
    static TDesC getSymbianLandmarkName(QLandmark* qtLandmark);

    /*
     * returns symbian landmark id from qt landmark object.
     */
    static TPosLmItemId getSymbianLandmarkId(QLandmark* qtLandmark);

    /*
     * returns list of qt category ids associated with landmark from symbian landmark object.
     */
    static QList<QLandmarkCategoryId> getCategoryIds(QString mangerUri,
        CPosLandmark* symbianLandmark);

    /*
     * checks the valid categoris of landmarks.
     * returns true, if valid categoy ids are associated with landmark object.
     * returns false, if invalid categories are associated with landmark object.
     * returns true, if no any category ids are associated with landmark object. 
     */
    static bool validCategoriesExist(CPosLmCategoryManager* catMgr, QLandmark* qtLandmark);

    /*
     * checks the valid local id
     * returns true, if local id having all numeric characts and value is unsigned long value.
     * returns false, if local id having some other chaaracts other than numeric characters.
     * returns false, if local id is empty.
     */
    static bool validLocalId(QString localId);

    /*
     * returns the list of qt landmark categories from list of qt category ids. 
     */
    static QList<QLandmarkCategory> getCategoriesL(CPosLmCategoryManager* catMgr, QList<
        QLandmarkCategoryId> qtCategoryIds);

    /*
     * returns the list of qt landmarks from the list of qt landmark ids.
     */
    static QList<QLandmark> getLandmarksL(CPosLandmarkDatabase* landmarkDb,
        QList<QLandmarkId> qtLandmarkIds);

    /* 
     * This method checks the provided category id is global category or not
     * 
     * Global category IDs as defined by Global Category API specification
     * Global category identifier when used for searching
     * 
     *      Accommodation       = 3000;  //Hotel, Camping site
     *      Business            = 6000;  //Bank, Factory, Office
     *      Communication       = 9000;  //Internet Access Point, Public Telephone, Wireless LAN Hot Spot
     *      Educational         = 12000; //School, College
     *      Entertainment       = 15000; //Amusement park, Cinema, Concert hall, Night club
     *      FoodBeverage        = 18000; //Fast food, Restaurant, Caf�, Bar
     *      GeographicalArea    = 21000; //City, City center, Town
     *      OutdoorActivities   = 24000; //Camping site, Fishing place, Hunting, National park, Playground
     *      People              = 27000; //My home, My friend's home, Father's summer cottage, Child's school
     *      PublicService       = 30000; //Tourist information office, Government office, Library, Post office, Hospital, Police
     *      ReligiousPlaces     = 33000; //Church, Mosque
     *      Shopping            = 36000; //Market place, Pharmacy, Shop, Shopping center
     *      Sightseeing         = 39000; //Monument, Mountain top, Museum
     *      Sports              = 42000; //Bowling, Golf course, Ice hockey hall, Stadium
     *      Transport           = 45000; //Airport, Bus stop, Harbor, Railway station, Rest area
     *          
     * This list may change from version to version without notice, 
     * or even be removed.
     *
     */
    static bool isGlobalCategoryId(CPosLmCategoryManager* catMgr, QLandmarkCategoryId qtCategoryId);

    /*
     * Returns Landmark Attribute keys
     * 
     * These are the symbian platform specific landmark attribute key list
     * relative to qt mobility api support.
     * 
     * This list may change from version to version without notice, 
     * or even be removed.
     *
     */
    static QStringList landmarkAttributeKeys();

    /*
     * Returns Landmark package formats
     *  
     * Thease formats are the symbian platform specific landmark package formats,
     * where landmarks can be packaged in different xml formats as defined in 
     * landmarks exchange format specification.
     * Thease formats are used to import and export of landmarks 
     * 
     * This list may change from version to version without notice, 
     * or even be removed.
     */
    static QStringList landmarkPackageFormats();
    
    
    /**
     * converts the attribute key string to symbian position field value. 
     */
    static TPositionFieldId LandmarkUtility::positionFieldId(QString keyValue);

};

#endif // QLANDMARKUTILITY_SYMBIAN_H
// end of file