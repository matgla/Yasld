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
    Wait For Line On Uart    [host] STM32F0 Nucleo Board started!       timeout=1
    Wait For Line On Uart    Object 1 contains: (1, 2, 3.140000)        timeout=1
    Wait For Line On Uart    Object 2 contains: (10, 20, 1234.119995)  timeout=1
