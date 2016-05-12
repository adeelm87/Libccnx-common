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
 * @file ccnx_Link.h
 * @ingroup ContentObject
 * @brief A generic CCNx link that contains a hash, name, KeyID.
 *
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
#ifndef libccnx_ccnx_Link_h
#define libccnx_ccnx_Link_h

#include <ccnx/common/ccnx_Name.h>

#include <parc/security/parc_Signature.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Buffer.h>

struct ccnx_link;

/**
 * @typedef CCNxLink
 * @brief A generic CCNx link that contains a hash, name, KeyID.
 * @see {@link ccnxLink_Create}
 */
typedef struct ccnx_link CCNxLink;

/**
 * Create a new `CCNxLink` instance.
 *
 * @param [in] name The {@link CCNxName} for the new `CCNxLink`.
 * @param [in] keyId A {@link PARCBuffer} containing the KeyID for the new `CCNxLink`.
 * @param [in] contentObjectHash A `PARCBuffer` containing the Content Object Hash for the new `CCNxLink`.
 * @return A pointer to a new `CCNxLink` instance, or NULL if an error or out of memory.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxLink *ccnxLink_Create(const CCNxName *name, PARCBuffer *keyId, PARCBuffer *contentObjectHash);

/**
 * Create a new `CCNxLink` instance as a copy of the original.
 *
 * @param [in] original The {@link CCNxLink} to copy.
 * @return A pointer to a new `CCNxLink` instance, or NULL if an error or out of memory.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxLink *ccnxLink_Copy(const CCNxLink *original);

/**
 * Fetch the name associated with this `CCNxLink`.
 *
 * @param [in] link  A pointer to a `CCNxLink` instance
 * @return The {@link CCNxName} associated with the @p link.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const CCNxName *ccnxLink_GetName(const CCNxLink *link);

/**
 * Fetch the KeyID associated with this `CCNxLink`.
 *
 * @param [in] link  A pointer to a `CCNxLink` instance
 * @return a {@link PARCBuffer} containing the KeyID associated with the @p link.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCBuffer *ccnxLink_GetKeyID(const CCNxLink *link);

/**
 * Fetch the ContentObjectHash associated with this `CCNxLink`.
 *
 * @param [in] link  A pointer to a `CCNxLink` instance
 * @return a {@link PARCBuffer} containing the Content Object Hash associated with the @p link.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCBuffer *ccnxLink_GetContentObjectHash(const CCNxLink *link);

/**
 * Determine if two `CCNxLink` instances are equal.
 *
 * The following equivalence relations on non-null `CCNxLink` instances are maintained:
 *
 *   * It is reflexive: for any non-null reference value x, `ccnxLink_Equals(x, x)` must return true.
 *
 *   * It is symmetric: for any non-null reference values x and y, `ccnxLink_Equals(x, y)` must return true if and only if
 *        `ccnxLink_Equals(y x)` returns true.
 *
 *   * It is transitive: for any non-null reference values x, y, and z, if
 *        `ccnxLink_Equals(x, y)` returns true and
 *        `ccnxLink_Equals(y, z)` returns true,
 *        then  `ccnxLink_Equals(x, z)` must return true.
 *
 *   * It is consistent: for any non-null reference values x and y, multiple invocations of `ccnxLink_Equals(x, y)`
 *         consistently return true or consistently return false.
 *
 *   * For any non-null reference value x, `ccnxLink_Equals(x, NULL)` must return false.
 *
 *
 * @param [in] x A pointer to a `CCNxLink` instance.
 * @param [in] y A pointer to a `CCNxLink` instance.
 * @return `true` if the referenced `CCNxLink` instances are equal.
 *
 * Example:
 * @code
 * {
 *    CCNxLink *a = ccnxLink_Create();
 *    CCNxLink *b = ccnxLink_Create();
 *
 *    if (ccnxLink_Equals(a, b)) {
 *        // true
 *    } else {
 *        // false
 *    }
 * }
 * @endcode
 */
bool ccnxLink_Equals(const CCNxLink *x, const CCNxLink *y);

/**
 * Create a null-terminated string representation of the given `CCNxLink`.
 *
 * The returned value must be freed by the caller using {@link parcMemory_Deallocate()}.
 *
 * @param [in] link  A pointer to a `CCNxLink` instance.
 * @return A pointer to null-terminated string of characters that must be freed by the caller by `parcMemory_Deallocate()`.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
char *ccnxLink_ToString(const CCNxLink *link);

/**
 * Increase the number of references to an instance of this object.
 *
 * Note that new instance is not created,
 * only that the given instance's reference count is incremented.
 * Discard the reference by invoking {@link ccnxLink_Release()}.
 *
 * @param [in] link  A pointer to a `CCNxLink` instance to acquire.
 * @return The value of the input parameter @p link.
 *
 * Example:
 * @code
 * {
 *     ...
 *
 *     CCNxLink *link = ccnxLink_Acquire(instance);
 *
 *     ccnxLink_Release(&link);
 *
 * }
 * @endcode
 *
 * @see ccnxLink_Release
 */
CCNxLink *ccnxLink_Acquire(const CCNxLink *link);

/**
 * Release a previously acquired reference to the specified instance,
 * decrementing the reference count for the instance.
 *
 * The pointer to the instance is set to NULL as a side-effect of this function.
 *
 * If the invocation causes the last reference to the instance to be released,
 * the instance is deallocated and the instance's implementation will perform
 * additional cleanup and release other privately held references.
 *
 * @param [in,out] linkP A pointer to a pointer to the instance to release.
 *
 *
 * Example:
 * @code
 * {
 *     ...
 *
 *     CCNxLink *link = ccnxLink_Acquire(instance);
 *
 *     ccnxLink_Release(&link);
 *
 * }
 * @endcode
 */
void ccnxLink_Release(CCNxLink **linkP);

/**
 * Check that the pointer to the `CCNxLink` is valid. It should be non-null,
 * and any referenced data should also be valid.
 *
 * @param [in] link  A pointer to a `CCNxLink` instance
 *
 *
 * Example:
 * @code
 * {
 *     ...
 *
 *     CCNxLink *link = ccnxLink_Acquire(instance);
 *
 *     ccnxLink_AssertValid(link);
 *
 * }
 * @endcode
 */
void ccnxLink_AssertValid(const CCNxLink *link);

/**
 * Determine if a `CCNxLink` instance is valid.
 *
 * @param [in] link  A pointer to a `CCNxLink` instance
 * @return true The instance is valid.
 * @return false The instance is invalid.
 *
 *
 * Example:
 * @code
 * {
 *     ...
 *
 *     CCNxLink *link = ccnxLink_Acquire(instance);
 *
 *     ccnxLink_AssertValid(link);
 *
 * }
 * @endcode
 */
bool ccnxLink_IsValid(const CCNxLink *link);
#endif // libccnx_ccnx_Link_h
