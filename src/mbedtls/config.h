/**
 * \file config.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 */
/*
 *  Copyright (C) 2006-2018, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

/** 	BEGIN AUTO-COMPLY ACTION	&&&&&&&&&&&&


	**This file has been modified from the original.***

	88888888888888888888888888888888888888888888888888888888
				NOTICE
	88888888888888888888888888888888888888888888888888888888
	/**
 * \file config.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 */
/*
 *  Copyright (C) 2006-2018, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*

	Any part of this file not not covered by the above that is not part of the original file is part of tpad and is released under the same license as above ( SPDX-License-Identifier: Apache-2.0 [ You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 ]) and is Copyright © 2014-2018 Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com>
        All Rights Reserved. 
	88888888888888888888888888888888888888888888888888888888
				END NOTICE
	88888888888888888888888888888888888888888888888888888888
	########################################################
			FOR YOUR INFORMATION			
        ########################################################	 
	A copy of the original file and a diff of changes has been included in a file named config.h.orig.tar.xz
        ########################################################
/** 	END AUTO-COMPLY ACTION.	&&&&&&&&&&&&
*/

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#define MBEDTLS_HAVE_ASM
#define MBEDTLS_REMOVE_ARC4_CIPHERSUITES
#define MBEDTLS_FS_IO
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define MBEDTLS_ECP_DP_SECP192K1_ENABLED
#define MBEDTLS_ECP_DP_SECP224K1_ENABLED
#define MBEDTLS_ECP_DP_SECP256K1_ENABLED
#define MBEDTLS_ECP_DP_BP256R1_ENABLED
#define MBEDTLS_ECP_DP_BP384R1_ENABLED
#define MBEDTLS_ECP_DP_BP512R1_ENABLED
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED
#define MBEDTLS_ECP_DP_CURVE448_ENABLED
#define MBEDTLS_ERROR_STRERROR_DUMMY
#define MBEDTLS_THREADING_C
#define MBEDTLS_THREADING_PTHREAD
#define MBEDTLS_TIMING_C
#define MBEDTLS_AESNI_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_MD_C
#define MBEDTLS_MD5_C
#define MBEDTLS_RIPEMD160_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA512_C
#define MBEDTLS_VERSION_C
#if defined(TARGET_LIKE_MBED) && defined(YOTTA_CFG_MBEDTLS_TARGET_CONFIG_FILE)
#include YOTTA_CFG_MBEDTLS_TARGET_CONFIG_FILE
#endif

/*
 * Allow user to override any previous default.
 *
 * Use two macro names for that, as:
 * - with yotta the prefix YOTTA_CFG_ is forced
 * - without yotta is looks weird to have a YOTTA prefix.
 */
#if defined(YOTTA_CFG_MBEDTLS_USER_CONFIG_FILE)
#include YOTTA_CFG_MBEDTLS_USER_CONFIG_FILE
#elif defined(MBEDTLS_USER_CONFIG_FILE)
#include MBEDTLS_USER_CONFIG_FILE
#endif

#include "check_config.h"

#endif /* MBEDTLS_CONFIG_H */



