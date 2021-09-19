/****************************************************************************
 * tblink_rpc.h
 ****************************************************************************/
#pragma once

#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IEndpointServices.h"
#include "tblink_rpc/IFactory.h"
#include "tblink_rpc/IInterfaceInst.h"

extern "C" {
tblink_rpc_core::IFactory *tblink_rpc_get_factory();
}
