/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * ################################################################################
 * #
 * # PATENT NOTICE
 * #
 * # This software is distributed under the BSD 2-clause License (see LICENSE
 * # file).  This BSD License does not make any patent claims and as such, does
 * # not act as a patent grant.  The purpose of this section is for each contributor
 * # to define their intentions with respect to intellectual property.
 * #
 * # Each contributor to this source code is encouraged to state their patent
 * # claims and licensing mechanisms for any contributions made. At the end of
 * # this section contributors may each make their own statements.  Contributor's
 * # claims and grants only apply to the pieces (source code, programs, text,
 * # media, etc) that they have contributed directly to this software.
 * #
 * # There is no guarantee that this section is complete, up to date or accurate. It
 * # is up to the contributors to maintain their portion of this section and up to
 * # the user of the software to verify any claims herein.
 * #
 * # Do not remove this header notification.  The contents of this section must be
 * # present in all distributions of the software.  You may only modify your own
 * # intellectual property statements.  Please provide contact information.
 * 
 * - Palo Alto Research Center, Inc
 * This software distribution does not grant any rights to patents owned by Palo
 * Alto Research Center, Inc (PARC). Rights to these patents are available via
 * various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
 * intellectual property used by its contributions to this software. You may
 * contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org
 */
/**
 * @file ccnx_KeystoreUtilities.h
 * @ingroup Signature
 * @brief A set of tools for working with the CCNx keystore.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
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
 * Create a new `KeystoreParams` from a @p path, @p password, and a {@link PARCSigner}.
 *
 * @param [in] signer A pointer to an instance of `PARCSigner`.
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
KeystoreParams *ccnxKeystoreUtilities_Create(PARCSigner *signer, const char *path, const char *password);

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
