/*
**  usb3start - kick off the usb3.device xHCI stack and exit.
**
**  Behaviour:
**    - Opens usb3.device (this triggers PCI scan, chip reset, controller
**      bring-up, and HCD/HUB task creation inside the device library).
**    - Emits a greppable marker via DebugPrintF so the run is easy to find
**      in DumpDebugBuffer output.
**    - On success: deliberately leaks the IORequest reference so lib_OpenCnt
**      stays > 0 and the HCD/HUB tasks survive after main() returns. Exits
**      RETURN_OK.
**    - On any failure: closes / frees every resource that was opened, in
**      reverse order, so nothing leaks. Exits RETURN_FAIL.
**
**  Does NOT block waiting for Ctrl-C; OpenDevice returns once the stack
**  is up.
**
**  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <exec/types.h>
#include <exec/io.h>
#include <exec/ports.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>

int main(void)
{
    struct MsgPort   *port        = NULL;
    struct IORequest *ior         = NULL;
    BOOL              device_open = FALSE;
    LONG              err;
    int               rc          = RETURN_FAIL;

    /* Unique greppable marker for this run. Build timestamp + ID so a
       single dump can include multiple runs and we can find the latest. */
    IExec->DebugPrintF("\n");
    IExec->DebugPrintF("==================================================\n");
    IExec->DebugPrintF("=== USB3START build %s %s - RUN BEGIN ===\n",
        __DATE__, __TIME__);
    IExec->DebugPrintF("==================================================\n");

    port = IExec->AllocSysObjectTags(ASOT_PORT, TAG_END);
    if (!port) {
        IDOS->Printf("usb3start: AllocSysObject(PORT) failed\n");
        IExec->DebugPrintF("=== USB3START: PORT alloc failed ===\n");
        goto cleanup;
    }

    ior = IExec->AllocSysObjectTags(ASOT_IOREQUEST,
        ASOIOR_Size,      sizeof(struct IORequest),
        ASOIOR_ReplyPort, port,
        TAG_END);
    if (!ior) {
        IDOS->Printf("usb3start: AllocSysObject(IOREQUEST) failed\n");
        IExec->DebugPrintF("=== USB3START: IOREQUEST alloc failed ===\n");
        goto cleanup;
    }

    IDOS->Printf("usb3start: opening usb3.device ...\n");
    err = IExec->OpenDevice("usb3.device", 0, ior, 0);
    if (err) {
        IDOS->Printf("usb3start: OpenDevice failed, IoErr=%ld\n", err);
        IExec->DebugPrintF("=== USB3START: OpenDevice failed (IoErr=%ld) ===\n", err);
        goto cleanup;
    }
    device_open = TRUE;

    IDOS->Printf("usb3start: usb3.device opened OK - stack is running.\n");
    IDOS->Printf("usb3start: exiting; HCD/HUB tasks remain active in background.\n");
    IExec->DebugPrintF("=== USB3START: SUCCESS - stack alive, intentional refcount leak ===\n");

    /*
     * Success path: deliberately do NOT CloseDevice / FreeSysObject.
     * lib_OpenCnt stays > 0, keeping the HCD/HUB tasks alive after main
     * returns. Tiny one-time memory leak (~64 B) is the price.
     */
    return RETURN_OK;

cleanup:
    /* Failure path: undo allocations in reverse order so nothing leaks. */
    if (device_open) {
        IExec->CloseDevice(ior);
        device_open = FALSE;
    }
    if (ior) {
        IExec->FreeSysObject(ASOT_IOREQUEST, ior);
        ior = NULL;
    }
    if (port) {
        IExec->FreeSysObject(ASOT_PORT, port);
        port = NULL;
    }
    IExec->DebugPrintF("=== USB3START: cleanup complete, exiting RETURN_FAIL ===\n");
    return rc;
}
