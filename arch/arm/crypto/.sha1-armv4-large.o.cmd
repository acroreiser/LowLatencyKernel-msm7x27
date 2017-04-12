cmd_arch/arm/crypto/sha1-armv4-large.o := /home/bootahn/arm-eabi-4.7/bin/arm-eabi-gcc -Wp,-MD,arch/arm/crypto/.sha1-armv4-large.o.d  -nostdinc -isystem /home/bootahn/arm-eabi-4.7/bin/../lib/gcc/arm-eabi/4.7/include -I/home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-msm/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=6 -march=armv6k -mtune=arm1136j-s -include asm/unified.h -msoft-float        -c -o arch/arm/crypto/sha1-armv4-large.o arch/arm/crypto/sha1-armv4-large.S

source_arch/arm/crypto/sha1-armv4-large.o := arch/arm/crypto/sha1-armv4-large.S

deps_arch/arm/crypto/sha1-armv4-large.o := \
  /home/bootahn/desktop/LowLatencyKernel-msm7x27/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \

arch/arm/crypto/sha1-armv4-large.o: $(deps_arch/arm/crypto/sha1-armv4-large.o)

$(deps_arch/arm/crypto/sha1-armv4-large.o):
