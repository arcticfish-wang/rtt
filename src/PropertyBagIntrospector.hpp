/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:57 CEST 2004  PropertyBagIntrospector.hpp

                        PropertyBagIntrospector.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef PROPERTYBAGINTROSPECTOR
#define PROPERTYBAGINTROSPECTOR

#include "PropertyBag.hpp"
#include "Property.hpp"
#include "PropertyIntrospection.hpp"
#include <stack>

#ifdef ORO_PRAGMA_INTERFACE
#pragma interface
#endif

namespace RTT
{

    /**
     * @brief This class fills up a given bag with
     * the results of an introspection.
     */
    class PropertyBagIntrospector
        : public PropertyBagVisitor
    {
        std::stack<PropertyBag*> mystack;
    public:
        /**
         * @brief Create a new Introspector, saving the results in a bag.
         *
         * The \a bag will contain decomposed clones of the original.
         *
         * @param bag The bag to safe the results in.
         */
        PropertyBagIntrospector( PropertyBag& bag )
        {
            mystack.push( &bag );
        }

        virtual ~PropertyBagIntrospector()
        {
            mystack.pop();
        }

        /**
         * Reset to do a new introspection.
         * @param new_bag The new bag to save the results in.
         */
        void reset( PropertyBag& new_bag)
        {
            mystack.pop();
            mystack.push( &new_bag );
        }


        /**
         * Use this entry function to inspect a bag.
         * @param v the properties of this bag will be inspected.
         */
        void introspect(const PropertyBag& v )
        {
            v.identify(this);
        }

        virtual void introspect(PropertyBase* v)
        {
            // if it is decomposable, identify a new bag, otherwise add a clone.
            Property<PropertyBag> res(v->getName(), v->getDescription() );
            if ( v->getTypeInfo()->decomposeType( v->getDataSource(), res.value() ))
                res.identify( this );
            else
                mystack.top()->add( v->clone() );
            deleteProperties(res.value());
        }

        virtual void introspect(Property<PropertyBag> &v)
        {
            PropertyBag* cur_bag = mystack.top();
            Property<PropertyBag>* bag_cl
                = new Property<PropertyBag>( v.getName(),
                                             v.getDescription(),
                                             PropertyBag( v.get().getType() ) );
            cur_bag->add( bag_cl );

            mystack.push( &bag_cl->value() );

            v.value().identify(this);

            mystack.pop();
        }
    };
}

#endif