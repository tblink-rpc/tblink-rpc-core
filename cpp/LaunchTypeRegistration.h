/*
 * LaunchTypeRegistration.h
 *
 *  Created on: Sep 19, 2021
 *      Author: mballance
 */

#pragma once
#include "TbLink.h"

namespace tblink_rpc_core {

template <class T> class LaunchTypeRegistration {
public:
	LaunchTypeRegistration() {
		if (!m_launch_type) {
			m_launch_type = std::unique_ptr<T>(new T());
		}
		TbLink::inst()->addLaunchType(m_launch_type.get());
	}

private:
	static std::unique_ptr<T>				m_launch_type;
};

template <class T> std::unique_ptr<T> LaunchTypeRegistration<T>::m_launch_type;

}
