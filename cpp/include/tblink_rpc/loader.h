/*
 * loader.h
 *
 *  Created on: Sep 24, 2021
 *      Author: mballance
 */

#pragma once
#include <dlfcn.h>
#include "tblink_rpc/ITbLink.h"

static tblink_rpc_core::ITbLink *get_tblink(const char *path) {
	static tblink_rpc_core::ITbLink *prv_tblink = 0;

	if (!prv_tblink) {
		void *dlh = dlopen(path, RTLD_LAZY);

		if (!dlh) {
			fprintf(stdout, "TBLink Error: failed to load library %s: %s\n",
					path, dlerror());
		} else {
			void *tblink_f = dlsym(dlh, "tblink");

			if (!tblink_f) {
				fprintf(stdout, "TBLink Error: failed to find symbol tblink in library %s: %s\n",
						path, dlerror());
			} else {
				void *tblink_h = ((void *(*)())tblink_f)();
				prv_tblink = reinterpret_cast<tblink_rpc_core::ITbLink *>(tblink_h);
			}
		}
	}

	return prv_tblink;
}

