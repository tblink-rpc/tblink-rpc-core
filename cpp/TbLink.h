/*
 * Factory.h
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#pragma once
#include <memory>

#include "tblink_rpc/ITbLink.h"
#include "tblink_rpc/ILaunchType.h"
#include "DynSymFinder.h"

namespace tblink_rpc_core {

class TbLink : public ITbLink {
public:
	TbLink();

	virtual ~TbLink();

	virtual IEndpoint *mkJsonRpcEndpoint(
			IEndpoint::Type		type,
			IEndpointServices	*services) override;

	virtual ITransport *mkSocketTransport(
			pid_t		pid,
			int32_t 	socket) override;

	virtual const std::vector<ILaunchType *> &launchTypes() const override {
		return m_launch_types;
	}

	virtual void addLaunchType(ILaunchType *launch_t) override;

	virtual ILaunchType *findLaunchType(const std::string &id) override;

	virtual ILaunchParams *newLaunchParams() override;

	virtual const std::string &getLibPath() override;

	virtual ISymFinder *sym_finder() override;

	static TbLink *inst();

private:
	std::map<std::string,ILaunchType *>		m_launch_type_m;
	std::vector<ILaunchType *>				m_launch_types;
	std::string								m_libpath;
	DynSymFinder							m_sym_finder;
	static TbLink							*m_inst;
};

} /* namespace tblink_rpc_core */

