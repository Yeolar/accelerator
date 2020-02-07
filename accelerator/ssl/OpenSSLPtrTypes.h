/*
 * Copyright 2017 Facebook, Inc.
 * Copyright 2017-present Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <openssl/ssl.h>

#include "accelerator/Logging.h"
#include "accelerator/Memory.h"
#include "accelerator/ssl/OpenSSL.h"

namespace acc {

// ASN1
using ASN1TimeDeleter =
    acc::static_function_deleter<ASN1_TIME, &ASN1_TIME_free>;
using ASN1TimeUniquePtr = std::unique_ptr<ASN1_TIME, ASN1TimeDeleter>;

// X509
using X509Deleter = acc::static_function_deleter<X509, &X509_free>;
using X509UniquePtr = std::unique_ptr<X509, X509Deleter>;
using X509StoreCtxDeleter =
    acc::static_function_deleter<X509_STORE_CTX, &X509_STORE_CTX_free>;
using X509StoreCtxUniquePtr =
    std::unique_ptr<X509_STORE_CTX, X509StoreCtxDeleter>;
using X509VerifyParamDeleter =
    acc::static_function_deleter<X509_VERIFY_PARAM, &X509_VERIFY_PARAM_free>;
using X509VerifyParam =
    std::unique_ptr<X509_VERIFY_PARAM, X509VerifyParamDeleter>;

// EVP
using EvpPkeyDel = acc::static_function_deleter<EVP_PKEY, &EVP_PKEY_free>;
using EvpPkeyUniquePtr = std::unique_ptr<EVP_PKEY, EvpPkeyDel>;
using EvpPkeySharedPtr = std::shared_ptr<EVP_PKEY>;

// No EVP_PKEY_CTX <= 0.9.8b
#if OPENSSL_VERSION_NUMBER >= 0x10000002L
using EvpPkeyCtxDeleter =
    acc::static_function_deleter<EVP_PKEY_CTX, &EVP_PKEY_CTX_free>;
using EvpPkeyCtxUniquePtr = std::unique_ptr<EVP_PKEY_CTX, EvpPkeyCtxDeleter>;
#else
struct EVP_PKEY_CTX;
#endif

using EvpMdCtxDeleter =
    acc::static_function_deleter<EVP_MD_CTX, &EVP_MD_CTX_free>;
using EvpMdCtxUniquePtr = std::unique_ptr<EVP_MD_CTX, EvpMdCtxDeleter>;

// HMAC
using HmacCtxDeleter = acc::static_function_deleter<HMAC_CTX, &HMAC_CTX_free>;
using HmacCtxUniquePtr = std::unique_ptr<HMAC_CTX, HmacCtxDeleter>;

// BIO
using BioMethodDeleter =
    acc::static_function_deleter<BIO_METHOD, &BIO_meth_free>;
using BioMethodUniquePtr = std::unique_ptr<BIO_METHOD, BioMethodDeleter>;
using BioDeleter = acc::static_function_deleter<BIO, &BIO_vfree>;
using BioUniquePtr = std::unique_ptr<BIO, BioDeleter>;
using BioChainDeleter = acc::static_function_deleter<BIO, &BIO_free_all>;
using BioChainUniquePtr = std::unique_ptr<BIO, BioChainDeleter>;
inline void BIO_free_fb(BIO* bio) { ACCCHECK_EQ(1, BIO_free(bio)); }
using BioDeleterFb = acc::static_function_deleter<BIO, &BIO_free_fb>;
using BioUniquePtrFb = std::unique_ptr<BIO, BioDeleterFb>;

// RSA and EC
using RsaDeleter = acc::static_function_deleter<RSA, &RSA_free>;
using RsaUniquePtr = std::unique_ptr<RSA, RsaDeleter>;
#ifndef OPENSSL_NO_EC
using EcKeyDeleter = acc::static_function_deleter<EC_KEY, &EC_KEY_free>;
using EcKeyUniquePtr = std::unique_ptr<EC_KEY, EcKeyDeleter>;
using EcGroupDeleter = acc::static_function_deleter<EC_GROUP, &EC_GROUP_free>;
using EcGroupUniquePtr = std::unique_ptr<EC_GROUP, EcGroupDeleter>;
using EcPointDeleter = acc::static_function_deleter<EC_POINT, &EC_POINT_free>;
using EcPointUniquePtr = std::unique_ptr<EC_POINT, EcPointDeleter>;
using EcdsaSignDeleter =
    acc::static_function_deleter<ECDSA_SIG, &ECDSA_SIG_free>;
using EcdsaSigUniquePtr = std::unique_ptr<ECDSA_SIG, EcdsaSignDeleter>;
#endif

// BIGNUMs
using BIGNUMDeleter = acc::static_function_deleter<BIGNUM, &BN_clear_free>;
using BIGNUMUniquePtr = std::unique_ptr<BIGNUM, BIGNUMDeleter>;

// SSL and SSL_CTX
using SSLDeleter = acc::static_function_deleter<SSL, &SSL_free>;
using SSLUniquePtr = std::unique_ptr<SSL, SSLDeleter>;

} // namespace acc
