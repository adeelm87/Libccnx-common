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
 * @file ccnx_NameSegmentNumber.h
 * @ingroup Naming
 * @brief Utilities for creating and parsing CCNxNameSegments that contain an integer value.
 *
 * CCNxNameSegments may optionally contain an integer value. This file contains some utilities
 * for creating and parsing CCNxNameSegments with integer values.
 * See {@link CCNxNameSegment}, {@link CCNxNameLabelType}, and {@link CCNxName} for more information.
 *
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef libccnx_ccnx_NameSegmentNumber_h
#define libccnx_ccnx_NameSegmentNumber_h

#include <stdbool.h>

#include <ccnx/common/ccnx_NameSegment.h>
#include <ccnx/common/ccnx_NameLabel.h>

/**
 * Create a new {@link CCNxNameSegment} consisting of a type and integer value.
 *
 * The newly created instance must eventually be released by calling {@link ccnxNameSegment_Release}().
 *
 * @param [in] type A valid {@link CCNxNameLabelType}.
 * @param [in] value The integer value to assign to the `CCNxNameSegment`.
 * @return A pointer to a new `CCNxNameSegment` instance, consisting of a type and integer value.
 *
 * Example:
 * @code
 * {
 *     uint64_t expected = 0x123456789ABCDEF0;
 *     CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);
 *
 *     uint64_t actual = ccnxNameSegmentNumber_Value(segment);
 *
 *     assertTrue(expected == actual, "Expected 0x%" PRIX64 " actual 0x%" PRIX64 "", expected, actual);
 *
 *     ccnxNameSegment_Release(&segment);
 * }
 * @endcode
 *
 * @see {@link ccnxNameSegment_Release}
 * @see `CCNxNameLabelType`
 */
CCNxNameSegment *ccnxNameSegmentNumber_Create(CCNxNameLabelType type, uint64_t value);

/**
 * Decode an integer value from a {@link CCNxNameSegment}.
 *
 * Given a `CCNxNameSegment` with a numeric type, return the integer value associated with the `CCNxNameSegment`.
 *
 * @param [in] nameSegment A pointer a `CCNxNameSegment` instance containing an integer value.
 * @return The integer value of the `CCNxNameSegment`.
 *
 * Example:
 * @code
 * {
 *     uint64_t expected = 0x123456789ABCDEF0;
 *     CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);
 *
 *     uint64_t actual = ccnxNameSegmentNumber_Value(segment);
 *
 *     assertTrue(expected == actual, "Expected 0x%" PRIX64 " actual 0x%" PRIX64 "", expected, actual);
 *
 *     ccnxNameSegment_Release(&segment);
 * }
 * @endcode
 */
uint64_t ccnxNameSegmentNumber_Value(const CCNxNameSegment *nameSegment);

/**
 * Determine if the given CCNxNameSegment value represents a valid encoded number.
 *
 * A valid encoded number contains at least 1 byte and nor more than 8 bytes.
 *
 * @param [in] nameSegment A pointer to a CCNxNameSegment instance.
 *
 * Example:
 * @code
 * {
 *     uint64_t expected = 0x123456789ABCDEF0;
 *     CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);
 *
 *     ccnxNameSegmentNumber_AssertValid(segment);
 * }
 * @endcode
 */
bool ccnxNameSegmentNumber_IsValid(const CCNxNameSegment *nameSegment);

/**
 * Assert that the given CCNxNameSegment value represents a valid encoded number.
 *
 * @param [in] nameSegment A pointer to a CCNxNameSegment instance.
 *
 * Example:
 * @code
 * {
 *     uint64_t expected = 0x123456789ABCDEF0;
 *     CCNxNameSegment *segment = ccnxNameSegmentNumber_Create(CCNxNameLabelType_CHUNK, expected);
 *
 *     ccnxNameSegmentNumber_AssertValid(segment);
 * }
 * @endcode
 */
void ccnxNameSegmentNumber_AssertValid(const CCNxNameSegment *nameSegment);
#endif // libccnx_ccnx_NameSegmentNumber_h
