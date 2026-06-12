# Roadmap

This document reviews what the usb3 stack can do today and lays out the plan
for modernising it. Phases are ordered by priority; each phase is independently
useful and testable.

## Current capabilities (June 2026)

**Working, verified on hardware (X5000/20 + Renesas uPD720202):**

- xHCI controller bring-up: reset (with a six-tier recovery ladder for the
  uPD720202's warm-reboot quirks), slot/command/event ring management,
  port handling, MSI-less interrupt operation
- Device enumeration on USB 2.0 ports (High-Speed verified)
- Control, interrupt and bulk transfers; bulk uses chained TDs with Event
  Data TRBs so short packets / residue are reported exactly
- Mass storage (Bulk-Only Transport): `usb3disk.device` units, automatic
  volume mounting at stack start via mounter.library `AnnounceDevice`
- Clean controller shutdown on software reboot (reset handler)

**Present but unverified on this controller:** HID boot-protocol mouse and
keyboard (verified on EHCI/QEMU only), external hubs, the Printer / RTL8188 /
PTP class drivers inherited from upstream.

**Known gaps** (discovered during the hardware bring-up, see also README
"Known issues"):

- Removal of a storage device is only noticed when an I/O fails: the Mounter
  `DenounceDevice` call lives solely in the bulk-transport error path
  (`MSD_2_Disk_DoBulk_*`); a surprise unplug of an idle stick leaves the
  volume mounted
- `TD_ADDCHANGEINT` / `NSCMD_AddStatCallBack` requests are queued but never
  fired -- filesystems are never told about media changes
- The xHCI slot is never freed on device detach (`Disable Slot` is only
  issued on enumeration errors); repeated plug/unplug leaks slots and leaves
  stale `SlotID_ByAddress` entries
- No `AbortIO`-driven cancellation of in-flight xHCI transfers
  (Stop Endpoint / Set TR Dequeue are unimplemented)
- Endpoint stall recovery issues the USB-level `CLEAR_FEATURE` but not the
  xHCI `Reset Endpoint` command, so a stalled endpoint stays halted at the
  controller
- A failed controller initialisation does not fail `OpenDevice()`
- SuperSpeed (ports 1-2 on the card) and isochronous transfers unimplemented

---

## Phase 1 — Storage hotplug: automatic mount and dismount

*Goal: inserting a USB stick mounts its volumes within a couple of seconds;
removing it dismounts them immediately and safely, whether or not I/O was in
flight. This is the flagship "feels modern" feature and exercises most of the
robustness work everything else needs.*

1. **Verify live insertion.** The hub driver already re-enumerates on
   connect-change interrupts; confirm the whole chain (port change → reset →
   enumerate → MSD bind → `AnnounceDevice` → Mounter automount) works while
   the stack is running, and fix whatever falls out.
2. **Denounce on detach, not just on I/O failure.** Call the detach path
   (`DenounceDevice`) from the MSD unit teardown that runs when the hub
   driver removes the function node, so an idle stick's volumes disappear on
   unplug.
3. **Fire disk-change interrupts.** Complete the queued `TD_ADDCHANGEINT` /
   `NSCMD_AddStatCallBack` requests on attach and detach so filesystems
   react immediately.
4. **Fail fast after removal.** Mark detached units so queued and future
   I/O returns `TDERR_DiskChanged` / `IOERR_OPENFAIL` immediately instead of
   timing out against dead hardware.
5. **xHCI detach hygiene.** On function removal: Stop Endpoint for active
   rings, Disable Slot, free the slot's rings and contexts, clear the
   address-to-slot mapping and cached port speed. Transfer events for
   already-freed slots must be discarded gracefully.
6. **Abort in-flight transfers.** Implement Stop Endpoint + Set TR Dequeue
   so `AbortIO` and the detach path can cancel a bulk transfer that will
   never complete.
7. **Test matrix:** insert at runtime; remove while idle; remove mid-read
   and mid-write; rapid insert/remove cycling; two sticks on different
   ports; re-insert of the same stick (address/slot reuse).

## Phase 2 — Robustness

- xHCI-correct stall recovery: `Reset Endpoint` + `Set TR Dequeue` alongside
  the existing `CLEAR_FEATURE(ENDPOINT_HALT)`, so MSD's recovery path
  actually clears a halted endpoint
- Propagate controller-init failure to `OpenDevice()` (no more
  silently-dead stack)
- Watchdog/timeout coverage for transfers that never complete (the stack's
  timer framework exists; wire the xHCI Remove path into it)
- Event-ring overflow and Host Controller Error (HCE) handling: restart the
  controller via the existing chip-restart signal rather than requiring a
  reboot
- Survive (and log) babble, transaction errors and protocol stalls per
  endpoint without taking down the whole device

## Phase 3 — SuperSpeed (USB 3.0, 5 Gbps)

- Drive the card's USB 3.0 ports: warm reset, link state (PLS) management,
  RxDetect/Polling/U0 transitions (groundwork — the compliance-mode escape —
  is already in place)
- Parse SuperSpeed Endpoint Companion descriptors; program burst size in
  endpoint contexts; honour the 1024-byte bulk max packet
- SuperSpeed device test pass with a USB 3.0 stick at 5 Gbps
- External hub support on xHCI (route strings in the slot context, hub
  flags, split-transaction fields for FS/LS devices behind HS hubs)

## Phase 4 — Performance

- Replace bounce-buffer copies with direct scatter-gather DMA of the
  caller's buffer (per-page TRBs via MMU physical-address lookup); keep
  bounce as fallback for unaligned/non-DMA-safe memory
- Interrupt moderation (IMOD) tuning and event-ring batch processing
- Measure and publish throughput against the stock stack's EHCI as a
  baseline (the same stick on both controllers)
- Longer-term: UASP (USB Attached SCSI) with xHCI streams for
  command-queued storage

## Phase 5 — Device classes

- Verify HID (mouse/keyboard) on the xHCI ports; fix what differs from the
  EHCI-verified path
- Full HID report-descriptor parsing (beyond boot protocol): wheels,
  side-buttons, gamepads — feeding AmigaInput
- Bring the inherited Printer class driver up against usb3.device
- Isochronous transfer support (audio class, then video) — needs xHCI isoch
  TD scheduling; largest single work item on this list

## Phase 6 — User experience and distribution

- Desktop notifications on attach/detach (the stack has a `DO_NOTIFY`
  Ringhio hook; finish and enable it)
- A small prefs file (`ENVARC:usb3.prefs`) for debug level, automount
  policy and per-device quirks
- Port the bundled USB3Analyzer monitoring tool to the usb3 namespace and
  make it build out of the box
- Versioned releases: LHA packaging with an Installer script, AmiUpdate
  support, and a CHANGELOG
- SDK target: install the public `usb3.h` headers so third parties can
  write class drivers

---

*Phases 1-2 are about correctness and are prerequisites for everything
else. Phases 3-4 widen the hardware envelope and speed. Phases 5-6 broaden
what the stack is useful for. Suggestions and patches welcome — see
README.md for contribution notes.*
