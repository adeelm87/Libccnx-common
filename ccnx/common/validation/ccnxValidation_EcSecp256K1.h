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
 * @file ccnxValidation_EcSecp256K1.h
 * @brief <#Brief Description#>
 *
 * <#Detailed Description#>
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef CCNx_Common_ccnxValidation_EcSecp256K1_h
#define CCNx_Common_ccnxValidation_EcSecp256K1_h

#include <stdbool.h>
#include <ccnx/common/internal/ccnx_TlvDictionary.h>
#include <ccnx/common/ccnx_KeyLocator.h>

/**
 *
 * Sets the validation algorithm to be Elliptical Curve with SECP-256K1 parameters.
 * Optionally includes a KeyId and KeyLocator with the message.
 *
 * @param [in] message The message dictionary
 * @param [in] keyid (Optional) The KEYID to include the the message
 * @param [in] keyLocator (Optional) The KEY LOCATOR to include in the message
 *
 * @return true success
 * @return false failure
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxValidationEcSecp256K1_Set(CCNxTlvDictionary *message, const PARCBuffer *keyid, const CCNxKeyLocator *keyLocator);

/**
 * Determines if the validation algorithm is Elliptical Curve with SECP-256K1 parameters.
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] message The message to check
 *
 * @return true The validation algorithm in the dictionary is this one
 * @return false The validaiton algorithm in the dictionary is something else or not present
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool ccnxValidationEcSecp256K1_Test(const CCNxTlvDictionary *message);

/**
 * Returns the KeyId associated with the validation algorithm
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] message The message to check
 *
 * @return non-NULL the keyid
 * @return null An error or no keyid or no validation algorithm in the message
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCBuffer *ccnxValidationEcSecp256K1_GetKeyId(const CCNxTlvDictionary *message);

/**
 * Returns the KeyName associated with the validation algorithm
 *
 * This should return a LINK, see case 1018
 *
 * @param [in] message The message to check
 *
 * @return non-NULL the KeyName
 * @return null An error or no keyid or no validation algorithm in the message
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxName   *ccnxValidationEcSecp256K1_GetKeyLocatorName(const CCNxTlvDictionary *message);

/**
 * Returns the PublicKey associated with the validation algorithm
 *
 * @param [in] message The message to check
 *
 * @return non-NULL the PublicKey (DER encoded)
 * @return null An error or no public key or no validation algorithm in the message
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCBuffer *ccnxValidationEcSecp256K1_GetKeyLocatorPublicKey(const CCNxTlvDictionary *message);

/**
 * Returns the Certificate associated with the validation algorithm
 *
 * @param [in] message The message to check
 *
 * @return non-NULL the Certificate (DER encoded)
 * @return null An error or no certificate or no validation algorithm in the message
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCBuffer *ccnxValidationEcSecp256K1_GetKeyLocatorCertificate(const CCNxTlvDictionary *message);
#endif // CCNx_Common_ccnxValidation_EcSecp256K1_h
