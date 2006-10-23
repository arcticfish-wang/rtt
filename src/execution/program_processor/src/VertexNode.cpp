/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  VertexNode.cxx

                        VertexNode.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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
#include "rtt/VertexNode.hpp"
#include <rtt/CommandNOP.hpp>
#include <rtt/CommandInterface.hpp>
#include <rtt/DataSource.hpp>

namespace RTT
{
    

	VertexNode::VertexNode()
        : command( new CommandNOP ),
          lineNumber( 0 )
	{
	}

        VertexNode::VertexNode( const VertexNode& orig )
                : command( orig.getCommand()->clone() ),
                  lineNumber(orig.getLineNumber())
        {
        }

        VertexNode::VertexNode(CommandInterface* cmd)
        : command( cmd ),
          lineNumber( 0 )
	{
	}

	VertexNode::~VertexNode()
	{
		delete command;
	}


	bool VertexNode::execute()
	{
		/**
         * Execute the command in any case.
         * If the command is Asynch, this will do nothing.
         */
		return command->execute();
	}

    bool VertexNode::isValid() const {
        return command->valid();
    }

	CommandInterface* VertexNode::getCommand() const
	{
		return command;
	}

	CommandInterface* VertexNode::setCommand(CommandInterface* cmd)
	{
        CommandInterface* old = command;
		command = cmd;
        return old;
	}

	void VertexNode::setLineNumber(int ln)
	{
		lineNumber = ln ;
	}

	int VertexNode::getLineNumber() const
	{
		return lineNumber;
	}

    void VertexNode::startExecution()
    {
        // reset the command
        command->reset();
        command->readArguments();
    }

    VertexNode VertexNode::copy( std::map<const DataSourceBase*, DataSourceBase*>& rdss ) const
    {
        VertexNode ret( *this );
        delete ret.setCommand( getCommand()->copy( rdss ) );
        return ret;
    }

    VertexNode& VertexNode::operator=( const VertexNode& orig )
    {
        delete command;
        command = orig.getCommand()->clone();
        lineNumber = orig.getLineNumber();
        return *this;
    }
}