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
 * @file ccnx_InterestPayloadId.h
 * @ingroup Naming
 * @brief The basic CCNx InterestPayloadId.
 *
 *
 * @author Mike Slominski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef libccnx_ccnx_InterestPayloadId_h
#define libccnx_ccnx_InterestPayloadId_h

#include <parc/algol/parc_Buffer.h>
#include <ccnx/common/ccnx_NameSegment.h>
#include <ccnx/common/ccnx_Name.h>
//#include <parc/security/parc_CryptoHashType.h>

struct ccnx_interest_payload_id;
/**
 * @typedef CCNxInterestPayloadId
 * @brief An RFC3986 URI compliant identifier in which each path segment carries a label.
 */
typedef struct ccnx_interest_payload_id CCNxInterestPayloadId;


typedef enum {
    //RFC 6920
    CCNxInterestPayloadId_TypeCode_RFC6920_Reserved = 0x00,
    CCNxInterestPayloadId_TypeCode_RFC6920_SHA256 = 0x01, //256 bits
    CCNxInterestPayloadId_TypeCode_RFC6920_SHA256_128 = 0X02, //128 bits
    CCNxInterestPayloadId_TypeCode_RFC6920_SHA256_120 = 0X03, //120 bits
    CCNxInterestPayloadId_TypeCode_RFC6920_SHA256_96 = 0X04, //96 bits
    CCNxInterestPayloadId_TypeCode_RFC6920_SHA256_64 = 0X05, //64 bits
    CCNxInterestPayloadId_TypeCode_RFC6920_SHA256_32 = 0x06, //32 bits
    //PARC
    CCNxInterestPayloadId_TypeCode_App = 0x80,
    CCNxInterestPayloadId_TypeCode_Nonce = 0x81
} CCNxInterestPayloadId_TypeCode;


/**
 * Create a new instance of a `CCNxInterestPayloadId` that uses the provided
 * PARCBuffer as the ID. The instance is dynamically allocated and must be released by
 * calling {@link ccnxInterestPayloadId_Release}.
 *
 * @param [in] data a PARCBuffer to be used as the body of the KeyId name segment.
 * @param [in] type a uint8_t, nominally one of the CCNxInterestPayloadId_TypeCodes,
 * which is prepended to the data.
 *
 * @return A pointer to a `CCNxInterestPayloadId` instance.
 *
 * Example:
 * @code
 * {
 *     PARCBuffer *somePaylaod = ....
 *     CCNxInterest *someInterest = ....
 *
 *     PARCBuffer *buffer = parcBuffer_WrapCString("Key_42");
 *     CCNxInterestPayloadId *interestPayloadId =
 *        ccnxInterestPayloadId_Create(data, InterestPayloadIdType_App);
 *     parcBuffer_Release(&buffer);
 *
 *     ccnxInterest_SetPayloadAndId(someInterest, somePayload, interestPayloadId);
 *     ccnxInterestPayloadId_Release(&interestPayloadId);
 *     parcBuffer_Release(&somePayload);
 *
 *     ...
 *
 * }
 * @endcode
 */
CCNxInterestPayloadId *
ccnxInterestPayloadId_Create(const PARCBuffer *data, uint8_t type);


/**
 * Create a new instance of a `CCNxInterestPayloadId` that is a sha256 hash of the
 * input data. The instance is dynamically allocated and must be released by
 * calling {@link ccnxInterestPayloadId_Release}.
 *
 * @param [in] data a PARCBuffer to be hashed to obtain the ID bytes.
 *
 * @return A pointer to a `CCNxInterestPayloadId` instance.
 *
 * Example:
 * @code
 * {
 *     PARCBuffer *somePaylaod = ....
 *     CCNxInterest *someInterest = ....
 *
 *     CCNxInterestPayloadId *interestPayloadId =
 *        ccnxInterestPayloadId_CreateAsSHA256Hash(somePayload);
 *
 *     ccnxInterest_SetPayloadAndId(someInterest, somePayload, interestPayloadId);
 *     ccnxInterestPayloadId_Release(&interestPayloadId);
 *     parcBuffer_Release(&somePayload);
 *
 *     ...
 *
 * }
 * @endcode
 */
CCNxInterestPayloadId *
ccnxInterestPayloadId_CreateAsSHA256Hash(const PARCBuffer *data);

/**
 * Create a new instance of a `CCNxInterestPayloadId` from an Interest Payload Id name
 * segment if one is present in the provided {@link CCNxName}, otherwise return
 * NULL. The instance is dynamically allocated and must be released by calling {@link
 * ccnxInterestPayloadId_Release}.
 *
 * @param [in] name a CCNxName that may or may not carry an Interest Payload Id CCNxNameSegment.
 *
 * @return A pointer to a `CCNxInterestPayloadId` instance or NULL if no
 * Interest-Payload-Id typed CCNxNameSegment is found.
 *
 * Example:
 * @code
 * {
 *     CCNxName *someName = ...
 *
 *     CCNxInterestPayloadId *interestPayloadId =
 *        ccnxInterestPayloadId_CreateFromSegmentInName(someName);
 *
 *     if (interestPayloadId != NULL) {
 *         ....
 *         ccnxInterestPayloadId_Release(&interestPayloadId);
 *     }
 *
 *     ...
 *
 *     ccnxName_Release(&someName);
 * }
 * @endcode
 */
CCNxInterestPayloadId *
ccnxInterestPayloadId_CreateFromSegmentInName(const CCNxName *name);

/**
 * Get a pointer to a CCNxNameSegment derived from the provided
 * CCNxInterestPayloadId. There is no need to release the returned CCNxNameSegment
 * object.
 *
 * @param [in] interestPayloadId A pointer to the `CCNxInterestPayloadId` instance.
 *
 * @return A pointer to a CCNxNameSegment.
 *
 * Example:
 * @code
 * {
 *     CCNxName *someName = ...
 *
 *     CCNxInterestPayloadId *interestPayloadId =
 *        ccnxInterestPayloadId_CreateFromSegmentInName(someName);
 *
 *     if (interestPayloadId != NULL) {
 *         CCNxNameSegmemt *nameSegment = ccnxInterestPayloadId_GetNameSegment(interestPayloadId);
 *         ....
 *         ccnxInterestPayloadId_Release(&interestPayloadId);
 *     }
 *
 *     ...
 *
 *     ccnxName_Release(&someName);
 * }
 * @endcode
 */
const CCNxNameSegment *
ccnxInterestPayloadId_GetNameSegment(const CCNxInterestPayloadId *interestPayloadId);

/**
 * Get the CCNxInterestPayloadId's value as a PARCBuffer. There is no need to release
 * the returned PARCBuffer object.
 *
 * @param [in] interestPayloadId A pointer to the `CCNxInterestPayloadId` instance.
 *
 * @return A pointer to a PARCBuffer object containing the Id bytes.
 *
 * Example:
 * @code
 * {
 *     CCNxName *someName = ...
 *
 *     CCNxInterestPayloadId *interestPayloadId =
 *        ccnxInterestPayloadId_CreateFromSegmentInName(someName);
 *
 *     if (interestPayloadId != NULL) {
 *         PARCBuffer *ipIdValue = ccnxInterestPayloadId_GetValue(interestPayloadId);
 *         ....
 *         ccnxInterestPayloadId_Release(&interestPayloadId);
 *     }
 *
 *     ...
 *
 *     ccnxName_Release(&someName);
 * }
 * @endcode
 */
PARCBuffer *
ccnxInterestPayloadId_GetValue(const CCNxInterestPayloadId *interestPayloadId);

/**
 * Get the CCNxInterestPayloadId's uint8 type.
 *
 * @param [in] interestPayloadId A pointer to the `CCNxInterestPayloadId` instance.
 *
 * @return A uint8_t containing the type value.
 *
 * Example:
 * @code
 * {
 *     CCNxName *someName = ...
 *
 *     CCNxInterestPayloadId *interestPayloadId =
 *        ccnxInterestPayloadId_CreateFromSegmentInName(someName);
 *
 *     if (interestPayloadId != NULL) {
 *         uint8_t type = ccnxInterestPayloadId_GetType(interestPayloadId);
 *         ....
 *         ccnxInterestPayloadId_Release(&interestPayloadId);
 *     }
 *
 *     ...
 *
 *     ccnxName_Release(&someName);
 * }
 * @endcode
 */
uint8_t
ccnxInterestPayloadId_GetType(const CCNxInterestPayloadId *interestPayloadId);

/**
 * Increase the number of references to a `CCNxInterestPayloadId` instance.
 *
 * Note that a new `CCNxInterestPayloadId` is not created, only that the given
 * `CCNxInterestPayloadId` reference count is incremented.  Discard the reference by
 * invoking {@link ccnxInterestPayloadId_Release}.
 *
 * @param [in] interestPayloadId A pointer to the original `CCNxInterestPayloadId`.
 *
 * @return The value of the input parameter @p interestPayloadId.
 *
 * Example:
 * @code
 * {
 *     CCNxInterestPayloadId *original = ccnxInterestPayloadId_CreateAsSHA256Hash(...);
 *
 *     CCNxInterestPayloadId *reference = ccnxInterestPayloadId_Acquire(original);
 *
 *     ccnxInterestPayloadId_Release(&original);
 *     ccnxInterestPayloadId_Release(&reference); //Triggers final cleanup
 * }
 * @endcode
 *
 * @see ccnxInterestPayloadId_Release
 */
CCNxInterestPayloadId *
ccnxInterestPayloadId_Acquire(const CCNxInterestPayloadId *interestPayloadId);

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
 * @param [in] interestPayloadIdP A pointer to a pointer to the instance to release.
 *
 * Example:
 * @code
 * {
 *     CCNxInterestPayloadId *interestPayloadId = ccnxInterestPayloadId_CreateAsSHA256Hash(...);
 *
 *     ccnxInterestPayloadId_Release(&interestPayloadId);
 * }
 * @endcode
 *
 * @see {@link ccnxInterestPayloadId_Acquire}
 */
void
ccnxInterestPayloadId_Release(CCNxInterestPayloadId **interestPayloadIdP);

/**
 * Assert that an instance of `CCNxInterestPayloadId` is valid. If the instance is not
 * valid, terminate via {@link trapIllegalValue}. Valid means the internal state of the
 * type is consistent with its required current or future behavior.  This may include the
 * validation of internal instances of types.
 *
 * @param [in] interestPayloadId A pointer to a `CCNxInterestPayloadId` instance.
 *
 * Example:
 * @code
 * {
 *     CCNxInterestPayloadId *interestPayloadId = ccnxInterestPayloadId_CreateAsSHA256Hash(...);
 *
 *     ccnxInterestPayloadId_AssertValid(interestPayloadId);
 *
 *     ccnxInterestPayloadId_Release(&interestPayloadId);
 * }
 * @endcode
 */
void
ccnxInterestPayloadId_AssertValid(const CCNxInterestPayloadId *interestPayloadId);

/**
 * Determine if an instance of `CCNxInterestPayloadId` is valid.
 *
 * Valid means the internal state of the type is consistent with its required current
 * or future behavior.  This may include the validation of internal instances of
 * types.
 *
 * @param [in] interestPayloadId A pointer to a `CCNxInterestPayloadId` instance.
 *
 * @return true If the instance is valid.
 * @return false if the instance is not valid.
 *
 * Example:
 * @code
 * {
 *     CCNxInterestPayloadId *interestPayloadId = ccnxInterestPayloadId_CreateAsSHA256Hash(...);
 *
 *     if (ccnxInterestPayloadId_IsValid(interestPayloadId) == true) {
 *         ...
 *     }
 *
 *     ccnxInterestPayloadId_Release(&interestPayloadId);
 * }
 * @endcode
 */
bool
ccnxInterestPayloadId_IsValid(const CCNxInterestPayloadId *interestPayloadId);

/**
 * Produce a null-terminated string representation of the specified instance. The
 * non-null result must be freed by the caller via {@link parcMemory_Deallocate}.
 *
 * @param [in] interestPayloadId A pointer to the `CCNxInterestPayloadId` instance.
 *
 * @return NULL Cannot allocate memory.
 * @return non-NULL A pointer to an allocated, null-terminated C string that must be
 *         deallocated via `parcMemory_Deallocate()`.
 *
 * Example:
 * @code
 * {
 *     CCNxInterestPayloadId *instance = ccnxInterestPayloadId_CreateAsSHA256Hash(...);
 *
 *     char *string = ccnxInterestPayloadId_ToString(instance);
 *
 *     if (string != NULL) {
 *         printf("Hello: %s\n", string);
 *         parcMemory_Deallocate(string);
 *     } else {
 *         printf("Cannot allocate memory\n");
 *     }
 *
 *     ccnxInterestPayloadId_Release(&instance);
 * }
 * @endcode
 */
char *
ccnxInterestPayloadId_ToString(const CCNxInterestPayloadId *interestPayloadId);

/**
 * Create a copy of the specified `CCNxInterestPayloadId` instance, producing a new,
 * independent, instance from dynamically allocated memory. This a deep copy, all
 * referenced memory is copied. The created instance of `CCNxInterestPayloadId` must be
 * released by calling {@link ccnxInterestPayloadId_Release}().
 *
 * @param [in] interestPayloadId The `CCNxInterestPayloadId` to copy
 *
 * @return A new, independent copy of the given `CCNxInterestPayloadId`.
 *
 * Example:
 * @code
 * {
 *     CCNxInterestPayloadId *interestPayloadId = ccnxInterestPayloadId_CreateAsSHA256Hash(...);
 *     CCNxInterestPayloadId *copy = ccnxInterestPayloadId_Copy(interestPayloadId);
 *
 *     ...
 *
 *     ccnxInterestPayloadId_Release(&interestPayloadId);
 *     ccnxInterestPayloadId_Release(&copy);
 * }
 * @endcode
 */
CCNxInterestPayloadId *
ccnxInterestPayloadId_Copy(const CCNxInterestPayloadId *interestPayloadId);

/**
 * Determine if two `CCNxInterestPayloadId` instances are equal.
 *
 * The following equivalence relations on non-null `CCNxInterestPayloadId` instances are maintained:
 *
 *   * It is reflexive: for any non-null reference value x, `ccnxInterestPayloadId_Equals(x, x)` must return true.
 *
 *   * It is symmetric: for any non-null reference values x and y, `ccnxInterestPayloadId_Equals(x, y)` must return true if and only if
 *        `ccnxInterestPayloadId_Equals(y, x)` returns true.
 *
 *   * It is transitive: for any non-null reference values x, y, and z, if
 *        `ccnxInterestPayloadId_Equals(x, y)` returns true and
 *        `ccnxInterestPayloadId_Equals(y, z)` returns true,
 *        then  `ccnxInterestPayloadId_Equals(x, z)` must return true.
 *
 *   * It is consistent: for any non-null reference values x and y, multiple invocations of `ccnxInterestPayloadId_Equals(x, y)`
 *         consistently return true or consistently return false.
 *
 *   * For any non-null reference value x, `ccnxInterestPayloadId_Equals(x, NULL)` must return false.
 *
 * @param [in] interestPayloadId1 A pointer to a `CCNxInterestPayloadId` instance.
 * @param [in] interestPayloadId2 A pointer to a `CCNxInterestPayloadId` instance.
 *
 * @return True If the given `CCNxInterestPayloadId` instances are equal
 * @return False Otherwise
 *
 * Example:
 * @code
 * {
 *     CCNxInterestPayloadId *a = ccnxInterestPayloadId_CreateAsSHA256Hash(...);
 *     CCNxInterestPayloadId *b = ccnxInterestPayloadId_CreateAsSHA256Hash(...);
 *
 *     if (ccnxInterestPayloadId_Equals(a, b)) {
 *         // true
 *     } else {
 *         // false
 *      }
 *     ccnxInterestPayloadId_Release(&a);
 *     ccnxInterestPayloadId_Release(&b);
 * }
 * @endcode
 *
 * @see {@link ccnxInterestPayloadId_Compare}
 */
bool
ccnxInterestPayloadId_Equals(const CCNxInterestPayloadId *interestPayloadId1, const CCNxInterestPayloadId *interestPayloadId2);

/**
 * Compare @p interestPayloadId1 to @p interestPayloadId2 using CCNx canonical ordering (shortlex).
 *
 * `NULL` is considered the shortest interestPayloadId, so `(NULL, non-NULL) -> -1` and
 * `(NULL, NULL) -> 0`, `(non-NULL, NULL) -> +1`.
 *
 * @param [in] interestPayloadId1 A pointer to a `CCNxInterestPayloadId` instance.
 * @param [in] interestPayloadId2 A pointer to a `CCNxInterestPayloadId` instance.
 *
 * Returns:
 * <ul>
 * <li>-1 for interestPayloadId1 < interestPayloadId2</li>
 * <li> 0 for interestPayloadId1 = interestPayloadId2</li>
 * <li>+1 for interestPayloadId1 > interestPayloadId2</li>
 * </ul>
 *
 * Example:
 * @code
 * {
 *     CCNxInterestPayloadId *a = ccnxInterestPayloadId_CreateAsSHA256Hash(...);
 *     CCNxInterestPayloadId *b = ccnxInterestPayloadId_CreateAsSHA256Hash(...);
 *
 *     if (ccnxInterestPayloadId_Compare(a, b) == 0) {
 *         ...
 *     }
 *
 *     ccnxInterestPayloadId_Release(&a);
 *     ccnxInterestPayloadId_Release(&b);
 * }
 * @endcode
 *
 * @see {@link ccnxInterestPayloadId_Equals}
 */
int
ccnxInterestPayloadId_Compare(const CCNxInterestPayloadId *interestPayloadId1, const CCNxInterestPayloadId *interestPayloadId2);

/**
 * Retrieve the hashcode of the CCNxInterestPayloadId.
 *
 * @param [in] id A pointer to a `CCNxINterestPayloadId` instance.
 *
 * @return uint32_t The hashcode
 *
 * Example:
 * @code
 * {
 *     CCNxName *someName = ...
 *
 *     CCNxInterestPayloadId *interestPayloadId =
 *        ccnxInterestPayloadId_CreateFromSegmentInName(someName);
 *
 *     if (interestPayloadId != NULL) {
 *         uint32_t hashcode = ccnxInterestPayloadId_HashCode(interestPayloadId);
 *         ....
 *         ccnxInterestPayloadId_Release(&interestPayloadId);
 *     }
 *
 *     ...
 *
 *     ccnxName_Release(&someName);
 * }
 * @endcode
 */
uint32_t
ccnxInterestPayloadId_HashCode(const CCNxInterestPayloadId *id);
#endif // libccnx_ccnx_InterestPayloadId_h
