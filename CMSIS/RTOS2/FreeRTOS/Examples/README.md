# CMSIS-FreeRTOS Examples

## Solution

| solution-name | solution YML                                           |
|:--------------|:-------------------------------------------------------|
| Examples      | [Examples.csolution.yml](Examples.csolution.yml)       |

## Project

| project-name  | project YML                                                                    |
|:--------------|:-------------------------------------------------------------------------------|
| Blinky        | [Blinky.cproject.yml](App/Blinky.cproject.yml)                                 |
| Blinky_Native | [Blinky_Native.cproject.yml](App/Blinky_Native.cproject.yml)                   |
| TZ_NonSecure  | [TZ_NonSecure.cproject.yml](App/TrustZone/NonSecure/TZ_NonSecure.cproject.yml) |
| TZ_Secure     | [TZ_Secure.cproject.yml](App/TrustZone/Secure/TZ_NonSecure.cproject.yml)       |

## Target types

| target-type | Target processor          | model-executable               |
|:------------|:--------------------------|:-------------------------------|
| CM0         | Cortex-M0                 | FVP_MPS2_Cortex-M0             |
| CM0plus     | Cortex-M0+                | FVP_MPS2_Cortex-M0plus         |
| CM3         | Cortex-M3                 | FVP_MPS2_Cortex-M3             |
| CM4         | Cortex-M4                 | FVP_MPS2_Cortex-M4             |
| CM7         | Cortex-M7                 | FVP_MPS2_Cortex-M7             |
| CM23        | Cortex-M23                | FVP_MPS2_Cortex-M23            |
| CM23_noTZ   | Cortex-M23                | FVP_MPS2_Cortex-M23            |
| CM33        | Cortex-M33                | FVP_MPS2_Cortex-M33            |
| CM33_noTZ   | Cortex-M33                | FVP_MPS2_Cortex-M33            |
| CM55        | Cortex-M55                | FVP_Corstone_SSE-300           |
| CM55_noTZ   | Cortex-M55                | FVP_Corstone_SSE-300           |
| CM85        | Cortex-M85                | FVP_Corstone_SSE-310           |
| CM85_noTZ   | Cortex-M85                | FVP_Corstone_SSE-310           |

## Build types

| build-type  |
|:------------|
| Debug       |
| Release     |

## Toolchain

| toolchain     |
|:--------------|
| AC6           |
| CLANG         |
| GCC (default) |

## Build

Build command syntax:

`cbuild Examples.csolution.yml [--context [<project-name>][.<build-type>][+<target-type>]] [--toolchain <toolchain>]`

### Hello World example

- Build example for debugging running on Cortex-M3 (use Arm Compiler 6)
  ```sh
  cbuild Examples.csolution.yml --context Hello.Debug+CM3 --update-rte --toolchain AC6
  ```

### TrustZone example (for ArmV8-M with TrustZone)

1. Build secure side example for Cortex-M33 (debug build)
   ```sh
   cbuild Examples.csolution.yml -r --context TZ_Secure.Debug+CM33S --update-rte --toolchain AC6
   ```
2. Build non-secure example for Cortex-M33 (debug build)
   ```sh
   cbuild Examples.csolution.yml -r --context TZ_NonSecure.Debug+CM33NS --update-rte  --toolchain AC6
   ```

## Run Example Application on VHT model

Run command syntax:

`<model-executable> -f ./Target/<target_type>/fvp_config.txt ./out/<project>/<project>.axf`

Examples:

- Run Hello World example executable on model for Cortex-M3
```sh
FVP_MPS2_Cortex-M3 -f ./Target/CM3/fvp_config.txt ./out/Hello/Hello.axf
```

- Run Blinky executable on model for Cortex-M3
```sh
FVP_MPS2_Cortex-M3 -f ./Target/CM3/fvp_config.txt ./out/Blinky/Blinky.axf
```

- Run TrustZone executable on model for Cortex-M33
```sh
FVP_MPS2_Cortex-M33 -f ./Target/CM33NS/fvp_config.txt -a ./out/TZ_Secure/TZ_Secure.axf -a ./out/TZ_NonSecure/TZ_NonSecure.axf
```

- Run TrustZone executable on model for Cortex-M55
```sh
FVP_Corstone_SSE-300 -f ./Target/CM55NS/fvp_config.txt -a ./out/TZ_Secure/TZ_Secure.axf -a ./out/TZ_NonSecure/TZ_NonSecure.axf
```

- Run TrustZone executable on model for Cortex-M85
```sh
FVP_Corstone_SSE-310 -f ./Target/CM85NS/fvp_config.txt -a ./out/TZ_Secure/TZ_Secure.axf -a ./out/TZ_NonSecure/TZ_NonSecure.axf
```

------------------------------------------------
cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM0 --update-rte --toolchain AC6
FVP_MPS2_Cortex-M0 -f ./Target/CM0/fvp_config.txt ./out/Hello/Hello.axf
cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM0 --update-rte --toolchain GCC
FVP_MPS2_Cortex-M0 -f ./Target/CM0/fvp_config.txt ./out/Hello/Hello.elf

cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM0plus --update-rte --toolchain AC6
FVP_MPS2_Cortex-M0plus -f ./Target/CM0plus/fvp_config.txt ./out/Hello/Hello.axf
cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM0plus --update-rte --toolchain GCC
FVP_MPS2_Cortex-M0plus -f ./Target/CM0plus/fvp_config.txt ./out/Hello/Hello.elf

cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM3 --update-rte --toolchain AC6
FVP_MPS2_Cortex-M3 -f ./Target/CM3/fvp_config.txt ./out/Hello/Hello.axf
cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM3 --update-rte --toolchain GCC
FVP_MPS2_Cortex-M3 -f ./Target/CM3/fvp_config.txt ./out/Hello/Hello.elf

cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM4 --update-rte --toolchain AC6
FVP_MPS2_Cortex-M4 -f ./Target/CM4/fvp_config.txt ./out/Hello/Hello.axf
cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM4 --update-rte --toolchain GCC
FVP_MPS2_Cortex-M4 -f ./Target/CM4/fvp_config.txt ./out/Hello/Hello.elf

cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM7 --update-rte --toolchain AC6
FVP_MPS2_Cortex-M7 -f ./Target/CM7/fvp_config.txt ./out/Hello/Hello.axf
cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM7 --update-rte --toolchain GCC
FVP_MPS2_Cortex-M7 -f ./Target/CM7/fvp_config.txt ./out/Hello/Hello.elf

cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM23 --update-rte --toolchain AC6
FVP_MPS2_Cortex-M23 -f ./Target/CM23/fvp_config.txt ./out/Hello/Hello.axf
cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM23 --update-rte --toolchain GCC
FVP_MPS2_Cortex-M23 -f ./Target/CM23/fvp_config.txt ./out/Hello/Hello.elf

cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM33 --update-rte --toolchain AC6
FVP_MPS2_Cortex-M33 -f ./Target/CM33/fvp_config.txt ./out/Hello/Hello.axf
cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM33 --update-rte --toolchain GCC
FVP_MPS2_Cortex-M33 -f ./Target/CM33/fvp_config.txt ./out/Hello/Hello.elf

cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM55 --update-rte --toolchain AC6
FVP_Corstone_SSE-300 -f ./Target/CM55/fvp_config.txt ./out/Hello/Hello.axf
cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM55 --update-rte --toolchain GCC
FVP_Corstone_SSE-300 -f ./Target/CM55/fvp_config.txt ./out/Hello/Hello.elf

cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM85 --update-rte --toolchain AC6
FVP_Corstone_SSE-310 -f ./Target/CM85/fvp_config.txt ./out/Hello/Hello.axf
cbuild Examples.csolution.yml --rebuild --context Hello.Debug+CM85 --update-rte --toolchain GCC
FVP_Corstone_SSE-310 -f ./Target/CM85/fvp_config.txt ./out/Hello/Hello.elf

Test: Hello
===========
| target-type | AC6 | GCC |
|:------------|:----|:----|
| CM0         | OK  | OK  |
| CM0plus     | OK  | OK  |
| CM3         | OK  | OK  |
| CM4         | OK  | OK  |
| CM7         | OK  | OK  |
| CM23        | OK  | OK  |
| CM23NS      |     |     |
| CM23S       |     |     |
| CM33        | OK  | OK  |
| CM33NS      |     |     |
| CM33S       |     |     |
| CM55        | OK  | OK  |
| CM55NS      |     |     |
| CM55S       |     |     |
| CM85        |     |     |
| CM85NS      |     |     |
| CM85S       |     |     |


cbuild Examples.csolution.yml --rebuild --context TZ_Secure.Debug+CM23S --update-rte --toolchain AC6
cbuild Examples.csolution.yml --rebuild --context TZ_NonSecure.Debug+CM23NS --update-rte --toolchain AC6
FVP_MPS2_Cortex-M23 -f ./Target/CM23/fvp_config.txt -a ./out/TZ_NonSecure/TZ_NonSecure.axf -a ./out/TZ_Secure/TZ_Secure.axf

cbuild Examples.csolution.yml --rebuild --context TZ_Secure.Debug+CM33S --update-rte --toolchain AC6
cbuild Examples.csolution.yml --rebuild --context TZ_NonSecure.Debug+CM33NS --update-rte --toolchain AC6
FVP_MPS2_Cortex-M33 -f ./Target/CM33/fvp_config.txt -a ./out/TZ_NonSecure/TZ_NonSecure.axf -a ./out/TZ_Secure/TZ_Secure.axf

cbuild Examples.csolution.yml --rebuild --context TZ_Secure.Debug+CM55S --update-rte --toolchain AC6
cbuild Examples.csolution.yml --rebuild --context TZ_NonSecure.Debug+CM55NS --update-rte --toolchain AC6
FVP_Corstone_SSE-300 -f ./Target/CM55/fvp_config.txt -a ./out/TZ_NonSecure/TZ_NonSecure.axf -a ./out/TZ_Secure/TZ_Secure.axf

cbuild Examples.csolution.yml --rebuild --context TZ_Secure.Debug+CM85S --update-rte --toolchain AC6
cbuild Examples.csolution.yml --rebuild --context TZ_NonSecure.Debug+CM85NS --update-rte --toolchain AC6
FVP_Corstone_SSE-310 -f ./Target/CM85/fvp_config.txt -a ./out/TZ_NonSecure/TZ_NonSecure.axf -a ./out/TZ_Secure/TZ_Secure.axf

Test: TZ
===========
| target-type | AC6 | GCC |
|:------------|:----|:----|
| CM23NS      | OK  |     |
| CM23S       | OK  |     |
| CM33NS      | OK  |     |
| CM33S       | OK  |     |
| CM55NS      |     |     |
| CM55S       |     |     |
| CM85NS      |     |     |
| CM85S       |     |     |
