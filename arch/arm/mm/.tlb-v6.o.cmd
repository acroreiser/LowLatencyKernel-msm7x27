cmd_arch/arm/mm/tlb-v6.o := /home/bootahn/arm-eabi-4.7/bin/arm-eabi-gcc -Wp,-MD,arch/arm/mm/.tlb-v6.o.d  -nostdinc -isystem /home/bootahn/arm-eabi-4.7/bin/../lib/gcc/arm-eabi/4.7/include -I/home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-msm/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=6 -march=armv6k -mtune=arm1136j-s -include asm/unified.h -msoft-float     -Wa,-march=armv6   -c -o arch/arm/mm/tlb-v6.o arch/arm/mm/tlb-v6.S

source_arch/arm/mm/tlb-v6.o := arch/arm/mm/tlb-v6.S

deps_arch/arm/mm/tlb-v6.o := \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/linkage.h \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/linkage.h \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/page.h \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/cpu/copy/v3.h) \
    $(wildcard include/config/cpu/copy/v4wt.h) \
    $(wildcard include/config/cpu/copy/v4wb.h) \
    $(wildcard include/config/cpu/copy/feroceon.h) \
    $(wildcard include/config/cpu/copy/fa.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/xscale.h) \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/copy/v6.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/memory/hotplug/sparse.h) \
  include/asm-generic/getorder.h \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/tlbflush.h \
    $(wildcard include/config/smp/on/up.h) \
    $(wildcard include/config/cpu/tlb/v3.h) \
    $(wildcard include/config/cpu/tlb/v4wt.h) \
    $(wildcard include/config/cpu/tlb/fa.h) \
    $(wildcard include/config/cpu/tlb/v4wbi.h) \
    $(wildcard include/config/cpu/tlb/feroceon.h) \
    $(wildcard include/config/cpu/tlb/v4wb.h) \
    $(wildcard include/config/cpu/tlb/v6.h) \
    $(wildcard include/config/cpu/tlb/v7.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/arm/errata/720789.h) \
    $(wildcard include/config/arch/msm8x60.h) \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/glue.h \
  arch/arm/mm/proc-macros.S \
    $(wildcard include/config/cpu/use/domains.h) \
    $(wildcard include/config/cpu/dcache/writethrough.h) \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/arm/thumbee.h) \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \

arch/arm/mm/tlb-v6.o: $(deps_arch/arm/mm/tlb-v6.o)

$(deps_arch/arm/mm/tlb-v6.o):
