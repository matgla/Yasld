*** Settings ***
Resource            stm32f0_discovery_f072_common.robot

Suite Setup         Setup
Suite Teardown      Teardown
Test Teardown       Test Teardown
Test Timeout        10 seconds


*** Variables ***
${TEST_FILE}    @renode_test_binary@


*** Test Cases ***
Print 'Hello World' on USART from relocated module using imported symbols
    Prepare Machine

    Wait For Line On Uart    [host] STM32F0 Discovery Board started!    timeout=1
    Wait For Line On Uart    Hello from non-standalone module!!
