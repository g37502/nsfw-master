################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/StrUtils.cpp \
../src/TxtClassifier.cpp \
../src/TxtFeature.cpp \
../src/TxtFeatureExtraction.cpp \
../src/TxtFeatureSet.cpp \
../src/TxtFilter.cpp \
../src/TxtSampleSet.cpp \
../src/TxtWebPageUtils.cpp 

C_SRCS += \
../src/alpha-map.c \
../src/darray.c \
../src/fileutils.c \
../src/tail.c \
../src/trie.c 

OBJS += \
./src/StrUtils.o \
./src/TxtClassifier.o \
./src/TxtFeature.o \
./src/TxtFeatureExtraction.o \
./src/TxtFeatureSet.o \
./src/TxtFilter.o \
./src/TxtSampleSet.o \
./src/TxtWebPageUtils.o \
./src/alpha-map.o \
./src/darray.o \
./src/fileutils.o \
./src/tail.o \
./src/trie.o 

C_DEPS += \
./src/alpha-map.d \
./src/darray.d \
./src/fileutils.d \
./src/tail.d \
./src/trie.d 

CPP_DEPS += \
./src/StrUtils.d \
./src/TxtClassifier.d \
./src/TxtFeature.d \
./src/TxtFeatureExtraction.d \
./src/TxtFeatureSet.d \
./src/TxtFilter.d \
./src/TxtSampleSet.d \
./src/TxtWebPageUtils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DHAVE_LANGINFO_CODESET -I"../include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"../include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


