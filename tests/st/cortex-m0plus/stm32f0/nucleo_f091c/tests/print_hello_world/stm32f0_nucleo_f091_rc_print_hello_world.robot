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
Print 'Hello World' on USART from relocated module
    Prepare Machine

    Wait For Line On Uart    [host] STM32F0 Nucleo Board started!    timeout=1
    Wait For Line On Uart    Hello from module    timeout=1
