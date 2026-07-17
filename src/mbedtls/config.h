/**
 * \file config.h
 *
 * \brief Tpad's minimal Mbed TLS configuration.
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
 * This file is intentionally modified for Tpad and contains only the modules
 * that Tpad directly invokes. Tpad's additions are Copyright (C) 2014-2026
 * Andrew Smith (GNA SERVICES INC) <Andrew@GNAServicesInc.com> and are
 * distributed under Apache-2.0. See LICENSE in this directory.
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#define MBEDTLS_BASE64_C
#define MBEDTLS_MD5_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA512_C

#include "check_config.h"

#endif /* MBEDTLS_CONFIG_H */
