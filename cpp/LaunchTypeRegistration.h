/*
 * LaunchTypeRegistration.h
 *
 *  Created on: Sep 19, 2021
 *      Author: mballance
 */

#pragma once
#include "Factory.h"

namespace tblink_rpc_core {

template <class T> class LaunchTypeRegistration {
public:
	LaunchTypeRegistration() {
		Factory::inst()->addLaunchType(&m_launch_type);
	}

private:
	static T						m_launch_type;
};

template <class T> T LaunchTypeRegistration<T>::m_launch_type;

}
