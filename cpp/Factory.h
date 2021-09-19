/*
 * Factory.h
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#pragma once
#include <memory>
#include "tblink_rpc/IFactory.h"
#include "tblink_rpc/ILaunchType.h"

namespace tblink_rpc_core {

class Factory : public IFactory {
public:
	Factory();

	virtual ~Factory();

	virtual IEndpoint *mkJsonRpcEndpoint(
			IEndpoint::Type		type,
			IEndpointServices	*services) override;

	virtual ITransport *mkSocketTransport(
			pid_t		pid,
			int32_t 	socket) override;

	const std::vector<ILaunchType *> &launchTypes() const {
		return m_launch_types;
	}

	void addLaunchType(ILaunchType *launch_t);

	ILaunchType *findLaunchType(const std::string &id);

	static Factory *inst();

private:
	std::map<std::string,ILaunchType *>		m_launch_type_m;
	std::vector<ILaunchType *>				m_launch_types;
	static std::unique_ptr<Factory>			m_inst;
};

} /* namespace tblink_rpc_core */

