# usb3-amigaos4

**An experimental USB 3.0 (xHCI) host stack for AmigaOS 4.1 — `usb3.device`**

> ⚠️ **BETA SOFTWARE.** This stack is under active development. It works for the
> configurations listed below, but it is not feature-complete and has not had
> wide hardware exposure. Keep backups of any media you connect to it.

## What this is

`usb3.device` adds USB 3.0 host controller (xHCI) support to AmigaOS 4.1 on the
AmigaOne X5000. It drives a PCIe xHCI controller card **alongside** the stock
AmigaOS USB stack: the onboard EHCI/OHCI controllers remain owned by the
operating system's own `usbsys.device`, while this stack claims only xHCI
controllers found on the PCIe bus. No Kickstart modules are replaced and the
standard boot configuration is left untouched.

The stack is a derivative of Rene W. Olsen's experimental USB host stack for
AmigaOS 4 ([rolsen74/usb2](https://github.com/rolsen74/usb2)), with all
user-visible names moved to a separate `usb3` namespace so the two can never
collide, and with a new xHCI host controller driver developed for this project.

## Status

Verified on real hardware (AmigaOne X5000/20, Renesas uPD720202 PCIe card):

| Area | Status |
|---|---|
| xHCI controller bring-up (reset, slots, command/event rings) | ✅ Working |
| Device enumeration (USB 2.0 device on xHCI port) | ✅ Working |
| Control transfers | ✅ Working |
| Interrupt transfers | ✅ Working |
| Bulk transfers (chained TDs, short-packet/residue handling) | ✅ Working |
| Mass storage: `usb3disk.device`, volumes mount read/write | ✅ Working, integrity-verified |
| HID (mouse/keyboard) on xHCI ports | ⚠️ Untested |
| SuperSpeed (USB 3.0 ports / 5 Gbps) | ⚠️ Untested — testing used the card's USB 2.0 ports |
| Isochronous transfers | ❌ Not implemented |
| Hot-unplug robustness | ⚠️ Limited testing |

## Supported hardware

- **Machine:** AmigaOne X5000/20 (expected to work on X5000/40; untested)
- **OS:** AmigaOS 4.1 Final Edition
- **Controller:** Renesas uPD720202 (PCI `1912:0015`) PCIe USB 3.0 card.
  Other xHCI controllers are matched by PCI class and may work, but only the
  uPD720202 has been tested. Note that the uPD720201/202 must be a variant
  with onboard firmware ROM (most retail cards are); firmware upload over PCIe
  is not implemented.

## Building

Cross-compile with the AmigaOS 4 GCC toolchain (`ppc-amigaos-gcc`). A
convenient prebuilt toolchain is available as a Docker image
([walkero/amigagccondocker](https://hub.docker.com/r/walkero/amigagccondocker)):

```sh
docker run --rm -v "$(pwd):/opt/code" -w /opt/code \
    walkero/amigagccondocker:os4-gcc11 make -j8
```

The default build produces an xHCI-only `bin/usb3.device` (the EHCI/OHCI/UHCI
drivers inherited from the upstream stack are disabled in the makefile; they
can be re-enabled with `make DRV_EHCI=1 ...` but that configuration is neither
needed nor tested here).

The loader utility is built separately:

```sh
ppc-amigaos-gcc -O2 -Wall -o Programs/usb3start/usb3start Programs/usb3start/usb3start.c
```

## Installation and use

See [INSTALL.md](INSTALL.md). In short: copy `usb3.device` to `DEVS:`, create
`LIBS:USB3/`, and run `usb3start` from a Shell (or `WBStartup`). Mass-storage
devices attached to the card appear via `usb3disk.device` and mount
automatically.

## Roadmap

Development priorities — starting with automatic mount/dismount of storage
devices on insert/removal — are tracked in [ROADMAP.md](ROADMAP.md).

## Known issues

- **Warm-reboot controller wedge (uPD720202).** If the controller is left
  running across a hardware reset, the chip can enter a state where `HCRST`
  never completes and no software recovery succeeds (the driver attempts a
  six-tier escalation: HCRST, PCI command toggle, D3hot→D0, FLR, PCIe
  Secondary Bus Reset, PCIe Link Disable — on the X5000 the P5020 root
  complex ignores the latter two, and the card retains standby power even
  with the PSU soft-off). The stack installs a reset handler that halts the
  controller on *software* reboots, which prevents the wedge in normal use.
  If it does occur, recovery requires removing mains power completely for
  ~30 seconds (PSU rocker switch) or re-seating the card.
- **Cold-boot detection.** On some cold power-ons the card fails to train its
  PCIe link and is absent from the bus; a reboot brings it back.
- A failed controller initialisation currently does not fail
  `OpenDevice()`; the stack loads but no ports are serviced.

## References

- [xHCI Specification 1.2](https://www.intel.com/content/www/us/en/products/docs/io/universal-serial-bus/extensible-host-controler-interface-usb-xhci.html) — Intel, *eXtensible Host Controller Interface for USB*
- [USB 2.0 / 3.2 Specifications](https://www.usb.org/documents) — USB Implementers Forum
- [USB Mass Storage Class, Bulk-Only Transport 1.0](https://www.usb.org/document-library/mass-storage-bulk-only-10) — USB-IF
- [Linux xHCI driver](https://github.com/torvalds/linux/tree/master/drivers/usb/host) — reference for controller quirk handling
- [rolsen74/usb2](https://github.com/rolsen74/usb2) — the upstream AmigaOS 4 USB stack this project derives from

## Credits

- **Rene W. Olsen** — author of the original AmigaOS 4 USB stack (2012–2026)
  that provides the device framework, hub/HID/MSD class drivers and
  EHCI/OHCI heritage this project builds on.
- **Richard Gibbs** — xHCI host controller driver, Renesas/X5000 bring-up,
  usb3 port.

## License

GNU General Public License v3.0 or later. See [LICENSE](LICENSE); all source
files carry SPDX license identifiers.
