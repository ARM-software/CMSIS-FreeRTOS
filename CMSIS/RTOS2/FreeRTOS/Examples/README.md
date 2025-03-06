# CMSIS-FreeRTOS Examples

CMSIS-FreeRTOS provides csolution based examples that can be built for multiple Cortex-M targets.

The solution file [Examples.csolution.yml](Examples.csolution.yml) defines projects and supported targets.


## Projects

| Project-Name                                                     | Purpose                                          | Description                                   |
|:-----------------------------------------------------------------|:-------------------------------------------------|:----------------------------------------------|
| [Hello](App/Hello/Hello.cproject.yml)                            | Simple Hello World application                   | [README.md](App/Hello/README.md)              |
| [TZ_Secure](App/TrustZone/Secure/TZ_Secure.cproject.yml)         | TrustZone application running on Secure side     | [README.md](App/TrustZone/Secure/README.md)   |
| [TZ_NonSecure](App/TrustZone/NonSecure/TZ_NonSecure.cproject.yml)| TrustZone application running on Non-secure side | [README.md](App/TrustZone/NonSecure/README.md)|

## Targets

The below table lists supported target processors together with the corresponding context target-types
and model executable that shall be used for running the application image.

| Target processor | Target-Type | Model Executable       |
|:-----------------|:------------|:-----------------------|
| Cortex-M0        | CM0         | FVP_MPS2_Cortex-M0     |
| Cortex-M0+       | CM0plus     | FVP_MPS2_Cortex-M0plus |
| Cortex-M3        | CM3         | FVP_MPS2_Cortex-M3     |
| Cortex-M4        | CM4         | FVP_MPS2_Cortex-M4     |
| Cortex-M7        | CM7         | FVP_MPS2_Cortex-M7     |
| Cortex-M23       | CM23        | FVP_MPS2_Cortex-M23    |
| Cortex-M23       | CM23_noTZ   | FVP_MPS2_Cortex-M23    |
| Cortex-M33       | CM33        | FVP_MPS2_Cortex-M33    |
| Cortex-M33       | CM33_noTZ   | FVP_MPS2_Cortex-M33    |
| Cortex-M55       | CM55        | FVP_Corstone_SSE-300   |
| Cortex-M55       | CM55_noTZ   | FVP_Corstone_SSE-300   |
| Cortex-M85       | CM85        | FVP_Corstone_SSE-310   |
| Cortex-M85       | CM85_noTZ   | FVP_Corstone_SSE-310   |

## Build

The Examples solution supports only "Debug" Build-Type which is optimized for debugging. It disables
compiler optimization and retains all debug related information. By default, Arm Compiler 6 is used to
build the projects.

## Build in VS Code using Arm Keil Studio Pack extensions

> - See [Arm Keil Studio Visual Studio Code Extensions User Guide](https://developer.arm.com/documentation/108029/0000/?lang=en)
>   for more information about using the Keil Studio extensions.
> - Search for [Arm Keil Studio Pack](https://marketplace.visualstudio.com/items?itemName=Arm.keil-studio-pack)
>   in the Visual Studio Marketplace to download the extensions.

To build a project using Keil Studio extensions open CMSIS view, open "Manage CMSIS Solution" view and select
"Active Context" and "Active Projects". Build Type is automatically selected since there is only
one option.

Once the context and projects are selected one can build them by selecting "Build" in CMSIS view. 

## Build via command line

> - See [CMSIS-Toolbox documentation](https://open-cmsis-pack.github.io/cmsis-toolbox/)
>   to learn more about CMSIS Solution project build and management tools.

To build the project via command line one can use the following command syntax:

`cbuild Examples.csolution.yml --context <project-name>.<build-type>+<target-type>`

To list the available context execute the following command:

```sh
cbuild list contexts Examples.csolution.yml
```

### Hello World example

- Build example project for Cortex-M3
  ```sh
  cbuild Examples.csolution.yml --context Hello.Debug+CM3 --update-rte
  ```

### TrustZone example (for ArmV8-M with TrustZone)

TrustZone example must always be build in two steps:

1. Build secure side project for Cortex-M55
   ```sh
   cbuild Examples.csolution.yml --context TZ_Secure.Debug+CM55 --update-rte
   ```
2. Build non-secure side project for Cortex-M55
   ```sh
   cbuild Examples.csolution.yml --context TZ_NonSecure.Debug+CM55 --update-rte
   ```

## Run Example Project on Virtual Hardware Target model

Run command syntax:

`<model-executable> -f ./Target/<target_type>/fvp_config.txt ./out/<project>/<project>.axf`

Examples:

- Run Hello World project executable on Cortex-M3 model
```sh
FVP_MPS2_Cortex-M3 -f ./Target/CM3/fvp_config.txt ./out/Hello/Hello.axf
```

- Run TrustZone executable on model for Cortex-M55
```sh
FVP_Corstone_SSE-300 -f ./Target/CM55/fvp_config.txt -a ./out/TZ_NonSecure/TZ_NonSecure.axf -a ./out/TZ_Secure/TZ_Secure.axf
```
