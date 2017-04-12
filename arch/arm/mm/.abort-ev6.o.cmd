cmd_arch/arm/mm/abort-ev6.o := /home/bootahn/arm-eabi-4.7/bin/arm-eabi-gcc -Wp,-MD,arch/arm/mm/.abort-ev6.o.d  -nostdinc -isystem /home/bootahn/arm-eabi-4.7/bin/../lib/gcc/arm-eabi/4.7/include -I/home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-msm/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=6 -march=armv6k -mtune=arm1136j-s -include asm/unified.h -msoft-float     -Wa,-march=armv6k   -c -o arch/arm/mm/abort-ev6.o arch/arm/mm/abort-ev6.S

source_arch/arm/mm/abort-ev6.o := arch/arm/mm/abort-ev6.S

deps_arch/arm/mm/abort-ev6.o := \
    $(wildcard include/config/cpu/v6.h) \
    $(wildcard include/config/cpu/32v6k.h) \
    $(wildcard include/config/cpu/endian/be8.h) \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/linkage.h \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/smp.h) \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/hwcap.h \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/domain.h \
    $(wildcard include/config/verify/permission/fault.h) \
    $(wildcard include/config/io/36.h) \
    $(wildcard include/config/cpu/use/domains.h) \
    $(wildcard include/config/emulate/domain/manager/v7.h) \
  arch/arm/mm/abort-macro.S \

arch/arm/mm/abort-ev6.o: $(deps_arch/arm/mm/abort-ev6.o)

$(deps_arch/arm/mm/abort-ev6.o):
