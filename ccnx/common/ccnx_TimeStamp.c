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
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <LongBow/runtime.h>
#include <parc/algol/parc_Object.h>

#include <ccnx/common/ccnx_TimeStamp.h>

#include <parc/algol/parc_Memory.h>

struct ccnx_timestamp {
    struct timespec timespec;
};

static void
_Destroy(CCNxTimeStamp **timeStampPtr)
{
}

parcObject_ExtendPARCObject(CCNxTimeStamp, _Destroy, ccnxTimeStamp_Copy,
                            ccnxTimeStamp_ToString, ccnxTimeStamp_Equals, NULL, NULL, NULL);

parcObject_ImplementAcquire(ccnxTimeStamp, CCNxTimeStamp);

parcObject_ImplementRelease(ccnxTimeStamp, CCNxTimeStamp);

void
ccnxTimeStamp_AssertValid(const CCNxTimeStamp *timeStamp)
{
    assertNotNull(timeStamp, "Parameter must be a pointer to a CCNxTimeStamp structure.");
}

CCNxTimeStamp *
ccnxTimeStamp_CreateFromCurrentUTCTime(void)
{
    struct timeval timeval;
    gettimeofday(&timeval, NULL);

    struct timespec timespec;
    timespec.tv_sec = timeval.tv_sec;
    timespec.tv_nsec = timeval.tv_usec * 1000;

    return ccnxTimeStamp_CreateFromTimespec(&timespec);
}

CCNxTimeStamp *
ccnxTimeStamp_CreateFromTimespec(const struct timespec *timespec)
{
    CCNxTimeStamp *result = parcObject_CreateInstance(CCNxTimeStamp);
    result->timespec.tv_sec = timespec->tv_sec;
    result->timespec.tv_nsec = timespec->tv_nsec;

    return result;
}

struct timespec
ccnxTimeStamp_AsTimespec(const CCNxTimeStamp *timeStamp)
{
    ccnxTimeStamp_AssertValid(timeStamp);
    return timeStamp->timespec;
}

CCNxTimeStamp *
ccnxTimeStamp_CreateFromNanosecondsSinceEpoch(uint64_t nanos)
{
    struct timespec timespec = { .tv_sec = nanos / 1000000000, .tv_nsec = (nanos % 1000000000) };

    return ccnxTimeStamp_CreateFromTimespec(&timespec);
}

CCNxTimeStamp *
ccnxTimeStamp_CreateFromMillisecondsSinceEpoch(uint64_t millis)
{
    struct timespec timespec = { .tv_sec = millis / 1000, .tv_nsec = (millis % 1000) * 1000000 };
    return ccnxTimeStamp_CreateFromTimespec(&timespec);
}

bool
ccnxTimeStamp_Equals(const CCNxTimeStamp *timeStampA, const CCNxTimeStamp *timeStampB)
{
    if (timeStampA == timeStampB) {
        return true;
    }
    if (timeStampA == NULL || timeStampB == NULL) {
        return false;
    }
    if (timeStampA->timespec.tv_sec == timeStampB->timespec.tv_sec) {
        if (timeStampA->timespec.tv_nsec == timeStampB->timespec.tv_nsec) {
            return true;
        }
    }
    return false;
}

uint64_t
ccnxTimeStamp_AsNanoSeconds(const CCNxTimeStamp *timeStamp)
{
    ccnxTimeStamp_AssertValid(timeStamp);
    uint64_t result = timeStamp->timespec.tv_sec * 1000000000ULL + timeStamp->timespec.tv_nsec;
    return result;
}

char *
ccnxTimeStamp_ToString(const CCNxTimeStamp *timeStamp)
{
    if (timeStamp == NULL) {
        return parcMemory_StringDuplicate("NULL", 4);
    }

    char *string;
    int failure = asprintf(&string, "%ld.%ld", timeStamp->timespec.tv_sec, timeStamp->timespec.tv_nsec);
    assertTrue(failure > -1, "Error asprintf");

    char *result = parcMemory_StringDuplicate(string, strlen(string));
    free(string);
    return result;
}

CCNxTimeStamp *
ccnxTimeStamp_Copy(const CCNxTimeStamp *timeStamp)
{
    ccnxTimeStamp_AssertValid(timeStamp);
    return ccnxTimeStamp_CreateFromTimespec(&timeStamp->timespec);
}
