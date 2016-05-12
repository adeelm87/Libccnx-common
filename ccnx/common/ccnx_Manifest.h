/*
 * Copyright (c) 2015-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @file ccnx_Manifest.h
 * @ingroup ContentObject
 * @brief The generic manifest.
 *
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2015-2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef libccnx_ccnx_Manifest_h
#define libccnx_ccnx_Manifest_h

#include <ccnx/common/ccnx_Name.h>
#include <ccnx/common/ccnx_ManifestHashGroup.h>

#include <ccnx/common/internal/ccnx_ManifestInterface.h>
#include <ccnx/common/internal/ccnx_TlvDictionary.h>

#include <parc/security/parc_Signature.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Buffer.h>

struct ccnx_manifest;

/**
 * @typedef CCNxManifest
 * @brief Structure of the CCNxManifest
 */
typedef CCNxTlvDictionary CCNxManifest;

/**
 * Create a new `CCNxManifest` instance.
 *
 * @param [in] nameLink A pointer to a `CCNxName`
 * @param [in] payload A pointer to a `CCNxManifestSection`
 *
 * @return A pointer to a `CCNxManifest` instance, or NULL if an error or out of memory.
 *
 * Example:
 * @code
 * {
 *     CCNxName *name = ccnxName_CreateFromURI("lci:/foo/bar/manifest");
 *
 *
 *     CCNxManifest *object = ccnxManifest_Create(name, payload);
 * }
 * @endcode
 */
CCNxManifest *ccnxManifest_Create(const CCNxName *name);

/**
 * Increase the number of references to an instance of this object.
 *
 * Note that new instance is not created,
 * only that the given instance's reference count is incremented.
 * Discard the reference by invoking {@link ccnxManifest_Release()}.
 *
 * @param [in] manifest A pointer to the instance of `CCNxManifest` to acquire.
 *
 * @return The value of the input parameter @p instance.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ccnxManifest_Acquire(instance);
 *
 *     ccnxManifest_Release(&manifest);
 * }
 * @endcode
 *
 * @see `ccnxManifest_Release`
 */
CCNxManifest *ccnxManifest_Acquire(const CCNxManifest *manifest);

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
 * @param [in,out] manifestP A pointer to a pointer to the instance to release.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ccnxManifest_Acquire(instance);
 *
 *     ccnxManifest_Release(&manifest);
 *
 * }
 * @endcode
 */
void ccnxManifest_Release(CCNxManifest **manifestP);

/**
 * Check that the pointer to the `CCNxManifest` is valid. It should be non-null,
 * and any referenced data should also be valid.
 *
 * @param [in] manifest A pointer to an instance of `CCNxManifest` to check.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ccnxManifest_Acquire(instance);
 *
 *     ccnxManifest_AssertValid(manifest);
 *
 * }
 * @endcode
 */
void ccnxManifest_AssertValid(const CCNxManifest *manifest);

/**
 * Add a HashGroup to the given `CCNxManifest`.
 *
 * @param [in] manifest A pointer to an instance of `CCNxManifest`.
 * @param [in] group A pointer to an instance of `CCNxManifestHashGroup`.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ...
 *     CCNxManifestHashGroup *group = ...;
 *
 *     ccnxManifest_AddHashGroup(manifest, group);
 * }
 * @endcode
 */
void ccnxManifest_AddHashGroup(CCNxManifest *manifest, const CCNxManifestHashGroup *group);

/**
 * Get the `CCNxManifestHashGroup` corresponding to the specified index.
 *
 * @param [in] manifest A pointer to an instance of `CCNxManifest`.
 * @param [in] index The index of the `CCNxManifestHashGroup` to retrieve.
 *
 * @return A pointer to a `CCNxManifestHashGroup`.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ...
 *     CCNxManifestHashGroup *group = ...;
 *
 *     // Add the first group
 *     ccnxManifest_AddHashGroup(manifest, group);
 *
 *     CCNxManifestHashGroup *expected = ccnxManifest_GetHashGroup(manifest, 0);
 * }
 * @endcode
 */
CCNxManifestHashGroup *ccnxManifest_GetHashGroupByIndex(const CCNxManifest *manifest, size_t index);

/**
 * Get the number of {@link CCNxManifestHashGroup} instances in the specified manifest.
 *
 * @param [in] manifest A pointer to an instance of {@link CCNxManifest}.
 *
 * @return A pointer to a {@link CCNxManifestHashGroup}.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ...
 *     CCNxManifestHashGroup *group = ...;
 *
 *     // Add the first group
 *     ccnxManifest_AddHashGroup(manifest, group);
 *
 *     // Add more groups...
 *
 *     printf("Number of hash groups: %d\n", ccnxManifest_GetNumberOfHashGroups(manifest));
 * }
 * @endcode
 */
size_t ccnxManifest_GetNumberOfHashGroups(const CCNxManifest *manifest);

/**
 * Get the `CCNxName` for the given `CCNxManifest`.
 *
 * @param [in] manifest A pointer to an instance of `CCNxManifest`.
 * @return A pointer to the `CCNxName`.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ...;
 *
 *     CCNxName *name = ccnxManifest_GetName(manifest);
 * }
 * @endcode
 */
const CCNxName *ccnxManifest_GetName(const CCNxManifest *manifest);

/**
 * Determine if two `CCNxManifest` instances are equal.
 *
 * The following equivalence relations on non-null `CCNxManifest` instances are maintained:
 *
 *   * It is reflexive: for any non-null reference value x, `ccnxManifest_Equals(x, x)` must return true.
 *
 *   * It is symmetric: for any non-null reference values x and y, `ccnxManifest_Equals(x, y)` must return true if and only if
 *        `ccnxManifest_Equals(y x)` returns true.
 *
 *   * It is transitive: for any non-null reference values x, y, and z, if
 *        `ccnxManifest_Equals(x, y)` returns true and
 *        `ccnxManifest_Equals(y, z)` returns true,
 *        then  `ccnxManifest_Equals(x, z)` must return true.
 *
 *   * It is consistent: for any non-null reference values x and y, multiple invocations of `ccnxManifest_Equals(x, y)`
 *         consistently return true or consistently return false.
 *
 *   * For any non-null reference value x, `ccnxManifest_Equals(x, NULL)` must return false.
 *
 *
 * @param [in] x A pointer to a `CCNxManifest` instance.
 * @param [in] y A pointer to a `CCNxManifest` instance.
 * @return `true` if the referenced `CCNxManifest` instances are equal.
 *
 * Example:
 * @code
 * {
 *    CCNxManifest *a = CCNxManifest_Create(...);
 *    CCNxManifest *b = CCNxManifest_Create(...);
 *
 *    if (CCNxManifest_Equals(a, b)) {
 *        // true
 *    } else {
 *        // false
 *    }
 * }
 * @endcode
 */
bool ccnxManifest_Equals(const CCNxManifest *x, const CCNxManifest *y);

/**
 * Create a null-terminated string representation of the given {@link CCNxManifest}.
 *
 * The returned value must be freed by the caller using {@link parcMemory_Deallocate()}.
 *
 * @param [in] manifest A pointer to an instance of {@link CCNxManifest}.
 * @return A pointer to null-terminated string of characters that must be freed by the caller by `parcMemory_Deallocate()`.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ...;
 *
 *     char *stringForm = ccnxManifest_ToString(manifest);
 * }
 * @endcode
 */
char *ccnxManifest_ToString(const CCNxManifest *manifest);
#endif // libccnx_ccnx_Manifest_h
