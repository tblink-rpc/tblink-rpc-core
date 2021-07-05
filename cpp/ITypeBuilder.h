/*
 * ITypeBuilder.h
 *
 *  Created on: Jul 3, 2021
 *      Author: mballance
 */
#include <memory>

namespace tblink_rpc_core {

class ITypeBuilder;
typedef std::shared_ptr<ITypeBuilder> ITypeBuilderSP;
class ITypeBuilder {
public:

	virtual ~ITypeBuilder() { }


};

}
