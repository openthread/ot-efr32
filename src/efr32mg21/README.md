# OpenThread on EFR32MG21 Example

This directory contains example platform drivers for the [Silicon Labs EFR32MG21][efr32mg] based on [EFR32™ Mighty Gecko Wireless Starter Kit][slwstk6000b]

[efr32mg]: http://www.silabs.com/products/wireless/mesh-networking/efr32mg-mighty-gecko-zigbee-thread-soc
[slwstk6000b]: http://www.silabs.com/products/development-tools/wireless/mesh-networking/mighty-gecko-starter-kit

The example platform drivers are intended to present the minimal code necessary to support OpenThread. [EFR32MG21 SoC][efr32mg21] has rich memory and peripheral resources which can support all OpenThread capabilities. See the "Run the example with EFR32 boards" section below for an example using basic OpenThread capabilities.

[efr32mg21]: https://www.silabs.com/products/wireless/mesh-networking/series-2-efr32-mighty-gecko-zigbee-thread-soc/device.efr32mg21a020f768im32

## Toolchain

Download and install the [GNU toolchain for ARM Cortex-M][gnu-toolchain].

[gnu-toolchain]: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm

In a Bash terminal, follow these instructions to install the GNU toolchain and other dependencies.

```bash
$ cd <path-to-efr32>
$ ./script/bootstrap
```

## Build Examples

Before building example apps, make sure to initialize all submodules. Afterward,
the build may be launched using `./script/build`

```bash
$ cd <path-to-efr32>
$ git submodule update --init
$ ./script/build efr32mg21 -DBOARD=brd4180b
...
-- Configuring done
-- Generating done
-- Build files have been written to: <path-to-efr32>/build
+ [[ -n ot-rcp ot-cli-ftd ot-cli-mtd ot-ncp-ftd ot-ncp-mtd sleepy-demo-ftd sleepy-demo-mtd ]]
+ ninja ot-rcp ot-cli-ftd ot-cli-mtd ot-ncp-ftd ot-ncp-mtd sleepy-demo-ftd sleepy-demo-mtd
[940/940] Linking CXX executable bin/ot-ncp-ftd
+ cd <path-to-efr32>
```

After a successful build, the `elf` files are found in `<path-to-efr32>/build/efr32mg21/bin`.

```bash
$ ls build/efr32mg21/bin
ot-cli-ftd      ot-cli-mtd      ot-ncp-ftd      ot-ncp-mtd      ot-rcp      sleepy-demo-ftd      sleepy-demo-mtd
ot-cli-ftd.s37  ot-cli-mtd.s37  ot-ncp-ftd.s37  ot-ncp-mtd.s37  ot-rcp.s37  sleepy-demo-ftd.s37  sleepy-demo-mtd.s37
```

## Flash Binaries

Simplicity Commander provides a graphical interface for J-Link Commander.

```bash
$ <path-to-simplicity-studio>/developer/adapter_packs/commander/commander
```

In the J-Link Device drop-down list select the serial number of the device to flash. Click the Adapter Connect button. Ensure the Debug Interface drop-down list is set to SWD and click the Target Connect button. Click on the Flash icon on the left side of the window to switch to the flash page. In the Flash MCU pane enter the path of the ot-cli-ftd.s37 file or choose the file with the Browse... button. Click the Flash button located under the Browse... button.

## Run the example with EFR32MG21 boards

1. Flash two EFR32 boards with the `CLI example` firmware (as shown above).
2. Open terminal to first device `/dev/ttyACM0` (serial port settings: 115200 8-N-1). Type `help` for a list of commands.

```bash
> help
help
channel
childtimeout
contextreusedelay
extaddr
extpanid
ipaddr
keysequence
leaderweight
masterkey
mode
netdata register
networkidtimeout
networkname
panid
ping
prefix
releaserouterid
rloc16
route
routerupgradethreshold
scan
start
state
stop
```

3. Start a Thread network as Leader.

```bash
> panid 0xface
Done
> ifconfig up
Done
> thread start
Done

wait a couple of seconds...

> state
leader
Done
```

4. Open terminal to second device `/dev/ttyACM1` (serial port settings: 115200 8-N-1) and attach it to the Thread network as a Router.

```bash
> panid 0xface
Done
> routerselectionjitter 1
Done
> ifconfig up
Done
> thread start
Done

wait a couple of seconds...

> state
router
Done
```

5. List all IPv6 addresses of Leader.

```bash
> ipaddr
fdde:ad00:beef:0:0:ff:fe00:fc00
fdde:ad00:beef:0:0:ff:fe00:800
fdde:ad00:beef:0:5b:3bcd:deff:7786
fe80:0:0:0:6447:6e10:cf7:ee29
Done
```

6. Send an ICMPv6 ping to Leader's Mesh-EID IPv6 address.

```bash
> ping fdde:ad00:beef:0:5b:3bcd:deff:7786
8 bytes from fdde:ad00:beef:0:5b:3bcd:deff:7786: icmp_seq=1 hlim=64 time=24ms
```

The above example demonstrates basic OpenThread capabilities. Enable more features/roles (e.g. commissioner, joiner, DHCPv6 Server/Client, etc.) by assigning compile-options before compiling.

```bash
$ cd <path-to-efr32>
$ ./script/build efr32mg21 -DBOARD=brd4180b -DOT_COMMISSIONER=ON -DOT_JOINER=ON -DOT_DHCP6_CLIENT=ON -DOT_DHCP6_SERVER=ON
```

For a list of all available commands, visit [OpenThread CLI Reference README.md][cli].

[cli]: https://github.com/openthread/openthread/blob/main/src/cli/README.md

## Verification

The following toolchain has been used for testing and verification:

- gcc version 7.3.1

The EFR32 example has been verified with following Flex SDK/RAIL Library version:

- Flex SDK v3.1.x
