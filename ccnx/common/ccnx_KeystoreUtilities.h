/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * @file ccnx_KeystoreUtilities.h
 * @ingroup Signature
 * @brief A set of tools for working with the CCNx keystore.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#ifndef libccnx_ccnx_KeystoreUtilities_h
#define libccnx_ccnx_KeystoreUtilities_h

#include <parc/security/parc_Signer.h>

struct keystore_params;
/**
 * @typedef KeystoreParams
 * @brief Parameters for the KeyStore.
 */

typedef struct keystore_params KeystoreParams;

/**
 * Create a new `KeystoreParams` from a @p path, @p password, and a {@link PARCSigningInterface}.
 *
 * @param [in] impl A pointer to an instance of `PARCSigningInterface`.
 * @param [in] path The path to use.
 * @param [in] password The password to use.
 *
 * @return A pointer to a new instance of `KeystoreParams`.
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 *
 */
KeystoreParams *ccnxKeystoreUtilities_Create(PARCSigningInterface *interface, const char *path, const char *password);

/**
 * Destroy the `KeystoreParams`.
 *
 * @param [in,out] paramsPtr A pointer to the pointer to the `KeystoreParams` to destroy.
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 *
 */
void keystoreParams_Destroy(KeystoreParams **paramsPtr);

/**
 * Opens a PKCS12 keystore for use with CCNx, or creates it if missing.
 *
 *   All options may be NULL.
 *      keystoreFile     is the filename and path to use.  If the option is omitted, then the default location is used.
 *                       The default location is in ~/.ccnx/.ccnx_keystore.p12, which is a PKCS12 keystore.  For compatability
 *                       with older implementations, will also look for ~/.ccnx/.ccnx_keystore without the file extension.
 *      keystorePassword is the password to use.  If missing, will prompt with getpass(3).
 *
 *   This function uses the equivalent of getopt_long(3).  It does not change the argv.
 *
 * @param [in] keystoreFile The full path to the keystore, may be NULL to use ~/.ccnx/.ccnx_keystore.p12
 * @param [in] keystorePassword The keystore password, may be NULL for no password.
 * @return The `KeystoreParams`, including the path used, password used, and the `PARCSigner`, NULL if cannot be opened.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
KeystoreParams *ccnxKeystoreUtilities_OpenFile(const char *keystoreFile, const char *keystorePassword);

/**
 * Creates a PKCS12 keystore.
 *
 * @param [in] keystoreFile may be NULL to use the default location
 * @param [in] keystorePassword The keystore password, may be NULL for no password.
 * @param [in] keystoreBits The keystore bits, may be NULL for no password.
 * @param [in] keystoreDays The keystore days, may be NULL for no password.
 * @return The keystore parameters, including the path used, password used, and the `PARCSigner`, NULL if cannot be created.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
KeystoreParams *ccnxKeystoreUtilities_CreateFile(const char *keystoreFile, const char *keystorePassword, int keystoreBits, int keystoreDays);

/**
 * Returns an allocated buffer with password
 *
 *   Reads a password from stdin, then scrubs the static memory
 *
 * @return Free with {@link parcMemory_Deallocate()}
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
char *ccnxKeystoreUtilities_ReadPassword(void);

/**
 * Returns an allocated buffer with password
 *
 *   Reads a password from stdin, then scrubs the static memory.
 *   Confirms that it equals the provided password.
 *
 * @param [in] mustEqualPassword The password that must match.
 *
 * @return `true` if the password from stdin matches.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxKeystoreUtilities_ConfirmPassword(const char *mustEqualPassword);

/**
 * Get the file name from the given {@link KeystoreParams} instance.
 *
 * @param [in] params A pointer to a valid `KeystoreParams` instance.
 *
 * @return A pointer to a null-terminated C string containing the file name.
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 *
 */
const char *ccnxKeystoreUtilities_GetFileName(const KeystoreParams *params);

/**
 * Get the password from the given `KeyStoreParams` instance.
 *
 * @param [in] params A pointer to a valid `KeystoreParams` instance.
 *
 * @return A pointer to a null-terminated C string containing the password.
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 *
 */
const char *ccnxKeystoreUtilities_GetPassword(const KeystoreParams *params);
#endif // libccnx_ccnx_KeystoreUtilities_h
