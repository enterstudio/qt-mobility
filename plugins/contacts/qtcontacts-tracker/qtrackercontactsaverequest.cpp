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


#include "qtrackercontactsaverequest.h"

#include <QtTracker/Tracker>
using namespace SopranoLive;

#include "qtrackercontactslive.h"

// TODO better error handling when saving
QTrackerContactSaveRequest::QTrackerContactSaveRequest(QContactAbstractRequest* req, QContactManagerEngine* parent)
: QObject(parent), QTrackerContactAsyncRequest(req)
{
    Q_ASSERT(req);
    Q_ASSERT(req->type() == QContactAbstractRequest::ContactSaveRequest);
    Q_ASSERT(parent);

    QContactSaveRequest* r = static_cast<QContactSaveRequest*>(req);
    QList<QContact> contacts = r->contacts();

    if(contacts.isEmpty()) {
        QList<QContactManager::Error> errors(QList<QContactManager::Error>()<<QContactManager::BadArgumentError);
        parent->updateRequest(req, contacts, QContactManager::BadArgumentError, errors, QContactAbstractRequest::Finished);
        return;
    }
    QList<QContactManager::Error> dummy;
    parent->updateRequestStatus(req, QContactManager::NoError, dummy,
            QContactAbstractRequest::Active);
    foreach(QContact contact, contacts)
    {
        saveContact(contact);
    }
}

void QTrackerContactSaveRequest::computeProgress()
{
    Q_ASSERT(req->type() == QContactAbstractRequest::ContactSaveRequest);
    QContactSaveRequest* r = static_cast<QContactSaveRequest*>(req);
    if( r->contacts().size() == contactsFinished.size() )
    {
        // compute master error - part of qtcontacts api
        QContactManager::Error error = QContactManager::NoError;
        foreach(QContactManager::Error err, errorsOfContactsFinished)
            if( QContactManager::NoError != err )
            {
                error = err;
                break;
            }
        QContactManagerEngine *engine = qobject_cast<QContactManagerEngine *> (parent());
        Q_ASSERT(engine);

        engine->updateRequest(req, contactsFinished, error, errorsOfContactsFinished, QContactAbstractRequest::Finished);
    }
}

void QTrackerContactSaveRequest::saveContact(QContact &contact)
{
    QContactManagerEngine *engine = qobject_cast<QContactManagerEngine *> (parent());
    Q_ASSERT(engine);
    QContactManager::Error error;
    // Ensure that the contact data is ok. This comes from QContactModelEngine
    if(!engine->validateContact(contact, error)) {
        contactsFinished << contact;
        errorsOfContactsFinished << error;
        computeProgress();
        return;
    }

    QTrackerContactsLive cLive;
    Live<nco::PersonContact> ncoContact;
    RDFServicePtr service = cLive.service();

    if(contact.localId() == 0) {
        // Save new contact. compute ID
        bool ok;
        QSettings definitions(QSettings::IniFormat, QSettings::UserScope, "Nokia", "Trackerplugin");
        // what if both processes read in the same time and write at the same time, no increment
        unsigned int m_lastUsedId = definitions.value("nextAvailableContactId", "1").toUInt(&ok);
        definitions.setValue("nextAvailableContactId", QString::number(++m_lastUsedId));

        ncoContact = service->liveNode(QUrl("contact:"+(QString::number(m_lastUsedId))));
        QContactId id; id.setLocalId(m_lastUsedId);
        contact.setId(id);
        ncoContact->setContactUID(QString::number(m_lastUsedId));
        ncoContact->setContentCreated(QDateTime::currentDateTime());
    }  else {
        ncoContact = service->liveNode(QUrl("contact:"+QString::number(contact.localId())));
        //  disabled because of 141727 - it breaks the transaction
        //  ncoContact->setContentLastModified(QDateTime::currentDateTime());
    }

    // if there are work related details, need to be saved to Affiliation.
    if( QTrackerContactSaveRequest::contactHasWorkRelatedDetails(contact))
        addAffiliation(service, contact.localId());

    // Add a special tag for contact added from addressbook, not from fb, telepathy etc.
    // this is important when returning contacts to sync team
    RDFVariable rdfContact = RDFVariable::fromType<nco::PersonContact>();
    rdfContact.property<nco::contactUID>() = LiteralValue(QString::number(contact.localId()));
    addTag(service, rdfContact, "addressbook");

    saveContactDetails( service, ncoContact, contact);

    // name & nickname - different way from other details
    cLive.setLiveContact(ncoContact);
    cLive.setQContact(contact);
    if( !contact.detail<QContactName>().isEmpty() || !contact.detail<QContactNickname>().isEmpty() )
        cLive.saveName();

    // remember to commit the transaction, otherwise all changes will be rolled back.
    cLive.commit();

    // TODO add async signal handling of for transaction's commitFinished
    contactsFinished << contact;
    errorsOfContactsFinished << QContactManager::NoError; // TODO ask how to get error code from tracker
    computeProgress();
}


QTrackerContactSaveRequest::~QTrackerContactSaveRequest()
{
    // TODO Auto-generated destructor stub
}

/*!
* Saving has to go in such way that all names are saved at once, all phone numbers together
* filled to rdfupdate query etc.
* This method goes through the contact and collect which contact detail definitions are there
*/
QStringList QTrackerContactSaveRequest::detailsDefinitionsInContact(const QContact &c)
{
    QStringList definitions;
    foreach(const QContactDetail& det, c.details())
        {
            definitions << det.definitionName();
        }
    definitions.removeDuplicates();
    return definitions;
}

//! Just moving this code out of saveContact to make it shorter
bool QTrackerContactSaveRequest::contactHasWorkRelatedDetails(const QContact &c)
{
    foreach(const QContactDetail& det, c.details())
    {
        if( det.contexts().contains(QContactDetail::ContextWork))
           return true;
    }
    return false;
}

// create nco::Affiliation if there is not one already in tracker
void QTrackerContactSaveRequest::addAffiliation(RDFServicePtr service, QContactLocalId contactId)
{
    RDFVariable contact = RDFVariable::fromType<nco::PersonContact>();
    contact.property<nco::contactUID> () = LiteralValue(QString::number(contactId));
    RDFVariable contact1 = contact.deepCopy();
    RDFUpdate up;

    // here we will specify to add new node for affiliation if it doesnt exist already
    RDFVariable affiliation = contact.optional().property<nco::hasAffiliation> ();
    RDFFilter doesntExist = affiliation.isBound().not_();// do not create if it already exist
    QUrl newAffiliation = ::tracker()->createLiveNode().uri();
    QList<RDFVariableStatement> insertions;
    insertions << RDFVariableStatement(contact, nco::hasAffiliation::iri(), newAffiliation)
    << RDFVariableStatement(newAffiliation, rdf::type::iri(), nco::Affiliation::iri());

    // this means that filter applies to both insertions
    up.addInsertion(insertions);
    service->executeQuery(up);
}

void QTrackerContactSaveRequest::saveContactDetails( RDFServicePtr service,
                                                Live<nco::PersonContact>& ncoContact,
                                                const QContact& contact)
{
    QStringList detailDefinitionsToSave = detailsDefinitionsInContact(contact);

    // all the rest might need to save to PersonContact and to Affiliation contact
    RDFVariable rdfPerson = RDFVariable::fromType<nco::PersonContact>();
    rdfPerson.property<nco::contactUID>() = LiteralValue(QString::number(contact.localId()));

    // Delete all existing phone numbers - office and home
    deletePhoneNumbers(service, rdfPerson);

    foreach(QString definition, detailDefinitionsToSave)
    {
        QList<QContactDetail> details = contact.details(definition);
        Q_ASSERT(!details.isEmpty());

        RDFVariable rdfAffiliation;
        RDFVariable rdfPerson1;
        rdfPerson1.property<nco::hasAffiliation>() = rdfAffiliation;
        rdfPerson1.property<nco::contactUID>() = LiteralValue(QString::number(contact.localId()));

        QList<QContactDetail> workDetails;
        QList<QContactDetail> homeDetails;
        foreach(const QContactDetail& det, details) {
            if( det.contexts().contains(QContactDetail::ContextWork) ) {
                workDetails << det;
            } else {
                homeDetails << det;
            }
        }

        /* Save details */
        if(definition == QContactPhoneNumber::DefinitionName) {
            if (!homeDetails.isEmpty()) {
                savePhoneNumbers(service, rdfPerson, homeDetails);
            }
            if( !workDetails.isEmpty()) {
                savePhoneNumbers(service, rdfAffiliation, workDetails);
            }
        }
        else if(definition == QContactEmailAddress::DefinitionName) {
            if (!homeDetails.isEmpty())
                saveEmails(service, rdfPerson, homeDetails);
            if( !workDetails.isEmpty())
                saveEmails(service, rdfAffiliation, workDetails);
        }
        else if(definition == QContactAddress::DefinitionName) {
            if (!homeDetails.isEmpty())
                saveAddresses(service, rdfPerson, homeDetails);
            if( !workDetails.isEmpty())
                saveAddresses(service, rdfAffiliation, workDetails);
        }
        else if(definition == QContactUrl::DefinitionName) {
            if (!homeDetails.isEmpty())
                saveUrls(service, rdfPerson, homeDetails);
            if( !workDetails.isEmpty())
                saveUrls(service, rdfAffiliation, workDetails);
        }
        else {
            // TODO refactor (bug: editing photo doesn't work)
            foreach(const QContactDetail &det, details )
            {
                definition = det.definitionName();
                if(definition == QContactAvatar::DefinitionName) {
                    QUrl avatar = det.value(QContactAvatar::FieldAvatar);
                    Live<nie::DataObject> fdo = service->liveNode( avatar );
                    ncoContact->setPhoto(fdo);
                }
/*                else if (definition == QContactOnlineAccount::DefinitionName){
                    // TODO parse URI, once it is defined
                    QString account = det.value("Account");
                    QString serviceName = det.value("ServiceName");
                    // TODO refactor  - remove blocking call in next line
                    Live<nco::IMAccount> liveIMAccount = ncoContact->firstHasIMAccount();
                    if (0 == liveIMAccount)
                    {
                        liveIMAccount = ncoContact->addHasIMAccount();
                    }
                    liveIMAccount->setImID(account);
                    liveIMAccount->setImAccountType(serviceName);
                }
*/
            } // end foreach detail
        }
    }
}

// Delete all existing phone numbers from the contact so that edits are
// reflected to Tracker correctly.
void QTrackerContactSaveRequest::deletePhoneNumbers(RDFServicePtr service, const RDFVariable& rdfContactIn) {
    RDFUpdate up;
    RDFVariable rdfContact = rdfContactIn.deepCopy();
    RDFVariable rdfContact2 = rdfContactIn.deepCopy();

    // Get the associations from the contact object.
    RDFVariable rdfAffiliations = rdfContact.property<nco::hasAffiliation>();
    RDFVariable rdfPhonesHome = rdfContact2.property<nco::hasPhoneNumber>();
    RDFVariable rdfPhonesWork = rdfAffiliations.property<nco::hasPhoneNumber>();

    // Delete the references to phone numbers and the numbers themselves.
    up.addDeletion(rdfContact, nco::hasPhoneNumber::iri(), rdfPhonesHome);
    up.addDeletion(rdfPhonesHome, rdf::type::iri(), rdfs::Resource::iri());

    up.addDeletion(rdfAffiliations, nco::hasPhoneNumber::iri(), rdfPhonesWork);
    up.addDeletion(rdfPhonesWork, rdf::type::iri(), rdfs::Resource::iri());

    service->executeQuery(up);
}

/*!
 * write all phone numbers on one query to tracker
 * TODO this is temporary code for creating new, saving contacts need to handle only what was
 * changed.
 */
void QTrackerContactSaveRequest::savePhoneNumbers(RDFServicePtr service, RDFVariable &var, const QList<QContactDetail> &details )
{
    RDFUpdate up;
    RDFVariable varForInsert = var.deepCopy();
    foreach(const QContactDetail& det, details)
    {
        QUrl newPhone = ::tracker()->createLiveNode().uri();
        up.addInsertion(varForInsert, nco::hasPhoneNumber::iri(), newPhone);
        QStringList subtypes = det.value<QStringList>(QContactPhoneNumber::FieldSubTypes);

        if( subtypes.contains(QContactPhoneNumber::SubTypeMobile))
            up.addInsertion(newPhone, rdf::type::iri(), nco::CellPhoneNumber::iri());
        else if( subtypes.contains(QContactPhoneNumber::SubTypeCar))
            up.addInsertion(newPhone, rdf::type::iri(), nco::CarPhoneNumber::iri());
        else if( subtypes.contains(QContactPhoneNumber::SubTypeBulletinBoardSystem))
            up.addInsertion(newPhone, rdf::type::iri(), nco::BbsNumber::iri());
        else if( subtypes.contains(QContactPhoneNumber::SubTypeFacsimile))
            up.addInsertion(newPhone, rdf::type::iri(), nco::FaxNumber::iri());
        else if( subtypes.contains(QContactPhoneNumber::SubTypeModem))
            up.addInsertion(newPhone, rdf::type::iri(), nco::ModemNumber::iri());
        else if( subtypes.contains(QContactPhoneNumber::SubTypePager))
            up.addInsertion(newPhone, rdf::type::iri(), nco::PagerNumber::iri());
        else if( subtypes.contains(QContactPhoneNumber::SubTypeMessagingCapable))
            up.addInsertion(newPhone, rdf::type::iri(), nco::MessagingNumber::iri());
        else
            up.addInsertion(newPhone, rdf::type::iri(), nco::VoicePhoneNumber::iri());

        up.addInsertion(newPhone, nco::phoneNumber::iri(), LiteralValue(det.value(QContactPhoneNumber::FieldNumber)));
    }
    service->executeQuery(up);
}

/*!
 * write all phone numbers on one query to tracker
 * TODO this is temporary code for creating new, saving contacts need to handle only what was
 * changed.
 */
void QTrackerContactSaveRequest::saveEmails(RDFServicePtr service, RDFVariable &var, const QList<QContactDetail> &details )
{
    RDFUpdate up;
    RDFVariable varForInsert = var.deepCopy();
    RDFVariable emails = var.property<nco::hasEmailAddress>();
    RDFVariable types = emails.property<rdf::type>();
    up.addDeletion(RDFVariableStatement(var, nco::hasEmailAddress::iri(), emails));
    up.addDeletion(emails, rdf::type::iri(), types);
    foreach(const QContactDetail& det, details)
    {
        QUrl newEmail = ::tracker()->createLiveNode().uri();
        up.addInsertion(newEmail, rdf::type::iri(), nco::EmailAddress::iri());
        up.addInsertion(newEmail, nco::emailAddress::iri(), LiteralValue(det.value(QContactEmailAddress::FieldEmailAddress)));
        up.addInsertion(RDFVariableStatement(varForInsert, nco::hasEmailAddress::iri(), newEmail));
    }
    service->executeQuery(up);
}

/*!
 * write all Urls
 * TODO this is temporary code for creating new, saving contacts need to handle only what was
 * changed.
 */
void QTrackerContactSaveRequest::saveUrls(RDFServicePtr service, RDFVariable &rdfContact, const QList<QContactDetail> &details )
{
    RDFUpdate up;
    RDFVariable varForInsert = rdfContact.deepCopy();
    RDFVariable urls = rdfContact.property<nco::url>();
    RDFVariable urltypes = urls.property<rdf::type>();

    RDFVariable websiteUrls = rdfContact.property<nco::websiteUrl>();
    RDFVariable websiteUrlTypes = websiteUrls.property<rdf::type>();

    up.addDeletion(RDFVariableStatement(rdfContact, nco::url::iri(), urls));
    up.addDeletion(RDFVariableStatement(rdfContact, nco::websiteUrl::iri(), websiteUrls));
    // first part - deleting previous before adding new again is to be removed

    // second part, write all urls
    foreach(const QContactDetail& det, details)
    {
        QUrl newUrl(det.value(QContactUrl::FieldUrl));//::tracker()->createLiveNode().uri();
        if(det.value(QContactUrl::FieldSubType) == QContactUrl::SubTypeFavourite)
        {
            up.addInsertion(varForInsert, nco::url::iri(), newUrl);
        }
        else // if not favourite, then homepage. don't support other
        {
            up.addInsertion(varForInsert, nco::websiteUrl::iri(), newUrl); // add it to contact
        }
    }
    service->executeQuery(up);
}

/*!
 * write all phone numbers on one query to tracker
 * TODO this is temporary code for creating new, saving contacts need to handle only what was
 * changed.
 */
void QTrackerContactSaveRequest::saveAddresses(RDFServicePtr service, RDFVariable &var, const QList<QContactDetail> &details )
{
    RDFUpdate up;
    RDFVariable varForInsert = var.deepCopy();
    RDFVariable addresses = var.property<nco::hasPostalAddress>();
    RDFVariable types = addresses.property<rdf::type>();
    up.addDeletion(RDFVariableStatement(var, nco::hasPostalAddress::iri(), addresses));
    up.addDeletion(addresses, rdf::type::iri(), types);
    foreach(const QContactDetail& det, details)
    {
        QUrl newPostalAddress = ::tracker()->createLiveNode().uri();
        // TODO     nco:DomesticDeliveryAddress, nco:InternationalDeliveryAddress, nco:ParcelDeliveryAddress
        up.addInsertion(newPostalAddress, rdf::type::iri(), nco::PostalAddress::iri());
        if( det.hasValue(QContactAddress::FieldStreet))
            up.addInsertion(newPostalAddress, nco::streetAddress::iri(), LiteralValue(det.value(QContactAddress::FieldStreet)));
        if( det.hasValue(QContactAddress::FieldLocality))
            up.addInsertion(newPostalAddress, nco::locality::iri(), LiteralValue(det.value(QContactAddress::FieldLocality)));
        if( det.hasValue(QContactAddress::FieldCountry))
            up.addInsertion(newPostalAddress, nco::country::iri(), LiteralValue(det.value(QContactAddress::FieldCountry)));
        if( det.hasValue(QContactAddress::FieldPostcode))
            up.addInsertion(newPostalAddress, nco::postalcode::iri(), LiteralValue(det.value(QContactAddress::FieldPostcode)));
        if( det.hasValue(QContactAddress::FieldRegion))
            up.addInsertion(newPostalAddress, nco::region::iri(), LiteralValue(det.value(QContactAddress::FieldRegion)));

        up.addInsertion(RDFVariableStatement(varForInsert, nco::hasPostalAddress::iri(), newPostalAddress));
    }
    service->executeQuery(up);
}

/*!
 * Not very good solution, but we add "addressbook" tag to identify which contacts
 * are added but addressbook ( in order to separate them from facebook and telepathy
 * contacts
 */
void QTrackerContactSaveRequest::createTagIfItDoesntExistAlready(SopranoLive::RDFServicePtr service, const QString &tag)
{
    static bool checked = false;
    // only once, if someone remove tag we are in problems (lost contacts)
    if( !checked )
    {
        checked = true;
        RDFVariable rdfTag = RDFVariable::fromType<nao::Tag>();
        RDFVariable labelVar = rdfTag.optional().property<nao::prefLabel>();
        labelVar = LiteralValue(tag);
        RDFFilter doesntExist = labelVar.isBound().not_();// do not create if it already exist

        RDFUpdate up;

        QUrl newTag = ::tracker()->createLiveNode().uri();
        rdfTag = newTag;
        QList<RDFVariableStatement> insertions;
        insertions << RDFVariableStatement(rdfTag, rdf::type::iri(), nao::Tag::iri())
        << RDFVariableStatement(newTag, nao::prefLabel::iri(), labelVar);
        up.addInsertion(insertions); // this way we apply filter doesntExist to both insertions
        service->executeQuery(up);
    }
}

void QTrackerContactSaveRequest::addTag(RDFServicePtr service, RDFVariable &var, const QString &tag)
{
    // TODO do all in one RDF query: create tag if not existing
    createTagIfItDoesntExistAlready(service, tag);
    RDFUpdate up;
    RDFVariable rdftag;
    rdftag.property<nao::prefLabel>() = LiteralValue(tag);
    up.addInsertion(var, nao::hasTag::iri(), rdftag);
    service->executeQuery(up);
}