/*
 *  sendmsg.c  --  send a message to a twin client
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>
#include "version.h"

byte *argv0;

void Usage(void) {
    fprintf(stderr, "Usage: %s [--msgport=]<MsgPort> [OPTIONS]\n"
	    "Currently known options:\n"
	    " -h, --help              display this help and exit\n"
	    " -V, --version           output version information and exit\n"
	    " --twin@<dpy>            set the server to contact (default is $TWDISPLAY)\n"
	    " --control               send a MSG_USER_CONTROL message (default)\n"
	    " --clientmsg             send a MSG_USER_CLIENTMSG message\n"
	    " [--code=]<Code>         set the message code (default is `open')\n"
	    " [--data=]<Data>         set the message data\n"
	    "Currently known codes for control messages are:\n"
	    " quit (0), restart (1), open (2)\n",
	    argv0);
}

void ShowVersion(void) {
    fputs("twsendmsg " TWIN_VERSION_STR "\n", stdout);
}

TW_DECL_MAGIC(sendmsg_magic);

int main(int argc, char *argv[]) {
    byte *DisplayName = NULL, *MsgPortName = NULL, *CodeName = NULL, *Data = NULL;
    udat Type = TW_MSG_USER_CONTROL, Code = TW_MSG_CONTROL_OPEN, DataLen = 0;
    tmsgport MsgPort;
    tmsg Msg;
    uldat err;

    TwMergeHyphensArgv(argc, argv);
    
    argv0 = argv[0];
    
    if (argc == 2) {
	if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help")) {
	    Usage();
	    return 0;
	} else if (!strcmp(argv[1], "-V") || !strcmp(argv[1], "-version")) {
	    ShowVersion();
	    return 0;
	}
    }
    
    if (TwCheckMagic(sendmsg_magic)) do {
	
	while (++argv, --argc) {
	    if (!strncmp(*argv, "-msgport=", 9))
		MsgPortName = *argv + 9;
	    else if (!strncmp(*argv, "-twin@", 6))
		DisplayName = *argv + 6;
	    else if (!strcmp(*argv, "-control"))
		Type = TW_MSG_USER_CONTROL;
	    else if (!strcmp(*argv, "-clientmsg"))
		Type = TW_MSG_USER_CLIENTMSG;
	    else if (!strncmp(*argv, "-code=", 6))
		CodeName = *argv + 6;
	    else if (!strncmp(*argv, "-data=", 6))
		Data = *argv + 6;
	    else if (!MsgPortName)
		MsgPortName = *argv;
	    else if (!CodeName)
		CodeName = *argv;
	    else if (!Data)
		Data = *argv;
	}
	
	if (CodeName) {
	    if (!strcmp(CodeName, "quit"))
		Code = TW_MSG_CONTROL_QUIT;
	    else if (!strcmp(CodeName, "restart"))
		Code = TW_MSG_CONTROL_RESTART;
	    else if (!strcmp(CodeName, "open"))
		Code = TW_MSG_CONTROL_OPEN;
	    else {
		fprintf(stderr, "%s: argument `%s' not recognized\n"
			"\ttry `%s --help' for usage summary.\n", argv0, CodeName, argv0);
		return 1;
	    }
	}
	
	if (Data)
	    DataLen = strlen(Data);

	if (!TwOpen(DisplayName))
	    break;
	
	if (MsgPortName) {
	    if ((MsgPort = TwFindMsgPort(TW_NOID, strlen(MsgPortName), MsgPortName))) {
		if (Type == TW_MSG_USER_CONTROL) {
		    tevent_control EventC;
		    if ((Msg = TwCreateMsg(TW_MSG_USER_CONTROL,
					   DataLen + TW_SIZEOF_TEVENT_CONTROL))) {
			EventC = &Msg->Event.EventControl;
			EventC->W = TW_NOID;
			EventC->Code = Code;
			EventC->Len = DataLen;
			EventC->X = 0;
			EventC->Y = 0;
			if (DataLen)
			    memcpy(EventC->Data, Data, DataLen);
			if (TwSendMsg(MsgPort, Msg))
			    return 0;
		    }
		} else {
		    tevent_clientmsg EventC;
		    if ((Msg = TwCreateMsg(TW_MSG_USER_CLIENTMSG,
					   DataLen + TW_SIZEOF_TEVENT_CLIENTMSG))) {
			EventC->W = TW_NOID;
			EventC->Code = Code;
			EventC->Len = DataLen;
			if (DataLen)
			    memcpy(EventC->Data.b, Data, DataLen);
			if (TwSendMsg(MsgPort, Msg))
			    return 0;
		    }
		}
	    } else {
		fprintf(stderr, "%s: MsgPort `%s' not found on server.\n", argv0, MsgPortName);
		return 1;
	    }
	} else {
	    Usage();
	    return 1;
	}
	
    } while (0);
    
    err = TwErrno;
    fprintf(stderr, "%s: libTw error: %s%s\n", argv0,
	    TwStrError(err), TwStrErrorDetail(err, TwErrnoDetail));
    return 1;
}

