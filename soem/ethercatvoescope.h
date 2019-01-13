/*
 * Licensed under the GNU General Public License version 2 with exceptions. See
 * LICENSE file in the project root for full license information
 */

/** \file
 * \brief
 * Headerfile for ethercatvoescope.c
 */

#ifndef _ethercatvoescope_
#define _ethercatvoescope_

#ifdef __cplusplus
extern "C"
{
#endif

#define VOE_SCOPETYPE           0x10

#define VOE_SCOPEMAXCHANNELS    6
#define EC_MAXSCOPESLAVE        6
#define EC_SCOPECHANNELS        6
#define EC_SCOPENAMELENGTH      16
#define EC_SCOPEBUFFERSIZE      200000

typedef struct ec_ringscopes
{
    int             tailpos, headpos, counter;
    char            name[EC_SCOPENAMELENGTH];
    double          tailtime, headtime, sampletime;
    double          fdata[EC_SCOPEBUFFERSIZE];
} ec_ringscopet;

typedef struct ec_channelassigns
{
   uint16         slave;
   uint16         channel;
} ec_channelassignt;

typedef struct ec_scopes
{
   int               scopeslavecnt;
   uint16            scopeslave[EC_MAXSCOPESLAVE];
   int               channelcount;
   ec_channelassignt channelassign[EC_SCOPECHANNELS];
   ec_ringscopet     ringscope[EC_SCOPECHANNELS];
} ec_scopet;

extern ec_scopet               *ec_scopevar;

int ecx_scopeinit(ecx_contextt *context, uint8 group);
int ecx_scopeclose(ecx_contextt *context, uint8 group);
int ecx_scopeenableslave(ecx_contextt *context, uint16 slave);
int ecx_scopedisableslave(ecx_contextt *context, uint16 slave);

#ifdef __cplusplus
}
#endif

#endif
