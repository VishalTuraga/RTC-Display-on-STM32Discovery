################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/Src/stm32f407x_i2c.c \
../drivers/Src/stm32f407xx_RCC.c \
../drivers/Src/stm32f407xx_gpio_driver.c \
../drivers/Src/stm32f407xx_spi.c \
../drivers/Src/stm32f407xx_usart.c 

OBJS += \
./drivers/Src/stm32f407x_i2c.o \
./drivers/Src/stm32f407xx_RCC.o \
./drivers/Src/stm32f407xx_gpio_driver.o \
./drivers/Src/stm32f407xx_spi.o \
./drivers/Src/stm32f407xx_usart.o 

C_DEPS += \
./drivers/Src/stm32f407x_i2c.d \
./drivers/Src/stm32f407xx_RCC.d \
./drivers/Src/stm32f407xx_gpio_driver.d \
./drivers/Src/stm32f407xx_spi.d \
./drivers/Src/stm32f407xx_usart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/Src/%.o drivers/Src/%.su drivers/Src/%.cyclo: ../drivers/Src/%.c drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"D:/STM32/2024Workspace/RTC_LED_PROJECT/BSP" -I"D:/STM32/2024Workspace/RTC_LED_PROJECT/drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-drivers-2f-Src

clean-drivers-2f-Src:
	-$(RM) ./drivers/Src/stm32f407x_i2c.cyclo ./drivers/Src/stm32f407x_i2c.d ./drivers/Src/stm32f407x_i2c.o ./drivers/Src/stm32f407x_i2c.su ./drivers/Src/stm32f407xx_RCC.cyclo ./drivers/Src/stm32f407xx_RCC.d ./drivers/Src/stm32f407xx_RCC.o ./drivers/Src/stm32f407xx_RCC.su ./drivers/Src/stm32f407xx_gpio_driver.cyclo ./drivers/Src/stm32f407xx_gpio_driver.d ./drivers/Src/stm32f407xx_gpio_driver.o ./drivers/Src/stm32f407xx_gpio_driver.su ./drivers/Src/stm32f407xx_spi.cyclo ./drivers/Src/stm32f407xx_spi.d ./drivers/Src/stm32f407xx_spi.o ./drivers/Src/stm32f407xx_spi.su ./drivers/Src/stm32f407xx_usart.cyclo ./drivers/Src/stm32f407xx_usart.d ./drivers/Src/stm32f407xx_usart.o ./drivers/Src/stm32f407xx_usart.su

.PHONY: clean-drivers-2f-Src

