//---------------------------------------------------------------------------


#define OCI_IMPORT_RUNTIME
#define OCI_CHARSET_ANSI
#define override
#define OCI_API __stdcall

#include "ocilib.h"
#include <stdio.h>


void log_(const char* msg) {
    printf(msg); 
    printf("\n");
}
void log2_(const char* msg, int n) {
    printf(msg); 
    printf("%d\n", n);
}

/* requires script demo/queue.sql */

void err_handler(OCI_Error* err)
{
    //    print("%s\n", OCI_ErrorGetString(err));
    log_("err");
    log_(OCI_ErrorGetString(err));
}


// _JMS_HEADER

#define MESSAGE_QUEUE_TABLE "message_queue_table7"
#define MESSAGE_QUEUE_TYPE "SYS.AQ$_JMS_BYTES_MESSAGE"
#define MESSAGE_QUEUE "message_queue7"

#define tSTRING 0

char blob_example[10 * 1024] = { 0 };
char raw_example[10 * 1024] = { 0 };

void init_blob_()
{

    int i;
    char* digits = "0123456789";
    log_("point1.1");

    for (i = 0; i < 3000; i++)
    {
        blob_example[i] = digits[i % 10];
    }

    for (i = 0; i < 15; i++)
    {
        raw_example[i] = digits[i % 10];
    }
}

void add_prop_(OCI_Coll* coll, OCI_Elem* elem, OCI_Object* ii, char* name, char* value)
{
    OCI_ObjectSetInt(ii, "TYPE", 100);
    OCI_ObjectSetInt(ii, "JAVA_TYPE", 27);
    OCI_ObjectSetString(ii, "STR_VALUE", value);
    OCI_ObjectSetString(ii, "NAME", name);
    OCI_ElemSetObject(elem, ii);
    OCI_CollAppend(coll, elem);
};

void add_blob(OCI_Lob* handle, char* val)
{
    OCI_LobSeek(handle, 0, 1);
    OCI_LobTruncate(handle, 0);
    OCI_LobWrite(handle, val, strlen(val));
}

typedef struct
{
    OCI_Connection* con;
    OCI_Enqueue* enq;
    OCI_Dequeue* deq;
    OCI_Msg* msg;
    OCI_TypeInfo* type_q;
    OCI_TypeInfo* type_header;
    OCI_TypeInfo* type_coll;
    OCI_TypeInfo* type_item;
    OCI_Object* header;
    OCI_Object* obj;

    OCI_Coll* coll;
    OCI_Elem* elem;
    OCI_Object* itype;

    OCI_Lob* lob;

} context;

void send_blob_(context ctx, char* buffer)
{
    ctx.msg = OCI_MsgCreate(ctx.type_q);
    ctx.obj = OCI_ObjectCreate(ctx.con, ctx.type_q);
    ctx.header = OCI_ObjectCreate(ctx.con, ctx.type_header);
    //    if(ctx.coll!=0){    OCI_CollFree(ctx.coll);}
    ctx.coll = OCI_CollCreate(ctx.type_coll);
    //  add_prop_(ctx.coll,ctx.elem,ctx.itype,"prop1","v1");
    //  add_prop_(ctx.coll,ctx.elem,ctx.itype,"prop2","v2");

    log_("point6");

    OCI_ObjectSetColl(ctx.header, "PROPERTIES", ctx.coll);
    log_("point6.1");
    OCI_ObjectSetObject(ctx.obj, "HEADER", ctx.header);
    log_("point6.2");
    OCI_ObjectSetDouble(ctx.obj, "BYTES_LEN", strlen(buffer));
    log_("point6.3");
    add_blob(ctx.lob, buffer);
    log_("point6.4");
    OCI_ObjectSetLob(ctx.obj, "BYTES_LOB", ctx.lob);

    log_("point7");
    OCI_MsgSetObject(ctx.msg, ctx.obj);

    log_("point8");
    OCI_EnqueuePut(ctx.enq, ctx.msg);

    OCI_MsgFree(ctx.msg);
    OCI_ObjectFree(ctx.obj);
    //    OCI_CollFree(ctx.coll);
}

void send_nil_(context ctx)
{
    ctx.msg = OCI_MsgCreate(ctx.type_q);
    ctx.obj = OCI_ObjectCreate(ctx.con, ctx.type_q);
    ctx.header = OCI_ObjectCreate(ctx.con, ctx.type_header);
    ctx.coll = OCI_CollCreate(ctx.type_coll);

    add_prop_(ctx.coll, ctx.elem, ctx.itype, "prop1", "v1");
    add_prop_(ctx.coll, ctx.elem, ctx.itype, "prop2", "v2");

    log_("raw.point6");

    OCI_ObjectSetColl(ctx.header, "PROPERTIES", ctx.coll);
    log_("raw.point6.1");

    log_("raw.point7");
    OCI_MsgSetObject(ctx.msg, ctx.obj);

    log_("raw.point8");
    OCI_EnqueuePut(ctx.enq, ctx.msg);
}
void send_raw_(context ctx, char* buffer)
{

    ctx.msg = OCI_MsgCreate(ctx.type_q);
    ctx.obj = OCI_ObjectCreate(ctx.con, ctx.type_q);
    ctx.header = OCI_ObjectCreate(ctx.con, ctx.type_header);
    ctx.coll = OCI_CollCreate(ctx.type_coll);

    add_prop_(ctx.coll, ctx.elem, ctx.itype, "prop1", "v1");
    add_prop_(ctx.coll, ctx.elem, ctx.itype, "prop2", "v2");

    log_("raw.point6");

    OCI_ObjectSetColl(ctx.header, "PROPERTIES", ctx.coll);
    log_("raw.point6.1");
    OCI_ObjectSetObject(ctx.obj, "HEADER", ctx.header);
    log_("point6.2");
    OCI_ObjectSetDouble(ctx.obj, "BYTES_LEN", strlen(buffer));
    OCI_ObjectSetRaw(ctx.obj, "BYTES_RAW", (void*)buffer, strlen(buffer));

    log_("raw.point7");
    OCI_MsgSetObject(ctx.msg, ctx.obj);

    log_("raw.point8");
    OCI_EnqueuePut(ctx.enq, ctx.msg);

    //    OCI_MsgFree(ctx.msg);
    //    OCI_ObjectFree(ctx.obj);
}

void clear_buf(char* buf, int size)
{
    int i = 0;
    for (i = 0; i < size; i++)
    {
        buf[i] = 0;
    }
}
void copy_buf(char* buf1, char* buf2, int size)
{
    int i, ii;
    char ch;
    ii = 0;
    for (i = 0; i < size; i++)
    {
        ch = buf1[i];
        if (ch != 0)
        {
            buf2[ii] = ch;
            ii++;
        }
    }
}
char* read_msg_(context ctx)
{
    int len;
    int size;
    char* buffer;
    char* buffer2;
    OCI_Lob* lob;
    unsigned int char_count;
    unsigned int byte_count;
    int isok;
    int isnull;

    byte_count = 10 * 1024;
    char_count = 10 * 1024;
    buffer = malloc(byte_count);
    buffer2 = malloc(byte_count);
    clear_buf(buffer, byte_count);
    clear_buf(buffer2, byte_count);

    ctx.msg = OCI_DequeueGet(ctx.deq);
    ctx.obj = OCI_MsgGetObject(ctx.msg);
    isnull = OCI_ObjectIsNull(ctx.obj, "BYTES_LEN");

    log2_("BYTES_LEN.isnull:", isnull);
    if (isnull)
    {
        return "nil";
    }

    len = OCI_ObjectGetDouble(ctx.obj, "BYTES_LEN");

    if (len < 2000)
    {

        log2_("len:raw", len);
        size = OCI_ObjectGetRaw(ctx.obj, "BYTES_RAW", buffer, 10 * 1024);
        log2_("len:raw.load:", size);
        log_(buffer);

        return "raw";
    }
    log2_("len:blob:", len);
    lob = OCI_ObjectGetLob(ctx.obj, "BYTES_LOB");
    if (lob == 0)
    {
        log2_("failed !!blob =0", (int)lob);
    }

    char_count = 10 * 1024;
    byte_count = 10 * 1024;

    isok = OCI_LobRead2(lob, buffer, &char_count, &byte_count);
    log2_("len:blob(isok):", isok);
    log2_("len:blob(byte_count):", byte_count);
    copy_buf(buffer, buffer2, byte_count);
    log_(buffer2);

    return "blob";
};

void finih_(context ctx)
{
    OCI_ConnectionFree(ctx.con);
    OCI_Cleanup();
}

int main(int argc, char* argv[])
{
    context ctx;
    int ii;
    char* mode;
    mode = argv[1];
    log_(mode);
    ii = 0;

    log_("point1");
    init_blob_();
    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }
    log_("point2");
    ctx.con = OCI_ConnectionCreate(argv[2], argv[3], argv[4], OCI_SESSION_DEFAULT);

    if (strcmp(mode, "init") == 0)
    {
        OCI_QueueTableCreate(ctx.con, MESSAGE_QUEUE_TABLE, MESSAGE_QUEUE_TYPE, NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL);
        OCI_QueueCreate(ctx.con, MESSAGE_QUEUE, MESSAGE_QUEUE_TABLE, OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL);
        OCI_QueueStart(ctx.con, MESSAGE_QUEUE, TRUE, TRUE);
        finih_(ctx);
        return;
    }

    log_("point3");
    ctx.type_q = OCI_TypeInfoGet(ctx.con, MESSAGE_QUEUE_TYPE, OCI_TIF_TYPE);
    ctx.type_header = OCI_TypeInfoGet(ctx.con, "SYS.AQ$_JMS_HEADER", OCI_TIF_TYPE);
    ctx.type_coll = OCI_TypeInfoGet(ctx.con, "SYS.AQ$_JMS_USERPROPARRAY", OCI_TIF_TYPE);
    ctx.type_item = OCI_TypeInfoGet(ctx.con, "SYS.AQ$_JMS_USERPROPERTY", OCI_TIF_TYPE);
    log_("point4");

    ctx.enq = OCI_EnqueueCreate(ctx.type_q, MESSAGE_QUEUE);
    ctx.deq = OCI_DequeueCreate(ctx.type_q, MESSAGE_QUEUE);

    ctx.coll = OCI_CollCreate(ctx.type_coll);
    ctx.elem = OCI_ElemCreate(ctx.type_coll);
    ctx.itype = OCI_ObjectCreate(ctx.con, ctx.type_item);
    ctx.lob = OCI_LobCreate(ctx.con, 2);

    log_("point5");
    if (strcmp(mode, "send_blob") == 0)
    {
        send_blob_(ctx, blob_example);
        OCI_Commit(ctx.con);
        finih_(ctx);
        return;
    }

    if (strcmp(mode, "send_raw") == 0)
    {
        send_raw_(ctx, raw_example);
        OCI_Commit(ctx.con);
        finih_(ctx);
        return;
    }
    if (strcmp(mode, "send_nil") == 0)
    {
        send_nil_(ctx);
        OCI_Commit(ctx.con);
        finih_(ctx);
        return;
    }

    if (strcmp(mode, "recive_all") == 0)
    {
        while (1 == 1)
        {
            log_(read_msg_(ctx));
            log_("commit");
            OCI_Commit(ctx.con);
            ii += 1;
            log2_("iter:", ii);
        }
        finih_(ctx);
        return;
    }

    OCI_EnqueueFree(ctx.enq);
    OCI_DequeueFree(ctx.deq);

    if (strcmp(mode, "clean") == 0)
    {
        OCI_QueueStop(ctx.con, MESSAGE_QUEUE, TRUE, TRUE, FALSE);
        OCI_QueueDrop(ctx.con, MESSAGE_QUEUE);
        OCI_QueueTableDrop(ctx.con, MESSAGE_QUEUE_TABLE, TRUE);
    }

    finih_(ctx);

    return EXIT_SUCCESS;
}
