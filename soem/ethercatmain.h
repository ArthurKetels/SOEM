/*
 * Licensed under the GNU General Public License version 2 with exceptions. See
 * LICENSE file in the project root for full license information
 */

/** \file
 * \brief
 * Headerfile for ethercatmain.c
 */

#ifndef _ethercatmain_
#define _ethercatmain_


#ifdef __cplusplus
extern "C"
{
#endif

/** max. entries in EtherCAT error list */
#define EC_MAXELIST       64
/** max. length of readable name in slavelist and Object Description List */
#define EC_MAXNAME        40
/** max. number of slaves in array */
#define EC_MAXSLAVE       200
/** max. number of groups */
#define EC_MAXGROUP       2
/** max. number of IO segments per group */
#define EC_MAXIOSEGMENTS  64
/** max. mailbox size */
#define EC_MAXMBX         1486
/** max. eeprom PDO entries */
#define EC_MAXEEPDO       0x200
/** max. SM used */
#define EC_MAXSM          8
/** max. FMMU used */
#define EC_MAXFMMU        4
/** max. Adapter */
#define EC_MAXLEN_ADAPTERNAME    128
/** define maximum number of concurrent threads in mapping */
#define EC_MAX_MAPT           1

typedef struct ec_adapter ec_adaptert;
struct ec_adapter
{
   char   name[EC_MAXLEN_ADAPTERNAME];
   char   desc[EC_MAXLEN_ADAPTERNAME];
   ec_adaptert *next;
};

/** record for FMMU */
PACKED_BEGIN
typedef struct PACKED ec_fmmu
{
   uint32  LogStart;
   uint16  LogLength;
   uint8   LogStartbit;
   uint8   LogEndbit;
   uint16  PhysStart;
   uint8   PhysStartBit;
   uint8   FMMUtype;
   uint8   FMMUactive;
   uint8   unused1;
   uint16  unused2;
}  ec_fmmut;
PACKED_END

/** record for sync manager */
PACKED_BEGIN
typedef struct PACKED ec_sm
{
   uint16  StartAddr;
   uint16  SMlength;
   uint32  SMflags;
} ec_smt;
PACKED_END

PACKED_BEGIN
typedef struct PACKED ec_state_status
{
   uint16  State;
   uint16  Unused;
   uint16  ALstatuscode;
} ec_state_status;
PACKED_END

/** mailbox buffer array */
typedef uint8 ec_mbxbuft[EC_MAXMBX + 1];

#define EC_MBXPOOLSIZE  32
#define EC_MBXINENABLE  (uint8 *)1

typedef struct
{
   int         listhead, listtail, listcount;
   int         mbxemptylist[EC_MBXPOOLSIZE];
   osal_mutext *mbxmutex;
   ec_mbxbuft  mbx[EC_MBXPOOLSIZE];
} ec_mbxpoolt;

#define EC_MBXQUEUESTATE_NONE       0
#define EC_MBXQUEUESTATE_REQ        1
#define EC_MBXQUEUESTATE_FAIL       2
#define EC_MBXQUEUESTATE_DONE       3

typedef struct
{
   int         listhead, listtail, listcount;
   ec_mbxbuft  *mbx[EC_MBXPOOLSIZE];
   int         mbxstate[EC_MBXPOOLSIZE];
   int         mbxremove[EC_MBXPOOLSIZE];
   int         mbxticket[EC_MBXPOOLSIZE];
   uint16      mbxslave[EC_MBXPOOLSIZE];
   osal_mutext *mbxmutex;
} ec_mbxqueuet;

#define VOE_SCOPEMAXCHANNELS    6
#define EC_MAXSCOPESLAVE        6
#define EC_SCOPECHANNELS        6
#define EC_SCOPEBUFFERSIZE      4069

typedef struct
{
    int             channels;
    int             redpos, writepos, counter;
    double          sampletime;
    double          fdata[EC_SCOPECHANNELS][EC_SCOPEBUFFERSIZE];
} ec_ringscopet;

typedef struct
{
   uint16         slave;
   uint16         channel;
} ec_channelassignt;

typedef struct ec_scope
{
   uint16            scopeslave[EC_MAXSCOPESLAVE];
   int               scopeslavecnt;
   ec_channelassignt channelassign[EC_SCOPECHANNELS];
   ec_ringscopet     ringscope;
} ec_scopet;

#define ECT_MBXPROT_AOE      0x0001
#define ECT_MBXPROT_EOE      0x0002
#define ECT_MBXPROT_COE      0x0004
#define ECT_MBXPROT_FOE      0x0008
#define ECT_MBXPROT_SOE      0x0010
#define ECT_MBXPROT_VOE      0x0020

#define ECT_COEDET_SDO       0x01
#define ECT_COEDET_SDOINFO   0x02
#define ECT_COEDET_PDOASSIGN 0x04
#define ECT_COEDET_PDOCONFIG 0x08
#define ECT_COEDET_UPLOAD    0x10
#define ECT_COEDET_SDOCA     0x20

#define EC_SMENABLEMASK      0xfffeffff

#define ECT_MBXH_NONE         0
#define ECT_MBXH_CYCLIC       1
#define ECT_MBXH_LOST         2

/** for list of ethercat slaves detected */
typedef struct ec_slave
{
   /** state of slave */
   uint16           state;
   /** AL status code */
   uint16           ALstatuscode;
   /** Configured address */
   uint16           configadr;
   /** Alias address */
   uint16           aliasadr;
   /** Manufacturer from EEprom */
   uint32           eep_man;
   /** ID from EEprom */
   uint32           eep_id;
   /** revision from EEprom */
   uint32           eep_rev;
   /** serial number from EEprom */
   uint32           eep_ser;
   /** Interface type */
   uint16           Itype;
   /** Device type */
   uint16           Dtype;
   /** output bits */
   uint16           Obits;
   /** output bytes, if Obits < 8 then Obytes = 0 */
   uint32           Obytes;
   /** output pointer in IOmap buffer */
   uint8            *outputs;
   /** output offset in IOmap buffer */
   uint32           Ooffset;
   /** startbit in first output byte */
   uint8            Ostartbit;
   /** input bits */
   uint16           Ibits;
   /** input bytes, if Ibits < 8 then Ibytes = 0 */
   uint32           Ibytes;
   /** input pointer in IOmap buffer */
   uint8            *inputs;
   /** input offset in IOmap buffer */
   uint32           Ioffset;
   /** startbit in first input byte */
   uint8            Istartbit;
   /** SM structure */
   ec_smt           SM[EC_MAXSM];
   /** SM type 0=unused 1=MbxWr 2=MbxRd 3=Outputs 4=Inputs */
   uint8            SMtype[EC_MAXSM];
   /** FMMU structure */
   ec_fmmut         FMMU[EC_MAXFMMU];
   /** FMMU0 function 0=unused 1=outputs 2=inputs 3=SM status*/
   uint8            FMMU0func;
   /** FMMU1 function */
   uint8            FMMU1func;
   /** FMMU2 function */
   uint8            FMMU2func;
   /** FMMU3 function */
   uint8            FMMU3func;
   /** length of write mailbox in bytes, if no mailbox then 0 */
   uint16           mbx_l;
   /** mailbox write offset */
   uint16           mbx_wo;
   /** length of read mailbox in bytes */
   uint16           mbx_rl;
   /** mailbox read offset */
   uint16           mbx_ro;
   /** mailbox supported protocols */
   uint16           mbx_proto;
   /** Counter value of mailbox link layer protocol 1..7 */
   uint8            mbx_cnt;
   /** has DC capability */
   boolean          hasdc;
   /** Physical type; Ebus, EtherNet combinations */
   uint8            ptype;
   /** topology: 1 to 3 links */
   uint8            topology;
   /** active ports bitmap : ....3210 , set if respective port is active **/
   uint8            activeports;
   /** consumed ports bitmap : ....3210, used for internal delay measurement **/
   uint8            consumedports;
   /** slave number for parent, 0=master */
   uint16           parent;
   /** port number on parent this slave is connected to **/
   uint8            parentport;
   /** port number on this slave the parent is connected to **/
   uint8            entryport;
   /** DC receivetimes on port A */
   int32            DCrtA;
   /** DC receivetimes on port B */
   int32            DCrtB;
   /** DC receivetimes on port C */
   int32            DCrtC;
   /** DC receivetimes on port D */
   int32            DCrtD;
   /** propagation delay */
   int32            pdelay;
   /** next DC slave */
   uint16           DCnext;
   /** previous DC slave */
   uint16           DCprevious;
   /** DC cycle time in ns */
   int32            DCcycle;
   /** DC shift from clock modulus boundary */
   int32            DCshift;
   /** DC sync activation, 0=off, 1=on */
   uint8            DCactive;
   /** link to config table */
   uint16           configindex;
   /** link to SII config */
   uint16           SIIindex;
   /** 1 = 8 bytes per read, 0 = 4 bytes per read */
   uint8            eep_8byte;
   /** 0 = eeprom to master , 1 = eeprom to PDI */
   uint8            eep_pdi;
   /** CoE details */
   uint8            CoEdetails;
   /** FoE details */
   uint8            FoEdetails;
   /** EoE details */
   uint8            EoEdetails;
   /** SoE details */
   uint8            SoEdetails;
   /** E-bus current */
   int16            Ebuscurrent;
   /** if >0 block use of LRW in processdata */
   uint8            blockLRW;
   /** group */
   uint8            group;
   /** first unused FMMU */
   uint8            FMMUunused;
   /** Boolean for tracking whether the slave is (not) responding, not used/set by the SOEM library */
   boolean          islost;
   /** registered configuration function PO->SO */
   int              (*PO2SOconfig)(uint16 slave);
   /** mailbox handler state, 0 = no handler, 1 = cyclic task mbx handler, 2 = slave lost */
   int              mbxhandlerstate; 
   /** mailbox handler robust mailbox protocol state */
   int              mbxrmpstate;
   /** mailbox handler RMP extended mbx in state */
   uint16           mbxinstateex; 
   /** pointer to CoE mailbox in buffer */
   uint8            *coembxin;
   /** CoE mailbox in flag, true = mailbox full */
   boolean          coembxinfull;   
   /** CoE mailbox in overrun counter */
   int              coembxoverrun; 
   /** pointer to SoE mailbox in buffer */
   uint8            *soembxin;
   /** SoE mailbox in flag, true = mailbox full */
   boolean          soembxinfull;   
   /** SoE mailbox in overrun counter */
   int              soembxoverrun; 
   /** pointer to FoE mailbox in buffer */
   uint8            *foembxin;
   /** FoE mailbox in flag, true = mailbox full */
   boolean          foembxinfull;   
   /** FoE mailbox in overrun counter */
   int              foembxoverrun; 
   /** pointer to EoE mailbox in buffer */
   uint8            *eoembxin;
   /** EoE mailbox in flag, true = mailbox full */
   boolean          eoembxinfull;   
   /** EoE mailbox in overrun counter */
   int              eoembxoverrun; 
   /** pointer to VoE mailbox in buffer */
   uint8            *voembxin;
   /** VoE mailbox in flag, true = mailbox full */
   boolean          voembxinfull;   
   /** VoE mailbox in overrun counter */
   int              voembxoverrun; 
   /** pointer to AoE mailbox in buffer */
   uint8            *aoembxin;
   /** AoE mailbox in flag, true = mailbox full */
   boolean          aoembxinfull;   
   /** AoE mailbox in overrun counter */
   int              aoembxoverrun; 
   /** pointer to out mailbox status register buffer */
   uint8            *mbxstatus;
   /** readable name */
   char             name[EC_MAXNAME + 1];
} ec_slavet;

/** for list of ethercat slave groups */
typedef struct ec_group
{
   /** logical start address for this group */
   uint32           logstartaddr;
   /** output bytes, if Obits < 8 then Obytes = 0 */
   uint32           Obytes;
   /** output pointer in IOmap buffer */
   uint8            *outputs;
   /** input bytes, if Ibits < 8 then Ibytes = 0 */
   uint32           Ibytes;
   /** input pointer in IOmap buffer */
   uint8            *inputs;
   /** has DC capabillity */
   boolean          hasdc;
   /** next DC slave */
   uint16           DCnext;
   /** E-bus current */
   int16            Ebuscurrent;
   /** if >0 block use of LRW in processdata */
   uint8            blockLRW;
   /** IO segegments used */
   uint16           nsegments;
   /** 1st input segment */
   uint16           Isegment;
   /** Offset in input segment */
   uint16           Ioffset;
   /** Expected workcounter outputs */
   uint16           outputsWKC;
   /** Expected workcounter inputs */
   uint16           inputsWKC;
   /** check slave states */
   boolean          docheckstate;
   /** IO segmentation list. Datagrams must not break SM in two. */
   uint32           IOsegment[EC_MAXIOSEGMENTS];
   /** pointer to out mailbox status register buffer */
   uint8            *mbxstatus;
   /** mailbox status register buffer length */
   int32            mbxstatuslength;
   /** mailbox status lookup table */
   uint16           mbxstatuslookup[EC_MAXSLAVE];
   /** mailbox last handled in mxbhandler */
   uint16           lastmbxpos;
   /** mailbox  transmit queue struct */
   ec_mbxqueuet     mbxtxqueue; 
   /** pointer to group scope variables */
   ec_scopet        *scopevar; 
} ec_groupt;

/** SII FMMU structure */
typedef struct ec_eepromFMMU
{
   uint16  Startpos;
   uint8   nFMMU;
   uint8   FMMU0;
   uint8   FMMU1;
   uint8   FMMU2;
   uint8   FMMU3;
} ec_eepromFMMUt;

/** SII SM structure */
typedef struct ec_eepromSM
{
   uint16  Startpos;
   uint8   nSM;
   uint16  PhStart;
   uint16  Plength;
   uint8   Creg;
   uint8   Sreg;       /* dont care */
   uint8   Activate;
   uint8   PDIctrl;      /* dont care */
} ec_eepromSMt;

/** record to store rxPDO and txPDO table from eeprom */
typedef struct ec_eepromPDO
{
   uint16  Startpos;
   uint16  Length;
   uint16  nPDO;
   uint16  Index[EC_MAXEEPDO];
   uint16  SyncM[EC_MAXEEPDO];
   uint16  BitSize[EC_MAXEEPDO];
   uint16  SMbitsize[EC_MAXSM];
} ec_eepromPDOt;

/** standard ethercat mailbox header */
PACKED_BEGIN
typedef struct PACKED ec_mbxheader
{
   uint16  length;
   uint16  address;
   uint8   priority;
   uint8   mbxtype;
} ec_mbxheadert;
PACKED_END

/** ALstatus and ALstatus code */
PACKED_BEGIN
typedef struct PACKED ec_alstatus
{
   uint16  alstatus;
   uint16  unused;
   uint16  alstatuscode;
} ec_alstatust;
PACKED_END

/** stack structure to store segmented LRD/LWR/LRW constructs */
typedef struct ec_idxstack
{
   uint8   pushed;
   uint8   pulled;
   uint8   idx[EC_MAXBUF];
   void    *data[EC_MAXBUF];
   uint16  length[EC_MAXBUF];
   uint8   type[EC_MAXBUF];    
} ec_idxstackT;

/** ringbuf for error storage */
typedef struct ec_ering
{
   int16     head;
   int16     tail;
   ec_errort Error[EC_MAXELIST + 1];
} ec_eringt;

/** SyncManager Communication Type structure for CA */
PACKED_BEGIN
typedef struct PACKED ec_SMcommtype
{
   uint8   n;
   uint8   nu1;
   uint8   SMtype[EC_MAXSM];
} ec_SMcommtypet;
PACKED_END

/** SDO assign structure for CA */
PACKED_BEGIN
typedef struct PACKED ec_PDOassign
{
   uint8   n;
   uint8   nu1;
   uint16  index[256];
} ec_PDOassignt;
PACKED_END

/** SDO description structure for CA */
PACKED_BEGIN
typedef struct PACKED ec_PDOdesc
{
   uint8   n;
   uint8   nu1;
   uint32  PDO[256];
} ec_PDOdesct;
PACKED_END

/** Context structure , referenced by all ecx functions*/
typedef struct ecx_context
{
   /** port reference, may include red_port */
   ecx_portt      *port;
   /** slavelist reference */
   ec_slavet      *slavelist;
   /** number of slaves found in configuration */
   int            *slavecount;
   /** maximum number of slaves allowed in slavelist */
   int            maxslave;
   /** grouplist reference */
   ec_groupt      *grouplist;
   /** maximum number of groups allowed in grouplist */
   int            maxgroup;
   /** internal, reference to eeprom cache buffer */
   uint8          *esibuf;
   /** internal, reference to eeprom cache map */
   uint32         *esimap;
   /** internal, current slave for eeprom cache */
   uint16         esislave;
   /** internal, reference to error list */
   ec_eringt      *elist;
   /** internal, reference to processdata stack buffer info */
   ec_idxstackT   *idxstack;
   /** reference to ecaterror state */
   boolean        *ecaterror;
   /** internal, position of DC datagram in process data packet */
   uint16         DCtO;
   /** internal, length of DC datagram */
   uint16         DCl;
   /** reference to last DC time from slaves */
   int64          *DCtime;
   /** internal, SM buffer */
   ec_SMcommtypet *SMcommtype;
   /** internal, PDO assign list */
   ec_PDOassignt  *PDOassign;
   /** internal, PDO description list */
   ec_PDOdesct    *PDOdesc;
   /** internal, SM list from eeprom */
   ec_eepromSMt   *eepSM;
   /** internal, FMMU list from eeprom */
   ec_eepromFMMUt *eepFMMU;
   /** internal, mailbox pool */
   ec_mbxpoolt    *mbxpool;
   /** registered FoE hook */
   int            (*FOEhook)(uint16 slave, int packetnumber, int datasize);
} ecx_contextt;

#ifdef EC_VER1
/** global struct to hold default master context */
extern ecx_contextt  ecx_context;
/** main slave data structure array */
extern ec_slavet   ec_slave[EC_MAXSLAVE];
/** number of slaves found by configuration function */
extern int         ec_slavecount;
/** slave group structure */
extern ec_groupt   ec_group[EC_MAXGROUP];
extern boolean     EcatError;
extern int64       ec_DCtime;
extern ec_mbxpoolt ec_mbxpool;  

void ec_pusherror(const ec_errort *Ec);
boolean ec_poperror(ec_errort *Ec);
boolean ec_iserror(void);
void ec_packeterror(uint16 Slave, uint16 Index, uint8 SubIdx, uint16 ErrorCode);
int ec_init(const char * ifname);
int ec_init_redundant(const char *ifname, char *if2name);
void ec_close(void);
uint8 ec_siigetbyte(uint16 slave, uint16 address);
int16 ec_siifind(uint16 slave, uint16 cat);
void ec_siistring(char *str, uint16 slave, uint16 Sn);
uint16 ec_siiFMMU(uint16 slave, ec_eepromFMMUt* FMMU);
uint16 ec_siiSM(uint16 slave, ec_eepromSMt* SM);
uint16 ec_siiSMnext(uint16 slave, ec_eepromSMt* SM, uint16 n);
int ec_siiPDO(uint16 slave, ec_eepromPDOt* PDO, uint8 t);
int ec_readstate(void);
int ec_writestate(uint16 slave);
uint16 ec_statecheck(uint16 slave, uint16 reqstate, int timeout);
int ec_mbxempty(uint16 slave, int timeout);
int ec_mbxsend(uint16 slave,ec_mbxbuft *mbx, int timeout);
int ec_mbxreceive(uint16 slave, ec_mbxbuft *mbx, int timeout);
void ec_esidump(uint16 slave, uint8 *esibuf);
uint32 ec_readeeprom(uint16 slave, uint16 eeproma, int timeout);
int ec_writeeeprom(uint16 slave, uint16 eeproma, uint16 data, int timeout);
int ec_eeprom2master(uint16 slave);
int ec_eeprom2pdi(uint16 slave);
uint64 ec_readeepromAP(uint16 aiadr, uint16 eeproma, int timeout);
int ec_writeeepromAP(uint16 aiadr, uint16 eeproma, uint16 data, int timeout);
uint64 ec_readeepromFP(uint16 configadr, uint16 eeproma, int timeout);
int ec_writeeepromFP(uint16 configadr, uint16 eeproma, uint16 data, int timeout);
void ec_readeeprom1(uint16 slave, uint16 eeproma);
uint32 ec_readeeprom2(uint16 slave, int timeout);
int ec_send_processdata_group(uint8 group);
int ec_send_overlap_processdata_group(uint8 group);
int ec_receive_processdata_group(uint8 group, int timeout);
int ec_send_processdata(void);
int ec_send_overlap_processdata(void);
int ec_receive_processdata(int timeout);
#endif

ec_adaptert * ec_find_adapters(void);
void ec_free_adapters(ec_adaptert * adapter);
uint8 ec_nextmbxcnt(uint8 cnt);
void ec_clearmbx(ec_mbxbuft *Mbx);
void ecx_pusherror(ecx_contextt *context, const ec_errort *Ec);
boolean ecx_poperror(ecx_contextt *context, ec_errort *Ec);
boolean ecx_iserror(ecx_contextt *context);
void ecx_packeterror(ecx_contextt *context, uint16 Slave, uint16 Index, uint8 SubIdx, uint16 ErrorCode);
int ecx_init(ecx_contextt *context, const char * ifname);
int ecx_init_redundant(ecx_contextt *context, ecx_redportt *redport, const char *ifname, char *if2name);
void ecx_close(ecx_contextt *context);
uint8 ecx_siigetbyte(ecx_contextt *context, uint16 slave, uint16 address);
int16 ecx_siifind(ecx_contextt *context, uint16 slave, uint16 cat);
void ecx_siistring(ecx_contextt *context, char *str, uint16 slave, uint16 Sn);
uint16 ecx_siiFMMU(ecx_contextt *context, uint16 slave, ec_eepromFMMUt* FMMU);
uint16 ecx_siiSM(ecx_contextt *context, uint16 slave, ec_eepromSMt* SM);
uint16 ecx_siiSMnext(ecx_contextt *context, uint16 slave, ec_eepromSMt* SM, uint16 n);
int ecx_siiPDO(ecx_contextt *context, uint16 slave, ec_eepromPDOt* PDO, uint8 t);
int ecx_readstate(ecx_contextt *context);
int ecx_writestate(ecx_contextt *context, uint16 slave);
uint16 ecx_statecheck(ecx_contextt *context, uint16 slave, uint16 reqstate, int timeout);
int ecx_mbxhandler(ecx_contextt *context, uint8 group, int limit);
int ecx_mbxempty(ecx_contextt *context, uint16 slave, int timeout);
int ecx_mbxsend(ecx_contextt *context, uint16 slave,ec_mbxbuft *mbx, int timeout);
//int ecx_mbxreceive(ecx_contextt *context, uint16 slave, ec_mbxbuft *mbx, int timeout);
int ecx_mbxreceive2(ecx_contextt *context, uint16 slave, ec_mbxbuft **mbx, int timeout);
void ecx_esidump(ecx_contextt *context, uint16 slave, uint8 *esibuf);
uint32 ecx_readeeprom(ecx_contextt *context, uint16 slave, uint16 eeproma, int timeout);
int ecx_writeeeprom(ecx_contextt *context, uint16 slave, uint16 eeproma, uint16 data, int timeout);
int ecx_eeprom2master(ecx_contextt *context, uint16 slave);
int ecx_eeprom2pdi(ecx_contextt *context, uint16 slave);
uint64 ecx_readeepromAP(ecx_contextt *context, uint16 aiadr, uint16 eeproma, int timeout);
int ecx_writeeepromAP(ecx_contextt *context, uint16 aiadr, uint16 eeproma, uint16 data, int timeout);
uint64 ecx_readeepromFP(ecx_contextt *context, uint16 configadr, uint16 eeproma, int timeout);
int ecx_writeeepromFP(ecx_contextt *context, uint16 configadr, uint16 eeproma, uint16 data, int timeout);
void ecx_readeeprom1(ecx_contextt *context, uint16 slave, uint16 eeproma);
uint32 ecx_readeeprom2(ecx_contextt *context, uint16 slave, int timeout);
int ecx_send_overlap_processdata_group(ecx_contextt *context, uint8 group);
int ecx_receive_processdata_group(ecx_contextt *context, uint8 group, int timeout);
int ecx_send_processdata(ecx_contextt *context);
int ecx_send_overlap_processdata(ecx_contextt *context);
int ecx_receive_processdata(ecx_contextt *context, int timeout);
ec_mbxbuft *ecx_getmbx(ecx_contextt *context);
int ecx_dropmbx(ecx_contextt *context, ec_mbxbuft *mbx);
int ecx_initmbxpool(ecx_contextt *context);
int ecx_initmbxqueue(ecx_contextt *context, uint16 group);
int ecx_slavembxcyclic(ecx_contextt *context, uint16 slave);

#ifdef __cplusplus
}
#endif

#endif
