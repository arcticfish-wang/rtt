/***************************************************************************
  tag: Peter Soetens  Mon Jun 26 13:25:58 CEST 2006  CorbaCommandFactory.hpp 

                        CorbaCommandFactory.hpp -  description
                           -------------------
    begin                : Mon June 26 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@fmtc.be
 
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
 
 
#ifndef ORO_CORBACOMMANDFACTORY_HPP
#define ORO_CORBACOMMANDFACTORY_HPP

#include "../OperationFactory.hpp"
#include "../ConditionBoolDataSource.hpp"
#include "CommandProxy.hpp"
#include "OperationInterfaceC.h"
#include "CommandProxy.hpp"

namespace RTT
{namespace Corba
{

    /**
     * This class mirrors a Corba Command Factory as a plain C++
     * factory.
     */
    class CorbaCommandFactory
        : public RTT::detail::OperationFactoryPart<DispatchInterface*>
    {
        std::string com;
        Corba::CommandInterface_var mfact;
        PortableServer::POA_var mpoa;
    public:
        typedef std::vector< RTT::ArgumentDescription > Descriptions;
        typedef std::vector<std::string> Commands;
        typedef std::vector<DataSourceBase::shared_ptr> Arguments;

        CorbaCommandFactory(const std::string& command, Corba::CommandInterface_ptr fact, PortableServer::POA_ptr the_poa) 
            : RTT::detail::OperationFactoryPart<DispatchInterface*>("Corba Command"),
              com(command), mfact( Corba::CommandInterface::_duplicate(fact) ),
              mpoa(PortableServer::POA::_duplicate(the_poa) )
        {}

        virtual ~CorbaCommandFactory() {}

        virtual int arity()  const {
            return this->getArgumentList().size();
        }

        virtual std::string resultType() const 
        {
            return "bool";
        }

        virtual std::string description() const 
        {
            try {
                CORBA::String_var result = mfact->getDescription( com.c_str() );
                return std::string( result.in() );
            } catch ( Corba::NoSuchNameException& nsn ) {
                throw name_not_found_exception( nsn.name.in() );
            }
            return std::string();
        }

        virtual PropertyBag
        getArgumentSpec() const {
            return PropertyBag();
        }

        /**
         * @brief Return the list of arguments of a certain command.
         */
        virtual Descriptions getArgumentList() const
        {
            Descriptions ret;
            try {
                Corba::Descriptions_var result = mfact->getArguments( com.c_str() );
                ret.reserve( result->length() );
                for (size_t i=0; i!= result->length(); ++i)
                    ret.push_back( RTT::ArgumentDescription(std::string( result[i].name.in() ),
                                                       std::string( result[i].description.in() ),
                                                       std::string( result[i].type.in() ) ));
            } catch ( Corba::NoSuchNameException& nsn ) {
                throw name_not_found_exception( nsn.name.in() );
            }
            return ret;
        }

        virtual DispatchInterface* produce(const Arguments& args) const 
        {
            Corba::Arguments_var nargs = new Corba::Arguments();
            nargs->length( args.size() );
            for (size_t i=0; i < args.size(); ++i )
                nargs[i] = args[i]->server( mpoa.in() );
            try {
                Corba::Command_var result = mfact->createCommand( com.c_str(), nargs.in() );
                // return a DispatchInterface object:
                return CommandProxy::Create( result.in() );
            } catch ( Corba::NoSuchNameException& nsn ) {
                throw name_not_found_exception( nsn.name.in() );
            } catch ( Corba::WrongNumbArgException& wa ) {
                throw wrong_number_of_args_exception( wa.wanted, wa.received );
            } catch ( Corba::WrongTypeArgException& wta ) {
                throw wrong_types_of_args_exception( wta.whicharg, wta.expected.in(), wta.received.in() );
            }
            return 0; // not reached.
        }
    };
}}

#endif