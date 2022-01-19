/*
 * TbLinkEvent.h
 *
 *  Created on: Jan 17, 2022
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/ITbLinkEvent.h"

namespace tblink_rpc_core {

class TbLinkEvent : public ITbLinkEvent {
public:
	TbLinkEvent(
			TbLinkEventKind		kind,
			void				*hndl);

	virtual ~TbLinkEvent();

	virtual TbLinkEventKind kind() override { return m_kind; }

	virtual void *hndl() override { return m_hndl; }

private:
	TbLinkEventKind				m_kind;
	void						*m_hndl;
};

} /* namespace tblink_rpc_core */

