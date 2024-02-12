*** Settings ***
Resource            stm32f0_discovery_f072_common.robot

Suite Setup         Setup
Suite Teardown      Teardown
Test Teardown       Test Teardown
Test Timeout        10 seconds


*** Variables ***
${TEST_FILE}    @renode_test_binary@


*** Test Cases ***
Calculate of values in module using imported symbols
    Prepare Machine

    Wait For Line On Uart    [host] STM32F0 Discovery Board started!    timeout=1
    Wait For Line On Uart    Module 'executable' got array size: 3
    Wait For Line On Uart    3.150000
    Wait For Line On Uart    23.141230
    Wait For Line On Uart    -1231.123047
    Wait For Line On Uart    [host] Module returned: -5
