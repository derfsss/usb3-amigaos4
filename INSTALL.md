# Installing the usb3 stack

Unlike the upstream usb2 stack — which replaces the operating system's USB
modules in Kickstart — **usb3 is loaded after boot and coexists with the
stock AmigaOS USB stack**. Your Kicklayout is not modified, the onboard
EHCI/OHCI controllers stay with `usbsys.device`, and removing usb3 is as
simple as deleting two files.

## Requirements

- AmigaOne X5000 running AmigaOS 4.1 Final Edition
- A PCIe xHCI (USB 3.0) controller card — tested with the Renesas uPD720202
- `bin/usb3.device` and `Programs/usb3start/usb3start` from this repository
  (see README for build instructions)

## Step 1: Copy the device driver

```
Copy usb3.device DEVS:
```

## Step 2: Create the class-driver directory

The stack scans `LIBS:USB3/` for external class drivers. The built-in
drivers (hub, HID, mass storage) are compiled in, so the directory can be
empty — but it must exist:

```
MakeDir LIBS:USB3
```

## Step 3: Start the stack

```
Run usb3start
```

`usb3start` opens `usb3.device` — which triggers the PCI scan, controller
initialisation and HCD/hub task creation — prints the result, and exits.
The stack keeps running in the background. Mass-storage devices attached to
the card are exposed through `usb3disk.device` and mounted automatically by
the system Mounter.

To start the stack on every boot, add the `Run usb3start` line to
`S:User-Startup`, or place `usb3start` in `SYS:WBStartup`.

## Stopping / uninstalling

There is no unload utility yet; the stack runs until reboot. To uninstall,
delete `DEVS:usb3.device`, `LIBS:USB3/` and the `usb3start` file, then
reboot.

## Troubleshooting

- **`usb3start` reports "No Controllers found".** The xHCI card is not
  visible on the PCIe bus. This occasionally happens on a cold power-on;
  reboot and try again. Verify the card is seated and shows up in
  `SYS:Utilities/PCITool` (class `0C0330`).
- **The controller fails to reset (debug output shows `HCRST` stuck).**
  See "Known issues" in the README — the card was most likely left running
  across a hardware reset. Power the machine off completely at the PSU
  switch for ~30 seconds and power back on.
- **Diagnostics.** The stack logs extensively to the kernel debug buffer.
  Capture it with `DumpDebugBuffer >RAM:usb3.log` after a run when
  reporting problems.
