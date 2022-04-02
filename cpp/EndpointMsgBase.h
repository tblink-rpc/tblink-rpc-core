/*
 * Endpoint.h
 *
 *  Created on: Jul 2, 2021
 *      Author: mballance
 */

#pragma once
#include <map>
#include <unordered_map>
#include <stdint.h>

#include "EndpointBase.h"
#include "InterfaceInstMsgTransport.h"
#include "InterfaceType.h"
#include "tblink_rpc/IParamVal.h"
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IEndpointMsg.h"
#include "tblink_rpc/IEndpointListener.h"
#include "tblink_rpc/IEndpointServices.h"


namespace tblink_rpc_core {

class EndpointMsgBase : public EndpointBase, public virtual IEndpointMsg {
public:
	using response_f = std::function<void (intptr_t, IParamValMap *, IParamValMap *)>;
public:

	friend class InterfaceInstBase;

	EndpointMsgBase();

	virtual ~EndpointMsgBase();

	virtual int32_t init(IEndpointServices *services) override;

	virtual int32_t build_complete() override;

	virtual int32_t connect_complete() override;

	virtual bool shutdown() override;

	virtual void update_comm_mode(comm_mode_e m, comm_state_e s) override;

	virtual intptr_t add_time_callback(
			uint64_t						time,
			const std::function<void()>		&cb_f) override;

	virtual void cancel_callback(intptr_t	id) override;

	/** Called by the environment to notify that
	 *  a callback has occurred
	 */
	virtual void notify_callback(intptr_t   id) override;

	virtual IInterfaceInst *createInterfaceObj(
			IInterfaceType			*type,
			bool					is_mirror,
			IInterfaceImpl			*impl) override;

	virtual void destroyInterfaceObj(
			IInterfaceInst			*ifinst) override;

	virtual IInterfaceInst *defineInterfaceInst(
			IInterfaceType			*type,
			const std::string		&inst_name,
			bool					is_mirror,
			IInterfaceImpl			*impl) override;

	virtual intptr_t send_req(
			const std::string 	&method,
			IParamValMap 		*params);

	virtual intptr_t send_req(
			const std::string 				&method,
			IParamValMap 					*params,
			const response_f				&rsp_f);

	int32_t recv_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMap			*params);

	int32_t recv_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error);

	virtual int32_t send_req(
			const std::string		&method,
			intptr_t				id,
			IParamValMap			*params) = 0;

	virtual int32_t send_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error) = 0;

//private:
	// Methods used by InterfaceInst

	int32_t invoke(
			InterfaceInstBase	*ifinst,
			IMethodType 		*method,
			IParamValVec 		*params,
			const invoke_rsp_f	&rsp_f);

	void invoke_nb_rsp(
			intptr_t			msg_id,
			IParamValMap		*result,
			IParamValMap		*error);

protected:

	virtual InterfaceInstBase *mkInterfaceInst(
			IInterfaceType		*iftype,
			const std::string	&name,
			bool				is_mirror,
			IInterfaceImpl		*ifimpl) override;

private:
	typedef std::pair<IParamValMapUP,IParamValMapUP> rsp_t;
	typedef std::function<rsp_t(intptr_t, IParamValMap *)> req_func_t;
private:

	rsp_t req_init(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_build_complete(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_connect_complete(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_get_interface_types(
			intptr_t		id,
			IParamValMap	*params);

	rsp_t req_add_time_callback(
			intptr_t		id,
			IParamValMap	*params);

	rsp_t req_cancel_callback(
			intptr_t		id,
			IParamValMap	*params);

	rsp_t req_notify_callback(
			intptr_t		id,
			IParamValMap	*params);

	rsp_t req_create_ifc_obj(
			intptr_t		id,
			IParamValMap	*params);

	rsp_t req_shutdown(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_invoke_nb(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_invoke_b(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_invoke_rsp_b(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_run_until_event(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_set_comm_state(
			intptr_t		id,
			IParamValMap 	*params);

	rsp_t req_update_comm_mode(
			intptr_t		id,
			IParamValMap 	*params);

	void call_completion_nb(
			intptr_t		id,
			intptr_t		call_id,
			IParamVal		*retval);

	void call_completion_b(
			InterfaceInstBase	*ifinst,
			intptr_t			call_id,
			IParamVal			*retval);

	void rsp_call_completion_b(
			intptr_t			rsp_id,
			IParamValMap		*result,
			IParamValMap		*error);

	IParamValMap *pack_iftypes(const iftype_m_t &iftypes);

	IParamValMap *pack_ifinsts(const ifinst_m_t &ifinsts);

	IParamValMap *pack_type(IType *t);

	void unpack_iftypes(
			std::vector<IInterfaceType *>		&iftypes_l,
			IParamValMap 						*iftypes_p);

	void unpack_ifinsts(
			std::vector<IInterfaceInst*>		&ifinsts_l,
			IParamValMap  						*ifinsts_p);

private:
	typedef std::pair<bool, std::pair<IParamValMap*,IParamValMap*>> rspq_elem_t;

protected:

	intptr_t														m_id;
	intptr_t														m_call_id;
	std::unordered_map<intptr_t,invoke_rsp_f>						m_outbound_invoke_m;

	std::unordered_map<intptr_t, response_f>						m_rsp_m;

	std::map<intptr_t, std::function<void()>>						m_callback_m;
	std::set<intptr_t>												m_pending_time_cb;
	intptr_t														m_callback_id;
	std::map<std::string,req_func_t>								m_req_m;

	int32_t															m_release_reqs;
	int32_t															m_wait_reqs;

};

} /* namespace tblink_rpc_core */

