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
 * @file ccnxCodec_EncodingBuffer
 * @brief An encoding buffer is a zero-copy vectored I/O for PARCBuffers
 *
 * An Encoding Buffer is an ordered list of PARCBuffers that can be written by functions like
 * writev().  You can append and append to the list and the buffers are stored only by reference.
 *
 * You can also append one encoding buffer to another.  In this case, the buffers are moved from
 * the previous list to the end of the new list.
 *
 * @code
 * {
 *    PARCBuffer *name = parcBuffer_Wrap("marc", 4, 0, 4);
 *    PARCBuffer *space= parcBuffer_Wrap(" ", 1, 0 ,1);
 *    PARCBuffer *email= parcBuffer_Wrap("<marc@example.com>", 18, 0, 18);
 *
 *    CCNxCodecEncodingBuffer *encodingBuffer = ccnxCodecEncodingBuffer_Create();
 *    ccnxCodecEncodingBuffer_BufferInsertTail(encodingBuffer, name);
 *    ccnxCodecEncodingBuffer_BufferInsertTail(encodingBuffer, space);
 *    parcBuffer_Release(&space);
 *    parcBuffer_Release(&name);
 *
 *    CCNxCodecEncodingBuffer *emailBuffer = ccnxCodecEncodingBuffer_Create();
 *    ccnxCodecEncodingBuffer_BufferInsertTail(emailBuffer, email);
 *    parcBuffer_Release(&email);
 *
 *    ccnxCodecEncodingBuffer_MoveToTail(encodingBuffer, emailBuffer);
 *    ccnxCodecEncodingBuffer_Release(&emailBuffer);
 *
 *    CCNxCodecEncodingBufferIOVec *iov = ccnxCodecEncodingBuffer_CreateIOVec(encodingBuffer);
 *    writev(STDOUT_FILENO, iov->iov, iov->iovcnt);
 *    ccnxCodecEncodingBufferIOVec_Release(&iov);
 *
 *    ccnxCodecEncodingBuffer_Release(&encodingBuffer);
 * }
 * @endcode
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef libccnx_ccnxCodec_EncodingBuffer_h
#define libccnx_ccnxCodec_EncodingBuffer_h

#include <sys/uio.h>
#include <parc/algol/parc_Buffer.h>

struct ccnx_codec_encoding_buffer;
typedef struct ccnx_codec_encoding_buffer CCNxCodecEncodingBuffer;

/**
 * @typedef CCNxCodecEncodingBufferIOVec
 * @abstract Used for writev() or similar functions
 * @constant encodingBuffer A reference counted copy of the encoding Buffer
 * @constant iov An allocated array of iovec entries
 * @constant iovcnt The number of array entries
 * @discussion <#Discussion#>
 */
typedef struct ccnx_tlv_encoding_buffer_iovec {
    CCNxCodecEncodingBuffer *encodingBuffer;
    int iovcnt;
    struct iovec iov[];
} CCNxCodecEncodingBufferIOVec;

// ======================================================================================

/**
 * Creates an empty encoding buffer
 *
 * <#Paragraphs Of Explanation#>
 *
 * @return non-null An allocated encoding buffer
 * @return null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxCodecEncodingBuffer *ccnxCodecEncodingBuffer_Create(void);

/**
 * Returns a reference counted copy
 *
 * Caller must call ccnxCodecEncodingBuffer_Release() on all copies.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return non-null A reference counted copy
 * @return null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxCodecEncodingBuffer *ccnxCodecEncodingBuffer_Acquire(const CCNxCodecEncodingBuffer *encodingBuffer);


/**
 * Release the encoding buffer and all internal references
 *
 * will release the list and release our reference to all enclosed PARCBuffers
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void ccnxCodecEncodingBuffer_Release(CCNxCodecEncodingBuffer **listBufferPtr);

/**
 * Displays the structure of the encoding buffer to STDOUT
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void ccnxCodecEncodingBuffer_Display(const CCNxCodecEncodingBuffer *encodingBuffer, int indentation);

/**
 * Appends a PARCBuffer to the encoding buffer
 *
 * Appends to the encoding buffer a reference count to the given buffer.
 * The return value is the storage node used in the internal data structure.
 *
 * The buffer will be used from its position at the time of use (i.e. when
 * ccnxCodecEncodingBuffer_CreateIOVec() is called).  It is important that no other
 * use of the PARCBuffer move the Position.
 *
 * @param [in] encodingBuffer The buffer to append to
 * @param [in] bufferToInsert The PARCBuffer to insert at the tail of the encoding buffer.
 *
 * @return number The position in the encoding buffer list
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
size_t ccnxCodecEncodingBuffer_AppendBuffer(CCNxCodecEncodingBuffer *encodingBuffer, PARCBuffer *bufferToInsert);
size_t ccnxCodecEncodingBuffer_PrependBuffer(CCNxCodecEncodingBuffer *encodingBuffer, PARCBuffer *bufferToPrepend);

/**
 * Puts the value in scratch memory
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void ccnxTlvEncodingbuffer_AppendUint16(CCNxCodecEncodingBuffer *encodingBuffer, uint16_t value);

/**
 * The number of elements in the list
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
size_t ccnxCodecEncodingBuffer_Size(const CCNxCodecEncodingBuffer *encodingBuffer);

/**
 * The total number of bytes in the list
 *
 * This is calculated as the sum of all PARCBuffer Remaining lengths in
 * the encoding buffer.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
size_t ccnxCodecEncodingBuffer_Length(const CCNxCodecEncodingBuffer *encodingBuffer);

// ======================================================================================

/**
 * Constructs an iovec array based on the buffers in the list
 *
 * The elements of the iovec array will be in the list order.
 * Each iovec entry will point to the backing array of each PARCBuffer
 * based on that buffers current position.
 *
 * This object contains a reference counted copy to the encoding buffer, so
 * the caller can release the encoding buffer and hold on to only this object
 * until the writev (or similar function) is done.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return non-null The allocated IOVec structure
 * @return null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxCodecEncodingBufferIOVec *ccnxCodecEncodingBuffer_CreateIOVec(CCNxCodecEncodingBuffer *encodingBuffer);

/**
 * Constructs an iovec array based on the buffers in the list that cooresponds to offset and length
 *
 * The elements of the iovec array will be in the list order.
 * Each iovec entry will point to the backing array of each PARCBuffer
 * based on that buffers current position.
 *
 * This object contains a reference counted copy to the encoding buffer, so
 * the caller can release the encoding buffer and hold on to only this object
 * until the writev (or similar function) is done.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return non-null The allocated IOVec structure
 * @return null An error, or the specified offset/length is not contained in the extent
 
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxCodecEncodingBuffer *ccnxCodecEncodingBuffer_Slice(CCNxCodecEncodingBuffer *encodingBuffer, size_t offset, size_t length);

/**
 * Release the iovec object.
 *
 * This will release the IOVec object and release its reference to the encoding
 * buffer.  If this was the last reference to the encoding buffer, all references to
 * the underlying PARCBuffers will be released too.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void ccnxCodecEncodingBufferIOVec_Release(CCNxCodecEncodingBufferIOVec **iovecPtr);

#endif // libccnx_ccnxCodec_EncodingBuffer_h
