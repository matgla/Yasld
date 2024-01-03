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
Run executable that uses shared libc
    Prepare Machine
    Wait For Line On Uart    [host] STM32F0 Nucleo Board started!    timeout=1
    Wait For Line On Uart    Hello using shared libc    timeout=1
    Wait For Line On Uart    Some values 1234, Float is not supported by baselibc    timeout=1
