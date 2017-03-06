#!/bin/bash
set -e

FILES=(
# arch
#arch/arm/mach-rk30/clock_data.c
#arch/arm/mach-rk30/ddr*.c
)

DIRS=(
)

EXCLUDES=(
pack-kernel*

arch/arm/plat-rk/vpu*.c
arch/arm/mach-rk29/*.c
arch/arm/mach-rk29/*.h
arch/arm/mach-rk29/*.S
arch/arm/mach-rk29/Makefile*
arch/arm/mach-rk29/include
arch/arm/configs/rk29*

drivers/*rk28*.c
include/*rk28*

drivers/staging/rk29/vivante

drivers/staging/rk29/ipp/rk29-ipp.c

arch/arm/mach-rk30/board-rk30-phone*
arch/arm/configs/rk30_phone_*
)

defconfig=${2-rk30_sdk_defconfig}
. pack-kernel-common.sh
