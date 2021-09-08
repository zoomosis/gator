#include <stdio.h>
#include <string.h>

char *show_address(unsigned short zone, unsigned short net,
                   unsigned short node, unsigned short point, char *domain)
{
    static char str[100];
    char tmpstr[10];

    *(str) = '\0';

    if (zone != 0)
    {
        sprintf(tmpstr, "%hd:", zone);
        strcat(str, tmpstr);
    }

    sprintf(tmpstr, "%hd/%hd", net, node);
    strcat(str, tmpstr);

    if (point != 0)
    {
        sprintf(tmpstr, ".%hd", point);
        strcat(str, tmpstr);
    }

    if (domain != NULL && *(domain) != '\0')
    {
        strcat(str, "@");
        strcat(str, domain);
    }

    return str;
}

char *show_4d(unsigned short zone, unsigned short net,
	      unsigned short node, unsigned short point)
{
    static char str[50];
    char tmpstr[10];

    *(str) = '\0';

    if (zone != 0)
    {
        sprintf(tmpstr, "%hd:", zone);
        strcat(str, tmpstr);
    }

    sprintf(tmpstr, "%hd/%hd", net, node);
    strcat(str, tmpstr);

    if (point != 0)
    {
        sprintf(tmpstr, ".%hd", point);
        strcat(str, tmpstr);
    }

    return str;
}

char *show_3d(unsigned short zone, unsigned short net, unsigned short node)
{
    static char str[50];
    char tmpstr[10];

    *(str) = '\0';

    if (zone != 0)
    {
        sprintf(tmpstr, "%hd:", zone);
        strcat(str, tmpstr);
    }

    sprintf(tmpstr, "%hd/%hd", net, node);
    strcat(str, tmpstr);
    return str;
}

char *show_2d(unsigned short net, unsigned short node)
{
    static char str[50];
    sprintf(str, "%hd/%hd", net, node);
    return str;
}

char *show_product(unsigned short prod_code)
{
    int i;

    struct product_t
    {
        unsigned short code;
        char *pproduct;
    };

    static struct product_t product[] =
    {
        {0x0000, "Fido/Unknown"},
        {0x0001, "Rover"},
        {0x0002, "SEAdog"},
        {0x0003, "WinDog"},
        {0x0004, "Slick-150"},
        {0x0005, "Opus"},
        {0x0006, "Dutchie"},
        {0x0007, "WPL Library"},
        {0x0008, "Tabby"},
        {0x0009, "SWMail"},
        {0x000A, "Wolf-68k"},
        {0x000B, "QMM"},
        {0x000C, "FrontDoor"},
        {0x000D, "GOmail"},
        {0x000E, "FFGate"},
        {0x000F, "FileMgr"},
        {0x0010, "FIDZERCP"},
        {0x0011, "MailMan"},
        {0x0012, "OOPS"},
        {0x0013, "GS-Point"},
        {0x0014, "BGMail"},
        {0x0015, "ComMotion/2"},
        {0x0016, "OurBBS Fidomailer"},
        {0x0017, "FidoPcb"},
        {0x0018, "WimpLink"},
        {0x0019, "BinkScan"},
        {0x001A, "D'Bridge"},
        {0x001B, "BinkleyTerm"},
        {0x001C, "Yankee"},
        {0x001D, "uuGate"},
        {0x001E, "Daisy"},
        {0x001F, "Polar Bear"},
        {0x0020, "The-Box"},
        {0x0021, "STARgate/2"},
        {0x0022, "TMail"},
        {0x0023, "TCOMMail"},
        {0x0024, "GIGO"},
        {0x0025, "RBBSMail"},
        {0x0026, "Apple-Netmail"},
        {0x0027, "Chameleon"},
        {0x0028, "Majik Board"},
        {0x0029, "QM"},
        {0x002A, "Point And Click"},
        {0x002B, "Aurora Three Bundler"},
        {0x002C, "FourDog"},
        {0x002D, "MSG-PACK"},
        {0x002E, "AMAX"},
        {0x002F, "Domain Communication System"},
        {0x0030, "LesRobot"},
        {0x0031, "Rose"},
        {0x0032, "Paragon"},
        {0x0033, "BinkleyTerm/oMMM/ST"},
        {0x0034, "StarNet"},
        {0x0035, "ZzyZx"},
        {0x0036, "QEcho"},
        {0x0037, "BOOM"},
        {0x0038, "PBBS"},
        {0x0039, "TrapDoor"},
        {0x003A, "Welmat"},
        {0x003B, "NetGate"},
        {0x003C, "Odie"},
        {0x003D, "Quick Gimme"},
        {0x003E, "dbLink"},
        {0x003F, "TosScan"},
        {0x0040, "Beagle"},
        {0x0041, "Igor"},
        {0x0042, "TIMS"},
        {0x0043, "Phoenix"},
        {0x0044, "FrontDoor APX"},
        {0x0045, "XRS"},
        {0x0046, "Juliet Mail System"},
        {0x0047, "Jabberwocky"},
        {0x0048, "XST"},
        {0x0049, "MailStorm"},
        {0x004A, "BIX-Mail"},
        {0x004B, "IMAIL"},
        {0x004C, "FTNGate"},
        {0x004D, "RealMail"},
        {0x004E, "Lora-CBIS"},
        {0x004F, "TDCS"},
        {0x0050, "InterMail"},
        {0x0051, "RFD"},
        {0x0052, "Yuppie!"},
        {0x0053, "EMMA"},
        {0x0054, "QBoxMail"},
        {0x0055, "Number 4"},
        {0x0056, "Number 5"},
        {0x0057, "GSBBS"},
        {0x0058, "Merlin"},
        {0x0059, "TPCS"},
        {0x005A, "Raid"},
        {0x005B, "Outpost"},
        {0x005C, "Nizze"},
        {0x005D, "Armadillo"},
        {0x005E, "rfmail"},
        {0x005F, "Msgtoss"},
        {0x0060, "InfoTex"},
        {0x0061, "GEcho"},
        {0x0062, "CDEhost"},
        {0x0063, "Pktize"},
        {0x0064, "PC-RAIN"},
        {0x0065, "Truffle"},
        {0x0066, "Foozle"},
        {0x0067, "White Pointer"},
        {0x0068, "GateWorks"},
        {0x0069, "Portal of Power"},
        {0x006A, "MacWoof"},
        {0x006B, "Mosaic"},
        {0x006C, "TPBEcho"},
        {0x006D, "HandyMail"},
        {0x006E, "EchoSmith"},
        {0x006F, "FileHost"},
        {0x0070, "SFTS"},
        {0x0071, "Benjamin"},
        {0x0072, "RiBBS"},
        {0x0073, "MP"},
        {0x0074, "Ping"},
        {0x0075, "Door2Europe"},
        {0x0076, "SWIFT"},
        {0x0077, "WMAIL"},
        {0x0078, "RATS"},
        {0x0079, "Harry the Dirty Dog"},
        {0x007A, "Squish"},
        {0x007B, "SwifEcho"},
        {0x007C, "GCChost"},
        {0x007D, "RPX-Mail"},
        {0x007E, "Tosser"},
        {0x007F, "TCL"},
        {0x0080, "MsgTrack"},
        {0x0081, "FMail"},
        {0x0082, "Scantoss"},
        {0x0083, "Point Manager"},
        {0x0084, "IMBINK"},
        {0x0085, "Simplex"},
        {0x0086, "UMTP"},
        {0x0087, "Indaba"},
        {0x0088, "Echomail Engine"},
        {0x0089, "DragonMail"},
        {0x008A, "Prox"},
        {0x008B, "Tick"},
        {0x008C, "RA-Echo"},
        {0x008D, "TrapToss"},
        {0x008E, "Babel"},
        {0x008F, "UMS"},
        {0x0090, "RWMail"},
        {0x0091, "WildMail"},
        {0x0092, "AlMAIL"},
        {0x0093, "XCS"},
        {0x0094, "Fone-Link"},
        {0x0095, "Dogfigh"},
        {0x0096, "Ascan"},
        {0x0097, "FastMail"},
        {0x0098, "DoorMan"},
        {0x0099, "PhaedoZap"},
        {0x009A, "SCREAM"},
        {0x009B, "MoonMail"},
        {0x009C, "Backdoor"},
        {0x009D, "MailLink"},
        {0x009E, "Mail Manager"},
        {0x009F, "Black Star"},
        {0x00A0, "Bermuda"},
        {0x00A1, "PT"},
        {0x00A2, "UltiMail"},
        {0x00A3, "GMD"},
        {0x00A4, "FreeMail"},
        {0x00A5, "Meliora"},
        {0x00A6, "Foodo"},
        {0x00A7, "MSBBS"},
        {0x00A8, "Boston BBS"},
        {0x00A9, "XenoMail"},
        {0x00AA, "XenoLink"},
        {0x00AB, "ObjectMatrix"},
        {0x00AC, "Milquetoast"},
        {0x00AD, "PipBase"},
        {0x00AE, "EzyMail"},
        {0x00AF, "FastEcho"},
        {0x00B0, "IOS"},
        {0x00B1, "Communique"},
        {0x00B2, "PointMail"},
        {0x00B3, "Harvey's Robot"},
        {0x00B4, "2daPoint"},
        {0x00B5, "CommLink"},
        {0x00B6, "fronttoss"},
        {0x00B7, "SysopPoint"},
        {0x00B8, "PTMAIL"},
        {0x00B9, "MHS"},
        {0x00BA, "DLGMail"},
        {0x00BB, "GatePrep"},
        {0x00BC, "Spoint"},
        {0x00BD, "TurboMail"},
        {0x00BE, "FXMAIL"},
        {0x00BF, "NextBBS"},
        {0x00C0, "EchoToss"},
        {0x00C1, "SilverBox"},
        {0x00C2, "MBMail"},
        {0x00C3, "SkyFreq"},
        {0x00C4, "ProMailer"},
        {0x00C5, "Mega Mail"},
        {0x00C6, "YaBom"},
        {0x00C7, "TachEcho"},
        {0x00C8, "XAP"},
        {0x00C9, "EZMAIL"},
        {0x00CA, "Arc-Binkley"},
        {0x00CB, "Roser"},
        {0x00CC, "UU2"},
        {0x00CD, "NMS"},
        {0x00CE, "BBCSCAN"},
        {0x00CF, "XBBS"},
        {0x00D0, "LoTek Vzrul"},
        {0x00D1, "Private Point Project"},
        {0x00D2, "NoSnail"},
        {0x00D3, "SmlNet"},
        {0x00D4, "STIR"},
        {0x00D5, "RiscBBS"},
        {0x00D6, "Hercules"},
        {0x00D7, "AMPRGATE"},
        {0x00D8, "BinkEMSI"},
        {0x00D9, "EditMsg"},
        {0x00DA, "Roof"},
        {0x00DB, "QwkPkt"},
        {0x00DC, "MARISCAN"},
        {0x00DD, "NewsFlash"},
        {0x00DE, "Paradise"},
        {0x00DF, "DogMatic-ACB"},
        {0x00E0, "T-Mail"},
        {0x00E1, "JetMail"},
        {0x00E2, "MainDoor"},
        {0x00E3, "Starnet Products"},
        {0x00E4, "BMB"},
        {0x00E5, "BNP"},
        {0x00E6, "MailMaster"},
        {0x00E7, "Mail Manager +Plus+"},
        {0x00E8, "BloufGate"},
        {0x00E9, "CrossPoint"},
        {0x00EA, "DeltaEcho"},
        {0x00EB, "ALLFIX"},
        {0x00EC, "NetWay"},
        {0x00ED, "MARSmail"},
        {0x00EE, "ITRACK"},
        {0x00EF, "GateUtil"},
        {0x00F0, "Bert"},
        {0x00F1, "Techno"},
        {0x00F2, "AutoMail"},
        {0x00F3, "April"},
        {0x00F4, "Amanda"},
        {0x00F5, "NmFwd"},
        {0x00F6, "FileScan"},
        {0x00F7, "FredMail"},
        {0x00F8, "TP Kom"},
        {0x00F9, "FidoZerb"},
        {0x00FA, "!!MessageBase"},
        {0x00FB, "EMFido"},
        {0x00FC, "GS-Toss"},
        {0x00FD, "QWKDoor"},
        {0x0000, NULL}
    };

    for (i = 0; product[i].pproduct != NULL; i++)
    {
        if (prod_code == product[i].code)
	{
            return product[i].pproduct;
	}
    }

    return "Unknown";
}
