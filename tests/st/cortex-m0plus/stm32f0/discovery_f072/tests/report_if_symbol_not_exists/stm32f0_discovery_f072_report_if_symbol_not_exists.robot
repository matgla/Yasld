*** Settings ***
Resource            stm32f0_discovery_f072_common.robot

Suite Setup         Setup
Suite Teardown      Teardown
Test Teardown       Test Teardown
Test Timeout        10 seconds


*** Variables ***
${TEST_FILE}    @renode_test_binary@


*** Test Cases ***
Report if symbol not exists
    Prepare Machine

    Wait For Line On Uart    [host] STM32F0 Discovery Board started!    timeout=1
    Wait For Line On Uart    [YASLD] Can't find symbol: __aeabi_f2d
    Wait For Line On Uart    [host] Module loading failed
