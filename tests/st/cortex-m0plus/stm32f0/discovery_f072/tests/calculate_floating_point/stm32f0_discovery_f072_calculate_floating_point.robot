*** Settings ***
Resource            stm32f0_common.robot

Suite Setup         Setup
Suite Teardown      Teardown
Test Teardown       Test Teardown
Test Timeout        10 seconds


*** Variables ***
${TEST_FILE}    @renode_test_binary@


*** Test Cases ***
Calculate floating points (verification of libgcc)
    Prepare Machine

    Wait For Line On Uart    [host] STM32F0 Discovery Board started!    timeout=1
    Wait For Line On Uart    STM32F0 floating point tests
    Wait For Line On Uart    Print test: 1.270000
    Wait For Line On Uart    Sum is: -3.400000
    Wait For Line On Uart    Division test: -3.130435
    Wait For Line On Uart    Sine of 2.300000 is 0.745705
