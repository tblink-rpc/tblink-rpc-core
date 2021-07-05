/*
 * IType.h
 *
 *  Created on: Jul 3, 2021
 *      Author: mballance
 */

#include <memory>

namespace tblink_rpc_core {

class IType;
typedef std::shared_ptr<IType> ITypeSP;
class IType {
public:

	virtual ~IType() { }


};

}

