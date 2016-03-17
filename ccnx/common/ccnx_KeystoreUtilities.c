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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include <pwd.h>
#include <unistd.h>

#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>

#include <sys/param.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>
#include <ccnx/common/ccnx_KeystoreUtilities.h>
#include <parc/security/parc_PublicKeySignerPkcs12Store.h>
#include <parc/security/parc_Signer.h>


struct keystore_params {
    char filename[1024];
    char password[1024];
    PARCSigner *signer;
};

#define OPT_KEYSTORE 'k'
#define OPT_PASSWORD 'p'
#define OPT_BITS     'b'
#define OPT_DAYS     'y'

static const char *emptyPassword = "";

static char *
ccnxKeystoreUtilities_ConstructPath(const char *dir, const char *file)
{
    char *path = parcMemory_AllocateAndClear(MAXPATHLEN);
    assertNotNull(path, "parcMemory_AllocateAndClear(%u) returned NULL", MAXPATHLEN);
    strncpy(path, dir, MAXPATHLEN);
    strncat(path, "/", MAXPATHLEN);
    strncat(path, file, MAXPATHLEN);
    return path;
}

static char *
ccnxKeystoreUtilities_HomeDirectoryFromEnv()
{
    char *home = getenv("HOME");
    if (home != NULL) {
        home = parcMemory_StringDuplicate(home, strlen(home) + 1);
    }
    return home;
}

static char *
ccnxKeystoreUtilities_HomeDirectoryFromPasswd()
{
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    return parcMemory_StringDuplicate(homedir, strlen(homedir) + 1);
}

static KeystoreParams *
ccnxKeystoreUtilities_OpenFromPath(const char *path, const char *password)
{
    KeystoreParams *params = NULL;

    // If the file exists, try to open it as a keystore
    struct stat filestat;
    int failure = stat(path, &filestat);
    if (!failure) {
        PARCSigningInterface *interface = parcPublicKeySignerPkcs12Store_Open(path, password, PARC_HASH_SHA256);
        if (interface) {
            params = ccnxKeystoreUtilities_Create(interface, path, password);
        }
    }

    return params;
}

static KeystoreParams *
ccnxKeystoreUtilities_CreateInPath(const char *path, const char *password, int keystoreBits, int keystoreDays)
{
    KeystoreParams *params = NULL;

    bool success = parcPublicKeySignerPkcs12Store_CreateFile(path, password, "ccnxuser", keystoreBits, keystoreDays);
    if (success) {
        PARCSigningInterface *interface = parcPublicKeySignerPkcs12Store_Open(path, password, PARC_HASH_SHA256);
        if (interface) {
            params = ccnxKeystoreUtilities_Create(interface, path, password);
        }
    }
    return params;
}


static char *
ccnxKeystoreUtilities_GetHomeDirectory()
{
    char *homedir = ccnxKeystoreUtilities_HomeDirectoryFromEnv();
    if (homedir == NULL) {
        homedir = ccnxKeystoreUtilities_HomeDirectoryFromPasswd();
    }
    return homedir;
}

static KeystoreParams *
ccnxKeystoreUtilities_OpenFromHomeDirectory(const char *password)
{
    KeystoreParams *params = NULL;

    char *homedir = ccnxKeystoreUtilities_GetHomeDirectory();
    char *ccnxdir = ccnxKeystoreUtilities_ConstructPath(homedir, ".ccnx");


    char *path = ccnxKeystoreUtilities_ConstructPath(ccnxdir, ".ccnx_keystore.p12");
    params = ccnxKeystoreUtilities_OpenFromPath(path, password);
    parcMemory_Deallocate((void **) &path);

    if (params == NULL) {
        // try the older filename
        char *path = ccnxKeystoreUtilities_ConstructPath(ccnxdir, ".ccnx_keystore");
        params = ccnxKeystoreUtilities_OpenFromPath(path, password);
        parcMemory_Deallocate((void **) &path);
    }

    parcMemory_Deallocate((void **) &ccnxdir);
    parcMemory_Deallocate((void **) &homedir);
    return params;
}

static KeystoreParams *
ccnxKeystoreUtilities_CreateInHomeDirectory(const char *keystorePassword, int keystoreBits, int keystoreDays)
{
    char *homedir = ccnxKeystoreUtilities_GetHomeDirectory();
    char *ccnxdir = ccnxKeystoreUtilities_ConstructPath(homedir, ".ccnx");
    // Needs to check the return value to ensure that it was created. See case 1003.
    mkdir(ccnxdir, 0700);

    char *path = ccnxKeystoreUtilities_ConstructPath(ccnxdir, ".ccnx_keystore.p12");
    KeystoreParams *params = ccnxKeystoreUtilities_CreateInPath(path, keystorePassword, keystoreBits, keystoreDays);

    parcMemory_Deallocate((void **) &path);
    parcMemory_Deallocate((void **) &ccnxdir);
    parcMemory_Deallocate((void **) &homedir);
    return params;
}

/**
 * Read a password, then zero the static memory
 *
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return An allocated string, use <code>parcMemory_Deallocate()</code> on it.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
static char *
_readPassword(const char *prompt)
{
    char *staticBuffer = getpass(prompt);
    char *password = parcMemory_StringDuplicate(staticBuffer, strlen(staticBuffer) + 1);
    memset(staticBuffer, 0, strlen(staticBuffer));
    return password;
}

KeystoreParams *
ccnxKeystoreUtilities_OpenFile(const char *keystoreFile, const char *keystorePassword)
{
    if (keystorePassword == NULL) {
        keystorePassword = emptyPassword;
    }

    KeystoreParams *params = NULL;

    if (keystoreFile == NULL) {
        params = ccnxKeystoreUtilities_OpenFromHomeDirectory(keystorePassword);
    } else {
        params = ccnxKeystoreUtilities_OpenFromPath(keystoreFile, keystorePassword);
    }

    return params;
}

KeystoreParams *
ccnxKeystoreUtilities_CreateFile(const char *keystoreFile, const char *keystorePassword, int keystoreBits, int keystoreDays)
{
    if (keystorePassword == NULL) {
        keystorePassword = emptyPassword;
    }

    KeystoreParams *params = NULL;

    if (keystoreFile == NULL) {
        params = ccnxKeystoreUtilities_CreateInHomeDirectory(keystorePassword, keystoreBits, keystoreDays);
    } else {
        params = ccnxKeystoreUtilities_CreateInPath(keystoreFile, keystorePassword, keystoreBits, keystoreDays);
    }

    return params;
}

KeystoreParams *
ccnxKeystoreUtilities_Create(PARCSigningInterface *interface, const char *path, const char *password)
{
    KeystoreParams *params = parcMemory_AllocateAndClear(sizeof(KeystoreParams));
    assertNotNull(params, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(KeystoreParams));
    params->signer = parcSigner_Create(interface);
    strncpy(params->filename, path, 1024);
    strncpy(params->password, password, 1024);
    return params;
}

void
keystoreParams_Destroy(KeystoreParams **paramsPtr)
{
    KeystoreParams *params = *paramsPtr;
    if (params->signer != NULL) {
        parcSigner_Release(&params->signer);
    }
    parcMemory_Deallocate((void **) &params);
    *paramsPtr = NULL;
}

char *
ccnxKeystoreUtilities_ReadPassword(void)
{
    return _readPassword("Password: ");
}

bool
ccnxKeystoreUtilities_ConfirmPassword(const char *mustEqualPassword)
{
    bool equals = false;

    char *b = _readPassword("Confirm  : ");
    if (strcmp(mustEqualPassword, b) == 0) {
        equals = true;
    }

    memset(b, 0, strlen(b));
    parcMemory_Deallocate((void **) &b);
    return equals;
}

const char *
ccnxKeystoreUtilities_GetFileName(const KeystoreParams *params)
{
    return params->filename;
}

const char *
ccnxKeystoreUtilities_GetPassword(const KeystoreParams *params)
{
    return params->password;
}
