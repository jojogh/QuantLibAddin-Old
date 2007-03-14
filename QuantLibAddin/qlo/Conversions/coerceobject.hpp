
/*
 Copyright (C) 2006, 2007 Eric Ehlers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#ifndef qlo_conversions_coerceobject_hpp
#define qlo_conversions_coerceobject_hpp

#include <oh/Conversions/coerce.hpp>
#include <oh/exception.hpp>
#include <qlo/handle.hpp>

//NANDO: why in the ObjectHandler namespace?

// It should not be.  The correct approach would be to divide the Conversion/Coercion
// functions across the ObjectHandler, QuantLibAddin, and (not yet created) QuantLibXL
// namespaces, but for the sake of expediency all of this was put in the ObjectHandler
// namespace initially.

namespace ObjHandler {

    // Accept an id of an Object in the Repository and return a boost::shared_ptr<LibraryClass>.
    // 1) If the Object is of class QuantLibAddin::RelinkableHandle then return the contained boost::shared_ptr<LibraryClass>
    // 2) If the Object is of class QuantLibAddin::ObjectClass then return the contained boost::shared_ptr<LibraryClass>
    // 3) Otherwise the Object is of an unexpected class so raise an exception

    template <class ObjectClass, class LibraryClass>
    bool objectToReference(const boost::shared_ptr<ObjHandler::Object> &in,
                           boost::shared_ptr<LibraryClass> &out) {
        boost::shared_ptr<ObjectClass> qloPointer = 
            boost::dynamic_pointer_cast<ObjectClass>(in);
        if (qloPointer) {
            qloPointer->getLibraryObject(out);
            return true;
        } else {
            return false;
        }
    }

    template <class LibraryClass>
    bool handleToReference(const boost::shared_ptr<ObjHandler::Object> &in,
                           boost::shared_ptr<LibraryClass> &out) {
        boost::shared_ptr<QuantLibAddin::RelinkableHandle<LibraryClass> > handlePointer =
            boost::dynamic_pointer_cast<QuantLibAddin::RelinkableHandle<LibraryClass> >(in);

        if (!handlePointer)
            return false;

        out = handlePointer->getHandle().currentLink();
        OH_REQUIRE(out, "unable to retrieve reference contained in handle");

        return true;
    }

    template <class ObjectClass, class LibraryClass>
    class CoerceObject : public ObjHandler::Coerce<
            boost::shared_ptr<ObjHandler::Object>, 
            boost::shared_ptr<LibraryClass> > {
        typedef typename ObjHandler::Coerce<
            boost::shared_ptr<ObjHandler::Object>,
            boost::shared_ptr<LibraryClass> >::Conversion Conversion; 
        Conversion *getConversions() {
            static Conversion conversions[] = {
                objectToReference<ObjectClass, LibraryClass>,
                handleToReference<LibraryClass>,
                0
            };
            return conversions; 
        };
    };

}

#endif
