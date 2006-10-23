/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:40 CET 2006  StateMachineTask.cxx 

                        StateMachineTask.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 

#include "rtt/StateMachineTask.hpp"
#include <rtt/Attribute.hpp>
#include "rtt/TaskContext.hpp"
#include "rtt/FactoryExceptions.hpp"
#include "rtt/CommandDS.hpp"
#include "rtt/Method.hpp"

namespace RTT
{
    
    using namespace detail;

        void StateMachineTask::createCommandFactory() {
            // Add the state specific methods :
            // Special trick : we store the 'this' pointer in a DataSource, such that when
            // the created commands are copied, they also get the new this pointer.
            // This requires template specialisations on the TemplateFactory level.
            DataSource<StateMachineWPtr>* ptr = _this.get();

            commands()->addCommandDS(ptr, command_ds("activate",
                                               &StateMachine::activate, &StateMachine::isStrictlyActive, engine()->commands()),
                                    "Activate this StateMachine to initial state and enter request Mode.");
            commands()->addCommandDS(ptr, command_ds("deactivate",
                                               &StateMachine::deactivate, &StateMachine::isActive, engine()->commands(),true),
                                    "Deactivate this StateMachine");
            commands()->addCommandDS(ptr, command_ds("start",
                                               &StateMachine::automatic, &StateMachine::isAutomatic, engine()->commands()),
                                    "Start this StateMachine, enter automatic Mode.");
            commands()->addCommandDS(ptr, command_ds("automatic",
                                               &StateMachine::automatic, &StateMachine::isAutomatic, engine()->commands()),
                                    "Start this StateMachine, enter automatic Mode.");
            commands()->addCommandDS(ptr, command_ds("pause",
                      &StateMachine::pause, &StateMachine::isPaused, engine()->commands()),
                                 "Pause this StateMachine, enter paused Mode.");
            commands()->addCommandDS(ptr, command_ds("step",
                      &StateMachine::step, &StateMachine::stepDone, engine()->commands()),
                                 "Step this StateMachine. When paused, step a single instruction or transition evaluation. \n"
                                 "When in reactive mode, evaluate transitions and go to a next state, or if none, run handle.");
            commands()->addCommandDS(ptr, command_ds("reset",
                      &StateMachine::reset, &StateMachine::inInitialState, engine()->commands()),
                                 "Reset this StateMachine to the initial state");
            commands()->addCommandDS(ptr, command_ds("stop",
                      &StateMachine::stop, &StateMachine::inFinalState, engine()->commands()),
                                 "Stop this StateMachine to the final state and enter request Mode.");
            commands()->addCommandDS(ptr, command_ds("reactive",
                      &StateMachine::reactive, &StateMachine::isStrictlyActive, engine()->commands()),
                                 "Enter reactive mode (see requestState() and step() ).\n Command is done if ready for requestState() or step() command.");
            commands()->addCommandDS(ptr, command_ds("requestState",
                      &StateMachine::requestState, &StateMachine::inStrictState, engine()->commands()),
                                 "Request to go to a particular state. Will succeed if there exists a valid transition from this state to the requested state.",
                                 "State", "The state to make the transition to.");
        }

    void StateMachineTask::createMethodFactory()
    {
            DataSource<StateMachineWPtr>* ptr =  _this.get();
            methods()->addMethodDS(ptr, method_ds("inState", &StateMachine::inState), "Is the StateMachine in a given state ?", "State", "State Name");
            methods()->addMethodDS(ptr, method_ds("inError", &StateMachine::inError), "Is this StateMachine in error ?");
            methods()->addMethodDS(ptr, method_ds("getState", &StateMachine::getCurrentStateName), "The name of the current state. An empty string if not active.");
            methods()->addMethodDS(ptr, method_ds("isActive", &StateMachine::isActive), "Is this StateMachine activated (possibly in transition) ?");
            methods()->addMethodDS(ptr, method_ds("isRunning", &StateMachine::isAutomatic), "Is this StateMachine running in automatic mode ?");
            methods()->addMethodDS(ptr, method_ds("isReactive", &StateMachine::isReactive), "Is this StateMachine ready and waiting for requests or events ?");
            methods()->addMethodDS(ptr, method_ds("isPaused", &StateMachine::isPaused), "Is this StateMachine paused ?");
            methods()->addMethodDS(ptr, method_ds("inInitialState", &StateMachine::inInitialState), "Is this StateMachine in the initial state ?");
            methods()->addMethodDS(ptr, method_ds("inFinalState", &StateMachine::inFinalState), "Is this StateMachine in the final state ?");
            methods()->addMethodDS(ptr, method_ds("inTransition", &StateMachine::inTransition), "Is this StateMachine executing a entry|handle|exit program ?");
        }

        StateMachineTask* StateMachineTask::copy(StateMachinePtr newsc, std::map<const DataSourceBase*, DataSourceBase*>& replacements, bool instantiate )
        {
            // if this gets copied, all created commands will use the new instance of StateMachineTask to
            // call the member functions. Further more, all future commands for the copy will also call the new instance
            // while future commands for the original will still call the original. 
            StateMachineTask* tmp = new StateMachineTask( newsc, this->engine() );
            replacements[ _this.get() ] = tmp->_this.get(); // put 'newsc' in map

            AttributeRepository* dummy = this->attributes()->copy( replacements, instantiate );
            *(tmp->attributes()) = *dummy;
            delete dummy;

            return tmp;
        }

        StateMachineTask::StateMachineTask(StateMachinePtr statemachine, ExecutionEngine* ee)
            : TaskContext( statemachine->getName(), ee ),
              _this( new ValueDataSource<StateMachineWPtr>( statemachine ) ) // was: VariableDataSource.
        {
            this->clear();
            this->createCommandFactory();
            this->createMethodFactory();
        }

    StateMachineTask::~StateMachineTask()
    {
    }
}
