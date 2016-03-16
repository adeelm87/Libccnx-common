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
 * @file ccnx_Manifest.h
 * @ingroup ContentObject
 * @brief The generic manifest.
 *
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#ifndef libccnx_ccnx_Manifest_h
#define libccnx_ccnx_Manifest_h

#include <ccnx/common/ccnx_Name.h>
#include <ccnx/common/ccnx_ManifestSection.h>

#include <parc/security/parc_Signature.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Buffer.h>

struct ccnx_manifest;

/**
 * @typedef CCNxManifest
 * @brief Structure of the CCNxManifest
 */
typedef struct ccnx_manifest CCNxManifest;

/**
 * Create a new `CCNxManifest` instance.
 *
 * @param [in] signature A pointer to a `PARCSignature`
 * @param [in] nameLink A pointer to a `CCNxLink`
 * @param [in] metadata A pointer to a `CCNxManifestSection`
 * @param [in] payload A pointer to a `CCNxManifestSection`
 *
 * @return A pointer to a `CCNxManifest` instance, or NULL if an error or out of memory.
 *
 * Example:
 * @code
 * {
 *     PARCBuffer *signatureBits = parcBuffer_Allocate(256);
 *     PARCSignature *signature = parcSignature_Create(PARCSigningAlgorithm_RSA, PARC_HASH_SHA256, signatureBits);
 *
 *     CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar/manifest");
 *     CCNxLink *nameLink = ccnxLink_Create(name, NULL, NULL);
 *
 *     CCNxManifest *object = ccnxManifest_Create(signature, name, NULL, NULL);
 * }
 * @endcode
 */
CCNxManifest *ccnxManifest_Create(PARCSignature *signature, CCNxLink *nameLink,
                                  CCNxManifestSection *metadata, CCNxManifestSection *payload);

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
 * Get the Metadata section of the hotel.
 *
 * @param [in] manifest A pointer to an instance of `CCNxManifest`.
 *
 * @return A pointer to a `CCNxManifestSection`
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ...;
 *
 *     CCNxManifestSection *metadataSection = ccnxManifest_GetMetadataSection(manifest);
 * }
 * @endcode
 *
 */
CCNxManifestSection *ccnxManifest_GetMetadataSection(const CCNxManifest *manifest);

/**
 * Get the paylod section of a `CCNxManifest`.
 *
 * @param [in] manifest A pointer to an instance of `CCNxManifest`.
 *
 * @return A pointer to a  `CCNxManifest`.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ...;
 *
 *     CCNxManifestSection *payloadSection = ccnxManifest_GetPayloadSection(manifest);
 * }
 * @endcode
 */
CCNxManifestSection *ccnxManifest_GetPayloadSection(const CCNxManifest *manifest);

/**
 * Get the `CCNxLink` for the given `CCNxManifest`.
 *
 * @param [in] manifest A pointer to an instance of `CCNxManifest`.
 * @return A pointer to the `CCNxLink`.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ...;
 *
 *     CCNxLink *nameLink = ccnxManifest_GetNameLink(manifest);
 * }
 * @endcode
 */
CCNxLink *ccnxManifest_GetNameLink(const CCNxManifest *manifest);

/**
 * Get the `PARCSignature instance for the given `CCNxManifest`.
 *
 * @param [in] manifest A pointer to an instance of `CCNxManifest`.
 * @return A pointer to a `PARCSignature` instance.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ...;
 *
 *     PARCSignature *signature = ccnxManifest_GetSignature(manifest);
 * }
 * @endcode
 */
PARCSignature *ccnxManifest_GetSignature(const CCNxManifest *manifest);

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

/**
 * Verify the `CCNxManifest`.
 *
 * @param [in] ccnxManifest A pointer to an instance of `CCNxManifest`.
 * @return A pointer to the @p ccnxManifest if it is verified, NULL otherwise.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ...;
 *
 *     assertNotNull(ccnxManifest_Verify(manifest), "Expected verification to return the validated object");
 * }
 * @endcode
 */
CCNxManifest *ccnxManifest_Verify(const CCNxManifest *ccnxManifest);
#endif // libccnx_ccnx_Manifest_h
