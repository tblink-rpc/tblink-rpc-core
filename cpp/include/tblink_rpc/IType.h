/*
 * IType.h
 *
 *  Created on: Sep 14, 2021
 *      Author: mballance
 */

#pragma once
#include <functional>
#include <memory>
#include <stdint.h>

namespace tblink_rpc_core {

enum class TypeE {
	Bool,
	Int,
	Map,
	Str,
	Vec
};


class IType;
typedef std::unique_ptr<IType> ITypeUP;
class IType {
public:

	virtual ~IType() { }

	virtual TypeE kind() const = 0;

};


}

namespace std {
template <> struct hash<tblink_rpc_core::TypeE> {
    size_t operator()(tblink_rpc_core::TypeE x) const {
        return std::hash<int>()(static_cast<int>(x));
    }
};
}
