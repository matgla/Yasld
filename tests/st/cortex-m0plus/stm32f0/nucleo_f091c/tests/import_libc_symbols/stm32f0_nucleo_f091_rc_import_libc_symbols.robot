*** Settings ***
Resource            stm32f0_nucleo_f091rc_common.robot

Suite Setup         Setup
Suite Teardown      Teardown
Test Teardown       Test Teardown
Test Timeout        10 seconds


*** Variables ***
${TEST_FILE}        @renode_test_binary@
${BOARD_FILE}       @renode_board_file@


*** Test Cases ***
Load shared libc as library module
    Prepare Machine

    Wait For Line On Uart    [host] STM32F0 Nucleo Board started!    timeout=1
    Wait For Line On Uart    [host][strcpy]: this is test string    timeout=1
    Wait For Line On Uart    [host][puts] Used puts from shared libc    timeout=1
