# EFR32 Sleepy Demo

The EFR32 Sleepy applications demonstrate Sleepy End Device behavior using the EFR32's low power EM2 mode. The steps below will take you through the process of building and running the demo

Note that a Sleepy End Device can be demonstrated in two ways.
**_sleepy-demo-mtd_**: Demonstrates Sleepy End Device (SED) behaviour with polling.
**_sleepy-demo-ssed_**: Demonstrates Synchronous Sleepy End Device (SSED) behaviour with CSL.

For setting up the build environment refer to [OpenThread on EFR32](../../src/README.md).

## 1. Build

In this `README`, all example commands will be targeting the `brd4186c` board on the `efr32mg24` platform. The same commands should work for the other boards

```bash
$ cd <path-to-ot-efr32>
$ board="brd4186c"
$ ./script/build $board
```

The build script will convert the resulting executables into S-Record format and append a `.s37` file extension.

```bash
$ ls build/$board/bin/sleepy*
build/brd4186c/bin/sleepy-demo-ftd  build/brd4186c/bin/sleepy-demo-ftd.s37  build/brd4186c/bin/sleepy-demo-mtd  build/brd4186c/bin/sleepy-demo-mtd.s37
```

In Silicon Labs Simplicity Studio flash one device with the `sleepy-demo-mtd.s37` image and the other device with the `sleepy-demo-ftd.s37` image.

For instructions on flashing firmware, see [Flashing binaries](../../src/README.md#flashing-binaries)

## 2. Starting Nodes

For demonstration purposes the network settings are hardcoded within the source files. The devices start Thread and form a network within a few seconds of powering on. In a real-life application the devices should implement and go through a commissioning process to create a network and add devices.

When the **_sleepy-demo-ftd_** device is started, the CLI should show:

```
sleepy-demo-ftd started
sleepy-demo-ftd changed to leader
```

When the **_sleepy-demo-mtd_** device is started, the CLI should show:

```
sleepy-demo-mtd started
[poll period: 2000 ms.]
```

The application is configured to join the pre-configured Thread network, disabling Rx-on-when-idle mode to become a Sleepy End Device. The default poll period is set in `sleepy-mtd.c`.

Issue the command `child table` in the FTD console and observe that the R (Rx-on-when-idle) flag of the child is 0.

```
> child table
| ID  | RLOC16 | Timeout    | Age        | LQ In | C_VN |R|D|N|Ver|CSL|QMsgCnt|Suprvsn| Extended MAC     |
+-----+--------+------------+------------+-------+------+-+-+-+---+---+-------+-------+------------------+
|   1 | 0x8401 |        240 |          3 |     3 |    3 |0|0|0|  4| 0 |     0 |   129 | 667bf54fcc2aed8a |

Done
```

When the **_sleepy-demo-ssed_** device is started, the CLI should show:

```
sleepy-demo-ssed started
[csl period: 500000 us.] [csl timeout: 30 sec.]
```

The application is configured to join the pre-configured Thread network, disabling Rx-on-when-idle mode to become a Synchronous Sleepy End Device. The default CSL parameters are set in `sleepy-ssed.c`

Issue the command `child table` in the FTD console and observe that the R (Rx-on-when-idle) flag of the child is 0, and the CSL flag is 1.

```
> child table
| ID  | RLOC16 | Timeout    | Age        | LQ In | C_VN |R|D|N|Ver|CSL|QMsgCnt|Suprvsn| Extended MAC     |
+-----+--------+------------+------------+-------+------+-+-+-+---+---+-------+-------+------------------+
|   1 | 0x8402 |        240 |          3 |     3 |    3 |0|0|0|  4| 1 |     0 |   129 | 8e8582dbd78c243c |

Done
```

## 3. Buttons on the MTD/SSED

Pressing button 0 on the MTD/SSED toggles between EM2 (sleep) and EM1 (idle) modes.

Pressing button 1 on the MTD/SSED sends a multicast UDP message containing a pre-defined string. The FTD listens on the multicast address and displays `Message Received: <string>` in the CLI.

## 4. Buttons on the FTD

Pressing either button 0 or 1 on the FTD sends a UDP message to the FTD containing the string "ftd button". First, press the MTD's/SSED's button 1 to send a multicast message so that the FTD knows the address of the sleepy device to send messages to.

## 5. Monitoring power consumption of the MTD/SSED

Open the Energy Profiler in Simplicity Studio 5 (SSv5). In the Quick Access menu select **Start Energy Capture...** and select the MTD/SSED device. When operating during EM2 (sleep) mode, the current should be under 20 microamps with occasional spikes during waking, polling the parent or turning its receiver on during a CSL window.

When the device goes back to EM1 (idle) mode, observe that the current is in the order of 10 mA.

With further configuration of GPIOs and peripherals it is possible to reduce the sleepy current consumption further.

## 6. Notes on sleeping, sleepy callback and interrupts

To allow the EFR32 to enter sleepy mode, the application must register a callback with `efr32SetSleepCallback`. The return value of the callback is used to indicate that the application has no further work to do and that it is safe to go into a low power mode. The callback is called with interrupts disabled so should do the minimum required to check if it can sleep.
