# Target Layers for Arm FVP Models

This directory contains pre-configured target layers for Arm Fixed Virtual Platform simulation models.

## Directory Content

Each directory contains layer description (Target.clayer.yml), RTE configuration for the target device and
configuration parameters for the FVP model. Each layer targets a specific device platform. The below table
contains information about target platforms and how they map to the model executable.

| Directory | Target device             | Target platform                | Model executable               |
|:----------|:--------------------------|:-------------------------------|:-------------------------------|
| CM0       | Cortex-M0                 | Arm MPS2 (no v8M additions)    | FVP_MPS2_Cortex-M0             |
| CM0plus   | Cortex-M0+                | Arm MPS2 (no v8M additions)    | FVP_MPS2_Cortex-M0plus         |
| CM3       | Cortex-M3                 | Arm MPS2 (no v8M additions)    | FVP_MPS2_Cortex-M3             |
| CM4       | Cortex-M4                 | Arm MPS2 (no v8M additions)    | FVP_MPS2_Cortex-M4             |
| CM7       | Cortex-M7                 | Arm MPS2 (no v8M additions)    | FVP_MPS2_Cortex-M7             |
| CM23      | Cortex-M23                | Arm MPS2 (with v8M additions)  | FVP_MPS2_Cortex-M23            |
| CM23_noTZ | Cortex-M23 (w/o TrustZone)| Arm MPS2 (with v8M additions)  | FVP_MPS2_Cortex-M23            |
| CM33      | Cortex-M33                | Arm MPS2 (with v8M additions)  | FVP_MPS2_Cortex-M33            |
| CM33_noTZ | Cortex-M33 (w/o TrustZone)| Arm MPS2 (with v8M additions)  | FVP_MPS2_Cortex-M33            |
| CM55      | Cortex-M55                | Arm Corstone SSE-300           | FVP_Corstone_SSE-300           |
| CM55_noTZ | Cortex-M55 (w/o TrustZone)| Arm Corstone SSE-300           | FVP_Corstone_SSE-300           |
| CM85      | Cortex-M85                | Arm Corstone SSE-310           | FVP_Corstone_SSE-310           |
| CM85_noTZ | Cortex-M85 (w/o TrustZone)| Arm Corstone SSE-310           | FVP_Corstone_SSE-310           |

## Target Platform Memory Configuration

### Arm MPS2 (no v8M additions)
Arm MPS2 platform without v8M extensions is used by layers for Cortex-M0, Cortex-M0plus, Cortex-M3, Cortex-M4, and Cortex-M7.

Property   | Value
-----------|-----------
__ROM_BASE | 0x00000000
__ROM_SIZE | 0x00400000
__RAM_BASE | 0x20000000
__RAM_SIZE | 0x00400000

### Arm MPS2 (with v8M additions)
Arm MPS2 platform with v8M extensions is used by layers for Cortex-M23 and Cortex-M33.

Property   | Secure Side | Non-secure Side
-----------|-------------|----------------
__ROM_BASE | 0x10000000  | 0x00200000
__ROM_SIZE | 0x00200000  | 0x00200000
__RAM_BASE | 0x30000000  | 0x20200000
__RAM_SIZE | 0x00200000  | 0x00200000

### Arm Corstone SSE-300
Arm Corstone SSE-300 platform is used by layer for Cortex-M55.

Property   | Secure Side | Non-secure Side
-----------|-------------|----------------
__ROM_BASE | 0x10000000  | 0x01000000
__ROM_SIZE | 0x00080000  | 0x00100000
__RAM_BASE | 0x30000000  | 0x21000000
__RAM_SIZE | 0x00080000  | 0x00080000

### Arm Corstone SSE-310
Arm Corstone SSE-310 platform is used by layer for Cortex-M85.

Property   | Secure Side | Non-secure Side
-----------|-------------|----------------
__ROM_BASE | 0x11000000  | 0x01100000
__ROM_SIZE | 0x00100000  | 0x00100000
__RAM_BASE | 0x31000000  | 0x21200000
__RAM_SIZE | 0x00200000  | 0x00200000
