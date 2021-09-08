#ifndef __FTNPKT_H__
#define __FTNPKT_H__

/* Raw Type 2 FidoNet Packet Header; see FTS-0001 */

typedef struct
{
    unsigned char origNode[2];
    unsigned char destNode[2];
    unsigned char year[2];
    unsigned char month[2];
    unsigned char day[2];
    unsigned char hour[2];
    unsigned char minute[2];
    unsigned char second[2];
    unsigned char baud[2];
    unsigned char packetType[2];
    unsigned char origNet[2];
    unsigned char destNet[2];
    unsigned char prodCode;
    unsigned char serialNo;
    unsigned char password[8];
    unsigned char origZone[2];
    unsigned char destZone[2];
    unsigned char fill[20];
}
raw_fido_pkt_hdr_t2;


/* Raw Type 2+ FidoNet Packet Header; see FSC-0039 */

typedef struct
{
    unsigned char origNode[2];
    unsigned char destNode[2];
    unsigned char year[2];
    unsigned char month[2];
    unsigned char day[2];
    unsigned char hour[2];
    unsigned char minute[2];
    unsigned char second[2];
    unsigned char baud[2];
    unsigned char packetType[2];
    unsigned char origNet[2];
    unsigned char destNet[2];
    unsigned char prodCodeLo;
    unsigned char prodRevMaj;
    unsigned char password[8];
    unsigned char QMOrigZone[2];
    unsigned char QMDestZone[2];
    unsigned char fill[2];
    unsigned char capValid[2];
    unsigned char prodCodeHi;
    unsigned char prodRevMin;
    unsigned char capWord[2];
    unsigned char origZone[2];
    unsigned char destZone[2];
    unsigned char origPoint[2];
    unsigned char destPoint[2];
    unsigned char prodData[4];
}
raw_fido_pkt_hdr_t2p;


/* Raw Type 2.2 FidoNet Packet Header; see FSC-0045 */

typedef struct
{
    unsigned char origNode[2];
    unsigned char destNode[2];
    unsigned char origPoint[2];
    unsigned char destPoint[2];
    unsigned char fill[8];
    unsigned char pktSubVer[2];
    unsigned char pktVer[2];
    unsigned char origNet[2];
    unsigned char destNet[2];
    unsigned char prodCode;
    unsigned char prodRev;
    unsigned char password[8];
    unsigned char origZone[2];
    unsigned char destZone[2];
    unsigned char origDomain[8];
    unsigned char destDomain[8];
    unsigned char prodData[4];
}
raw_fido_pkt_hdr_t2pt2;


/* Raw Type 2.N FidoNet Packet Header; see FSC-0048 */

typedef struct
{
    unsigned char origNode[2];
    unsigned char destNode[2];
    unsigned char year[2];
    unsigned char month[2];
    unsigned char day[2];
    unsigned char hour[2];
    unsigned char minute[2];
    unsigned char second[2];
    unsigned char baud[2];
    unsigned char packetType[2];
    unsigned char origNet[2];
    unsigned char destNet[2];
    unsigned char prodCodeLo;
    unsigned char prodRevMaj;
    unsigned char password[8];
    unsigned char QMOrigZone[2];
    unsigned char QMDestZone[2];
    unsigned char auxNet[2];
    unsigned char capValid[2];
    unsigned char prodCodeHi;
    unsigned char prodRevMin;
    unsigned char capWord[2];
    unsigned char origZone[2];
    unsigned char destZone[2];
    unsigned char origPoint[2];
    unsigned char destPoint[2];
    unsigned char prodData[4];
}
raw_fido_pkt_hdr_t2ptN;


/* Raw Type 2 FidoNet Packed Message Header; see FTS-0001 */

typedef struct
{
    unsigned char packetType[2];
    unsigned char origNode[2];
    unsigned char destNode[2];
    unsigned char origNet[2];
    unsigned char destNet[2];
    unsigned char Attribute[2];
    unsigned char cost[2];
    unsigned char DateTime[20];
}
raw_fido_pkd_msg_hdr_t2;

#endif

/* __FTNPKT_H__ */
