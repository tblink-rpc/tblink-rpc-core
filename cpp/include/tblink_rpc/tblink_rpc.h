/****************************************************************************
 * tblink_rpc.h
 ****************************************************************************/
#pragma once

#include "ITbLink.h"
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IEndpointServices.h"
#include "tblink_rpc/IInterfaceInst.h"

extern "C" {
tblink_rpc_core::ITbLink *tblink();
}
