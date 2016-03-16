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
 * @file ccnx_ManifestSection.h
 * @ingroup ContentObject
 * @brief A SECTION in a generic manifest (metadata or payload).
 *
 * @author Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#ifndef libccnx_ccnx_ManifestSection_h
#define libccnx_ccnx_ManifestSection_h

#include <ccnx/common/ccnx_Name.h>
#include <ccnx/common/ccnx_Link.h>

#include <parc/security/parc_Signature.h>
#include <parc/algol/parc_Buffer.h>
#include <parc/algol/parc_List.h>
#include <parc/algol/parc_Memory.h>

struct ccnx_manifest_section;

/**
 * @typedef CCNxManifestSection
 * @brief A section in a generic Manifest (metadata or payload)
 */
typedef struct ccnx_manifest_section CCNxManifestSection;

/**
 * Create a new {@link CCNxManifestSection} instance.
 *
 * @param [in] acsLink - A pointer to a {@link CCNxLink} to the ACS - optional.
 *
 * @return A pointer to a {@link CCNxManifestSection} instance, or NULL if an error or out of memory.
 *
 * Example:
 * @code
 * {
 *     CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
 *     CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);
 *
 *     CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);
 *
 *     ...
 *
 *     ccnxManifestSection_Release(&section);
 * }
 * @endcode
 */
CCNxManifestSection *ccnxManifestSection_Create(CCNxLink *acsLink);

/**
 * Increase the number of references to an instance of this object.
 *
 * Note that new instance is not created,
 * only that the given instance's reference count is incremented.
 * Discard the reference by invoking {@link ccnxManifestSection_Release()}.
 *
 * @param [in] section A pointer to the {@link CCNxManifestSection} to acquire.
 * @return The value of the input parameter @p section.
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ccnxManifestSection_Acquire(instance);
 *
 *     ccnxManifestSection_Release(&manifest);
 * }
 * @endcode
 *
 * @see `ccnxManifestSection_Release`
 */
CCNxManifestSection *ccnxManifestSection_Acquire(const CCNxManifestSection *section);

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
 * @param [in,out] sectionP A pointer to a pointer to the instance to release.
 *
 *
 * Example:
 * @code
 * {
 *     CCNxManifestSection *manifest = ccnxManifestSection_Acquire(instance);
 *
 *     ccnxManifestSection_Release(&manifest);
 * }
 * @endcode
 */
void ccnxManifestSection_Release(CCNxManifestSection **sectionP);

/**
 * Check that the pointer to the {@link CCNxManifestSection} is valid. It should be non-null,
 * and any referenced data should also be valid.
 *
 * @param [in] manifest A pointer to the {@link CCNxManifestSection} to check.
 *
 *
 * Example:
 * @code
 * {
 *     CCNxManifest *manifest = ccnxManifestSection_Acquire(instance);
 *
 *     ccnxManifestSection_AssertValid(manifest);
 * }
 * @endcode
 */
void ccnxManifestSection_AssertValid(const CCNxManifestSection *manifest);

/**
 * Add a new name entry to the {@link CCNxManifestSection} with the specified name and hash digest.
 *
 * @param [in] section - A {@link CCNxManifestSection} instance.
 * @param [in] name - The {@link CCNxName} instance to add.
 * @param [in] chunk - The chunk for the name entry.
 * @param [in] digest - The hash digest of the name entry.
 *
 * @return True if the name was added successful, False otherwise.
 *
 * Example:
 * @code
 * {
 *     CCNxName *acsName = ccnxName_CreateFromCString("lci:/foo/bar/manifest/acs");
 *     CCNxLink *acsLink = ccnxLink_Create(acsName, NULL, NULL);
 *     CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);
 *
 *     CCNxName *nameEntry = ccnxName_CreateFromCString("lci:/some/content");
 *     PARCBuffer *hashDigest = ...;
 *
 *     ccnxManifestSection_AddNameEntry(section, nameEntry, 0, hashDigest);
 *
 *     ccnxManifestSection_Release(&section);
 * }
 * @endcode
 */
bool ccnxManifestSection_AddNameEntry(CCNxManifestSection *section, const CCNxName *name, size_t chunk, const PARCBuffer *digest);

/**
 * Get a {@link CCNxLink} for the Access Control Specification for the {@link CCNxManifestSection}.
 *
 * @param [in] section A pointer to a {@link CCNxManifestSection} from which to retrieve the link.
 *
 * @return A pointer to a `CCNxLink` containing the Access Control Specification.
 *
 * Example:
 * @code
 * {
 *     CCNxLink *acsLink = ...;
 *
 *     CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);
 *
 *     CCNxLink *acs = ccnxManifestSection_GetACSLink(section);
 *     // create and issue interest for ACS
 *     ccnxLink_Release(&acs);
 *
 *     ccnxManifestSection_Release(&section);
 * }
 * @endcode
 */
CCNxLink *ccnxManifestSection_GetACSLink(const CCNxManifestSection *section);

/**
 * Get a count of the Names in the {@link CCNxManifestSection}.
 *
 * @param [in] section A pointer to the {@link CCNxManifestSection} from which to count the names.
 *
 * @return The number of Names in the @p section.
 *
 * Example:
 * @code
 * {
 *     CCNxLink *acsLink = ...;
 *
 *     CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);
 *
 *     size_t numberOfNames = ccnxManifestSection_GetNameCount(section);
 *     printf("Number of names in manifest = %u\n", numberOfNames);
 *
 *     ccnxManifestSection_Release(&section);
 * }
 * @endcode
 */
size_t ccnxManifestSection_GetNameCount(const CCNxManifestSection *section);

/**
 * Get the number of hashes in a {@link CCNxManifestSection}.
 *
 * @param [in] section A pointer to the {@link CCNxManifestSection} from which to count the hashes.
 *
 * @return A count of the hashes in @p section.
 *
 * Example:
 * @code
 * {
 *     CCNxLink *acsLink = ...;
 *
 *     CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);
 *
 *     size_t numberOfHashes = ccnxManifestSection_GetHashCount(section);
 *     printf("Number of hash entries in manifest = %u\n", numberOfHashes);
 *
 *     ccnxManifestSection_Release(&section);
 * }
 * @endcode
 */
size_t ccnxManifestSection_GetHashCount(const CCNxManifestSection *section);

/**
 * Get the media name from the hash index of a {@link CCNxManifestSection}.
 *
 * @param [in] section A pointer to the {@link CCNxManifestSection} from which to extract the media name at @p index.
 * @param [in] index The index into @p section from which to extract the media name.
 *
 * @return A pointer to a {@link CCNxName} containing the media name.
 *
 * Example:
 * @code
 * {
 *     CCNxLink *acsLink = ...;
 *
 *     CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);
 *
 *     size_t numberOfHashes = ccnxManifestSection_GetHashCount(section);
 *     for (size_t i = 0; i < numberOfHashes; i++) {
 *         CCNxName *contentName = ccnxManifestSection_GetNameFromHashIndex(section, i);
 *         // issue interest for the content with the given name
 *     }
 *
 *     ccnxManifestSection_Release(&section);
 * }
 * @endcode
 */
CCNxName *ccnxManifestSection_GetNameFromHashIndex(const CCNxManifestSection *section, size_t index);

/**
 * Get the media name chunk number from the hash index of a {@link CCNxManifestSection}.
 *
 * @param [in] section A pointer to the {@link CCNxManifestSection} from which to extract the media name at @p index.
 * @param [in] index The index into @p section from which to extract the media name.
 *
 * @return The chunk number associated with the name of the specified hash entry.
 *
 * Example:
 * @code
 * {
 *     CCNxLink *acsLink = ...;
 *
 *     CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);
 *
 *     size_t numberOfHashes = ccnxManifestSection_GetHashCount(section);
 *     for (size_t i = 0; i < numberOfHashes; i++) {
 *         size_t chunkNumber = ccnxManifestSection_GetNameChunkFromHashIndex(section, i);
 *         // use the chunk number as needed
 *     }
 *
 *     ccnxManifestSection_Release(&section);
 * }
 * @endcode
 */
size_t ccnxManifestSection_GetNameChunkFromHashIndex(const CCNxManifestSection *section, size_t index);

/**
 * Get the hash at an index into a {@link CCNxManifestSection}.
 *
 * @param [in] section A pointer to the {@link CCNxManifestSection} from which to get the hashe at @p index.
 * @param [in] index An index into @p section from which to get the hash.
 *
 * @return  A pointer to a {@link PARCBuffer} containing the hash.
 *
 * Example:
 * @code
 * {
 *     CCNxLink *acsLink = ...;
 *
 *     CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);
 *
 *     size_t numberOfHashes = ccnxManifestSection_GetHashCount(section);
 *     for (size_t i = 0; i < numberOfHashes; i++) {
 *         PARCBuffer *hashEntry = ccnxManifestSection_GetHashAtIndex(section, i);
 *         // use the hash entry as needed
 *     }
 *
 *     ccnxManifestSection_Release(&section);
 * }
 * @endcode
 */
PARCBuffer *ccnxManifestSection_GetHashAtIndex(const CCNxManifestSection *section, size_t index);

/**
 * Determine if two {@link CCNxManifestSection} instances are equal.
 *
 * The following equivalence relations on non-null {@link CCNxManifestSection} instances are maintained:
 *   * It is reflexive: for any non-null reference value x, `ccnxManifestSection_Equals(x, x)` must return true.
 *
 *   * It is symmetric: for any non-null reference values x and y, `ccnxManifestSection_Equals(x, y)` must return true if and only if
 *        `ccnxManifestSection_Equals(y x)` returns true.
 *
 *   * It is transitive: for any non-null reference values x, y, and z, if
 *        `ccnxManifestSection_Equals(x, y)` returns true and
 *        `ccnxManifestSection_Equals(y, z)` returns true,
 *        then  `ccnxManifestSection_Equals(x, z)` must return true.
 *
 *   * It is consistent: for any non-null reference values x and y, multiple invocations of `ccnxManifestSection_Equals(x, y)`
 *         consistently return true or consistently return false.
 *
 *   * For any non-null reference value x, `ccnxManifestSection_Equals(x, NULL)` must return false.
 *
 *
 * @param [in] x A pointer to a {@link CCNxManifestSection} instance.
 * @param [in] y A pointer to a {@link CCNxManifestSection} instance.
 * @return `true` if the referenced {@link CCNxManifestSection} instances are equal.
 *
 * Example:
 * @code
 * {
 *    CCNxManifestSection *a = ccnxManifestSection_Create(...);
 *    CCNxManifestSection *b = ccnxManifestSection_Create(...);
 *
 *    if (ccnxManifestSection_Equals(a, b)) {
 *        // true
 *    } else {
 *        // false
 *    }
 * }
 * @endcode
 */
bool ccnxManifestSection_Equals(const CCNxManifestSection *x, const CCNxManifestSection *y);

/**
 * Create a null-terminated string representation of the given {@link CCNxManifestSection}.
 *
 * The returned value must be freed by the caller using {@link parcMemory_Deallocate()}.
 *
 * @param [in] section A pointer to a {@link CCNxManifestSection} instance.
 *
 * @return A pointer to null-terminated string of characters repesenting the @p section that must be freed by the caller by `parcMemory_Deallocate()`.
 *
 * Example:
 * @code
 * {
 *     CCNxLink *acsLink = ...;
 *
 *     CCNxManifestSection *section = ccnxManifestSection_Create(acsLink);
 *
 *     char *sectionDescription = ccnxManifestSection_ToString(section);
 *     printf("Manifest: %s\n", sectionDescription);
 *
 *     ccnxManifestSection_Release(&section);
 * }
 * @endcode
 */
char *ccnxManifestSection_ToString(const CCNxManifestSection *section);
#endif // libccnx_ccnx_ManifestSection_h
