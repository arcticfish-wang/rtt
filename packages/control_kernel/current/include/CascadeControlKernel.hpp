/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  CascadeControlKernel.hpp 

                        CascadeControlKernel.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
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
 
#ifndef CASCADE_CONTROL_KERNEL_HPP
#define CASCADE_CONTROL_KERNEL_HPP

#include "NextGenKernel.hpp"
#include "DataServer.hpp"

namespace ORO_ControlKernel
{

    using ORO_CoreLib::NameServer;
    using ORO_CoreLib::NameServerRegistrator;

    /**
     * @brief The CascadeControlKernel is a ControlKernel where
     * its Outputs are introduced in the Commands namespace of
     * all kernels with the same prefix.
     *
     * It must be used in conjunction with a LockFreeControlKernel.
     *
     * @see LockFreeControlKernel
     * @see StandardControlKernel
     *
     */
    template < class _Extension = KernelBaseFunction >
    class CascadeControlKernel
        : public detail::BaseKernel< detail::DataObjectLockFreeC ,
                                     detail::DataObjectLockFreeC ,
                                     detail::DataObjectLockFreeC ,
                                     detail::DataObjectLockFreeC ,
                                     detail::DataObjectLockFreeC , 
                                     _Extension >
    {
        typedef detail::BaseKernel< detail::DataObjectLockFreeC ,
                                    detail::DataObjectLockFreeC ,
                                    detail::DataObjectLockFreeC ,
                                    detail::DataObjectLockFreeC ,
                                    detail::DataObjectLockFreeC , 
                                    _Extension > Base;
    public:
        /**
         * @brief Create a nameserved control kernel.
         *
         * @param name The name of this kernel. 
         * @param prefix The prefix to use for the DataObject names. Set prefix
         * to the name of another kernel, to be able to access its data objects.
         */
        CascadeControlKernel(const std::string& name, const std::string& prefix)
            : Base(name, prefix + "::Inputs", prefix + "::Models", prefix + "::Commands",
                   prefix + "::SetPoints", prefix + "::Commands")
        {
        }

    protected:

        virtual void updateComponents()
        {
            this->sensor->update();
            this->estimator->update();
            this->generator->update();
            this->controller->update();
            this->effector->update();
        }
    };

}

#endif
