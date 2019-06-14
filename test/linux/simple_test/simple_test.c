/** \file
 * \brief Example code for Simple Open EtherCAT master
 *
 * Usage : simple_test [ifname1]
 * ifname is NIC interface, f.e. eth0
 *
 * This is a minimal test.
 *
 * (c)Arthur Ketels 2010 - 2011
 */
#include <stdio.h>
#include <string.h>
//#include <Mmsystem.h>

#include "osal.h"
#include "ethercat.h"

#define NSEC_PER_SEC 1000000000
#define EC_TIMEOUTMON 500

#define SMARTWHEELID 0x1705011

char IOmap[4096];
OSAL_THREAD_HANDLE thread1, thread2;
int expectedWKC;
boolean needlf;
volatile int wkc;
volatile int rtcnt;
boolean inOP;
uint8 currentgroup = 0;
int64 toff;

/* add ns to timespec */
void add_timespec(struct timespec *ts, int64 addtime)
{
   int64 sec, nsec;

   nsec = addtime % NSEC_PER_SEC;
   sec = (addtime - nsec) / NSEC_PER_SEC;
   ts->tv_sec += sec;
   ts->tv_nsec += nsec;
   if ( ts->tv_nsec > NSEC_PER_SEC )
   {
      nsec = ts->tv_nsec % NSEC_PER_SEC;
      ts->tv_sec += (ts->tv_nsec - nsec) / NSEC_PER_SEC;
      ts->tv_nsec = nsec;
   }
}

/* PI calculation to get linux time synced to DC time */
void ec_sync(int64 reftime, int64 cycletime , int64 *offsettime)
{
   static int64 integral = 0;
   int64 delta;
   /* set linux sync point 50us later than DC sync, just as example */
   delta = (reftime - 50000) % cycletime;
   if(delta> (cycletime / 2)) { delta= delta - cycletime; }
   if(delta>0){ integral++; }
   if(delta<0){ integral--; }
   *offsettime = -(delta / 100) - (integral / 20);
}


/* RT EtherCAT thread */
OSAL_THREAD_FUNC_RT ecatthread(void)
{
   struct timespec   ts, tleft;
   int ht;
   int64 cycletime;

   clock_gettime(CLOCK_MONOTONIC, &ts);
   ht = (ts.tv_nsec / 1000000) + 1; /* round to nearest ms */
   ts.tv_nsec = ht * 1000000;
   cycletime = 1000 * 1000; /* cycletime in ns */
   toff = 0;
   while(1)
   {
      /* calculate next cycle start */
      add_timespec(&ts, cycletime + toff);
      /* wait to cycle start */
      clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, &tleft);
      ec_send_processdata();
      wkc = ec_receive_processdata(500);
      ecx_mbxhandler(&ecx_context, 0, 4);
      rtcnt++;
      if (ec_slave[0].hasdc)
      {
         /* calulate toff to get linux time and DC synced */
         ec_sync(ec_DCtime, cycletime, &toff);
      }
   }
}

int EL7031setup(uint16 slave)
{
    int retval;
//    uint16 u16val;

    // map velocity
    uint16 map_1c12[4] = {0x0003, 0x1601, 0x1602, 0x1604};
    uint16 map_1c13[3] = {0x0002, 0x1a01, 0x1a03};

    retval = 0;

    // Set PDO mapping using Complete Access
    // Strange, writing CA works, reading CA doesn't
    // This is a protocol error of the slave.
    retval += ec_SDOwrite(slave, 0x1c12, 0x00, TRUE, sizeof(map_1c12), &map_1c12, EC_TIMEOUTSAFE);
    retval += ec_SDOwrite(slave, 0x1c13, 0x00, TRUE, sizeof(map_1c13), &map_1c13, EC_TIMEOUTSAFE);

    // bug in EL7031 old firmware, CompleteAccess for reading is not supported even if the slave says it is.
    ec_slave[slave].CoEdetails &= ~ECT_COEDET_SDOCA;

    // set some motor parameters, just as example
//    u16val = 1200; // max motor current in mA
//    retval += ec_SDOwrite(slave, 0x8010, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTSAFE);
//    u16val = 150; // motor coil resistance in 0.01ohm
//    retval += ec_SDOwrite(slave, 0x8010, 0x04, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTSAFE);

    // set other nescessary parameters as needed
    // .....

    while(EcatError) printf("%s", ec_elist2string());

    printf("EL7031 slave %d set, retval = %d\n", slave, retval);
    return 1;
}

int AEPsetup(uint16 slave)
{
    int retval;
    uint8 u8val;
    uint16 u16val;

    retval = 0;

    u8val = 0;
    retval += ec_SDOwrite(slave, 0x1c12, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    u16val = 0x1603;
    retval += ec_SDOwrite(slave, 0x1c12, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);
    u8val = 1;
    retval += ec_SDOwrite(slave, 0x1c12, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    u8val = 0;
    retval += ec_SDOwrite(slave, 0x1c13, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
    u16val = 0x1a03;
    retval += ec_SDOwrite(slave, 0x1c13, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTRXM);
    u8val = 1;
    retval += ec_SDOwrite(slave, 0x1c13, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    u8val = 8;
    retval += ec_SDOwrite(slave, 0x6060, 0x00, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);

    // set some motor parameters, just as example
//    u16val = 1200; // max motor current in mA
//    retval += ec_SDOwrite(slave, 0x8010, 0x01, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTSAFE);
//    u16val = 150; // motor coil resistance in 0.01ohm
//    retval += ec_SDOwrite(slave, 0x8010, 0x04, FALSE, sizeof(u16val), &u16val, EC_TIMEOUTSAFE);

    // set other nescessary parameters as needed
    // .....

    while(EcatError) printf("%s", ec_elist2string());

    printf("AEP slave %d set, retval = %d\n", slave, retval);
    return 1;
}

void simpletest(char *ifname)
{
    int i, j, oloop, iloop, chk, slc, psize;
    uint32 prodcode;
 //   uint8 u8dummy;

    needlf = FALSE;
    inOP = FALSE;

   printf("Starting simple test 2\n");

   /* initialise SOEM, bind socket to ifname */
   if (ec_init(ifname))
   {
      printf("ec_init on %s succeeded.\n",ifname);
      /* find and auto-config slaves */


       if ( ec_config_init(FALSE) > 0 )
       {
            printf("mbxqueue mutex:%p\n\r",ecx_context.grouplist[0].mbxtxqueue.mbxmutex);
         printf("%d slaves found and configured.\n",ec_slavecount);

         if((ec_slavecount > 1))
         {
             for(slc = 1; slc <= ec_slavecount; slc++)
             {
                 // beckhoff EL7031, using ec_slave[].name is not very reliable
                 if((ec_slave[slc].eep_man == 0x00000002) && (ec_slave[slc].eep_id == 0x1b773052))
                 {
                     printf("Found %s at position %d\n", ec_slave[slc].name, slc);
                     // link slave specific setup to preop->safeop hook
                     ec_slave[slc].PO2SOconfig = &EL7031setup;
                 }
                 // Copley Controls EAP, using ec_slave[].name is not very reliable
                 if((ec_slave[slc].eep_man == 0x000000ab) && (ec_slave[slc].eep_id == 0x00000380))
                 {
                     printf("Found %s at position %d\n", ec_slave[slc].name, slc);
                     // link slave specific setup to preop->safeop hook
                     ec_slave[slc].PO2SOconfig = &AEPsetup;
                 }
             }
         }

         ec_config_map(&IOmap);

         ec_configdc();

         printf("Slaves mapped, state to SAFE_OP.\n");
         /* wait for all slaves to reach SAFE_OP state */
         ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);

         oloop = ec_slave[0].Obytes;
         if ((oloop == 0) && (ec_slave[0].Obits > 0)) oloop = 1;
         if (oloop > 8) oloop = 8;
         iloop = ec_slave[0].Ibytes;
         if ((iloop == 0) && (ec_slave[0].Ibits > 0)) iloop = 1;
         if (iloop > 8) iloop = 8;

         printf("segments : %d : %d %d %d %d\n",ec_group[0].nsegments ,ec_group[0].IOsegment[0],ec_group[0].IOsegment[1],ec_group[0].IOsegment[2],ec_group[0].IOsegment[3]);

         printf("Request operational state for all slaves\n");
         expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
         printf("Calculated workcounter %d\n", expectedWKC);
         ec_slave[0].state = EC_STATE_OPERATIONAL;

         /* start RT thread */
         osal_thread_create_rt(&thread2, 64 * 1024 * 2, &ecatthread, NULL);

         ecx_scopeinit(&ecx_context, 0);
         uint16 mbxsl = 0;
         for(i = 1; i <= ec_slavecount; i++)
         {
            if(ec_slave[i].mbx_l > 0)
            {
               if(!mbxsl) mbxsl = i;
               ecx_slavembxcyclic(&ecx_context, i);
               if(ec_slave[i].eep_id == SMARTWHEELID)
               {
                  printf("Found %s at position %d\n", ec_slave[i].name, i);
                  ecx_scopeenableslave(&ecx_context, i);
                  uint8 u8val = 0x02;
                  ec_SDOwrite(i, 0x8f00, 0x01, FALSE, sizeof(u8val), &u8val, EC_TIMEOUTRXM);
               }
            }
         }

         /* request OP state for all slaves */
         ec_writestate(0);
         chk = 200;
         /* wait for all slaves to reach OP state */ 
         do 
         {
            ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
         }
         while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));
         if (ec_slave[0].state == EC_STATE_OPERATIONAL )
         {
            printf("Operational state reached for all slaves.\n");
            inOP = TRUE;


            /* cyclic loop, reads data from RT thread */
            for(i = 1; i <= 500; i++)
            {
                //    if(wkc >= expectedWKC)
                    {
                        printf("Processdata cycle %4d, WKC %d , O:", rtcnt, wkc);

                        for(j = 0 ; j < oloop; j++)
                        {
                            printf(" %2.2x", *(ec_slave[0].outputs + j));
                        }

                        printf(" I:");
                        for(j = 0 ; j < iloop; j++)
                        {
                            printf(" %2.2x", *(ec_slave[0].inputs + j));
                        }
                        printf(" T:%ld\r\n",ec_DCtime);
                        //needlf = TRUE;
                    }
                    prodcode = 0;
                    psize = sizeof(prodcode);

                    if(ecx_SDOread(&ecx_context, mbxsl, 0x1018, 0x02, FALSE, &psize, &prodcode, 5000) > 0)
                    {
                       printf("Prodcode %d %8.8x %d\r\n", mbxsl, prodcode, ecx_context.mbxpool->listcount);
                    }
                    else
                    {
                       printf("missed %4.4x %d\r\n",i, ecx_context.mbxpool->listcount);
                    }
                    
                    osal_usleep(50000);

            }
            inOP = FALSE;
         }
         else
         {
                printf("Not all slaves reached operational state.\n");
                ec_readstate();
                for(i = 1; i<=ec_slavecount ; i++)
                {
                    if(ec_slave[i].state != EC_STATE_OPERATIONAL)
                    {
                        printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                            i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                    }
                }
         }

         printf("\nRequest init state for all slaves\n");
         ec_slave[0].state = EC_STATE_INIT;
         /* request INIT state for all slaves */
         ec_writestate(0);
        }
        else
        {
            printf("No slaves found!\n");
        }
        printf("End simple test, close socket\n");
        /* stop SOEM, close socket */
        ec_close();
    }
    else
    {
        printf("No socket connection on %s\nExcecute as root\n",ifname);
    }
}

OSAL_THREAD_FUNC ecatcheck(void)
{
    int slave;

    while(1)
    {
        if( inOP && ((wkc < expectedWKC) || ec_group[currentgroup].docheckstate))
        {
            if (needlf)
            {
               needlf = FALSE;
               printf("\n");
            }
            /* one ore more slaves are not responding */
            ec_group[currentgroup].docheckstate = FALSE;
            ec_readstate();
            for (slave = 1; slave <= ec_slavecount; slave++)
            {
               if ((ec_slave[slave].group == currentgroup) && (ec_slave[slave].state != EC_STATE_OPERATIONAL))
               {
                  ec_group[currentgroup].docheckstate = TRUE;
                  if (ec_slave[slave].state == (EC_STATE_SAFE_OP + EC_STATE_ERROR))
                  {
                     printf("ERROR : slave %d is in SAFE_OP + ERROR, attempting ack.\n", slave);
                     ec_slave[slave].state = (EC_STATE_SAFE_OP + EC_STATE_ACK);
                     ec_writestate(slave);
                  }
                  else if(ec_slave[slave].state == EC_STATE_SAFE_OP)
                  {
                     printf("WARNING : slave %d is in SAFE_OP, change to OPERATIONAL.\n", slave);
                     ec_slave[slave].state = EC_STATE_OPERATIONAL;
                     if(ec_slave[slave].mbxhandlerstate == ECT_MBXH_LOST) ec_slave[slave].mbxhandlerstate = ECT_MBXH_CYCLIC;
                     ec_writestate(slave);
                  }
                  else if(ec_slave[slave].state > EC_STATE_NONE)
                  {
                     if (ec_reconfig_slave(slave, EC_TIMEOUTMON) >= EC_STATE_PRE_OP)
                     {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d reconfigured\n",slave);
                     }
                  }
                  else if(!ec_slave[slave].islost)
                  {
                     /* re-check state */
                     ec_statecheck(slave, EC_STATE_OPERATIONAL, EC_TIMEOUTRET);
                     if (ec_slave[slave].state == EC_STATE_NONE)
                     {
                        ec_slave[slave].islost = TRUE;
                        ec_slave[slave].mbxhandlerstate = ECT_MBXH_LOST;
                        /* zero input data for this slave */
                        if(ec_slave[slave].Ibytes)
                        {
                           memset(ec_slave[slave].inputs, 0x00, ec_slave[slave].Ibytes);
                           printf("zero inputs %p %d\n\r", ec_slave[slave].inputs, ec_slave[slave].Ibytes);
                        }
                        printf("ERROR : slave %d lost\n",slave);
                     }
                  }
               }
               if (ec_slave[slave].islost)
               {
                  if(ec_slave[slave].state <= EC_STATE_INIT)
                  {
                     if (ec_recover_slave(slave, EC_TIMEOUTMON))
                     {
                        ec_slave[slave].islost = FALSE;
                        printf("MESSAGE : slave %d recovered\n",slave);
                     }
                  }
                  else
                  {
                     ec_slave[slave].islost = FALSE;
                     printf("MESSAGE : slave %d found\n",slave);
                  }
               }
            }
            if(!ec_group[currentgroup].docheckstate)
               printf("OK : all slaves resumed OPERATIONAL.\n");
        }
        osal_usleep(10000);
    }
}

char ifbuf[1024];

int main(int argc, char *argv[])
{
   ec_adaptert * adapter = NULL;
   printf("SOEM (Simple Open EtherCAT Master)\nSimple test 2\n");

   if (argc > 1)
   {
      /* create thread to handle slave error handling in OP */
      osal_thread_create(&thread1, 128000, &ecatcheck, NULL);
      strcpy(ifbuf, argv[1]);
      /* start cyclic part */
      simpletest(ifbuf);
   }
   else
   {
      printf("Usage: simple_test ifname1\n");
   	/* Print the list */
      printf ("Available adapters\n");
      adapter = ec_find_adapters ();
      while (adapter != NULL)
      {
         printf ("Description : %s, Device to use for wpcap: %s\n", adapter->desc,adapter->name);
         adapter = adapter->next;
      }
   }

   printf("End program\n");
   return (0);
}
