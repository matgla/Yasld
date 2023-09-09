*** Settings ***
Resource            stm32f0_common.robot

Suite Setup         Setup
Suite Teardown      Teardown
Test Teardown       Test Teardown
Test Timeout        10 seconds


*** Variables ***
${TEST_FILE}    @renode_test_binary@


*** Test Cases ***
Report error for invalid YASIFF file
    Prepare Machine

    Wait For Line On Uart    [host] STM32F0 Discovery Board started!    timeout=1
    Wait For Line On Uart    [host] Module loading failed
