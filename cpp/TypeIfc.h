/*
 * TypeIfc.h
 *
 *  Created on: Feb 6, 2022
 *      Author: mballance
 */

#pragma once
#include <string>
#include "tblink_rpc/ITypeIfc.h"

namespace tblink_rpc_core {

class TypeIfc : public ITypeIfc {
public:
	TypeIfc(
			const std::string		&name,
			bool					is_mirror);

	virtual ~TypeIfc();

	virtual TypeE kind() const override { return TypeE::Ifc; }

	virtual const std::string &name() const { return m_name; }

	virtual bool is_mirror() const { return m_is_mirror; }

private:
	std::string				m_name;
	bool					m_is_mirror;
};

} /* namespace tblink_rpc_core */

