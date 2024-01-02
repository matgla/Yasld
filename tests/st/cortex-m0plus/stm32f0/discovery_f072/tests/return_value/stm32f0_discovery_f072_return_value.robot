*** Settings ***
Resource            stm32f0_discovery_f072_common.robot

Suite Setup         Setup
Suite Teardown      Teardown
Test Teardown       Test Teardown
Test Timeout        10 seconds


*** Variables ***
${TEST_FILE}    @renode_test_binary@


*** Test Cases ***
Calculate sum and return it from module
    Prepare Machine

    Wait For Line On Uart    [host] STM32F0 Discovery Board started!    timeout=1
    Wait For Line On Uart    Module 'sum_module' got arguments:
    Wait For Line On Uart    arg(1): 20
    Wait For Line On Uart    arg(2): 10
    Wait For Line On Uart    arg(3): 1
    Wait For Line On Uart    [host] Returned value: 31
