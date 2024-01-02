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
    Wait For Line On Uart    [simple_library] Calculating sum 15 + 22
    Wait For Line On Uart    [simple_library] Call count: 0
    Wait For Line On Uart    [host][sum] Sum is: 37
    Wait For Line On Uart    [simple_library] A: hello, B: elo
    Wait For Line On Uart    [simple_library] Call count: 1
    Wait For Line On Uart    [host][process_str] Str is: aa
    Wait For Line On Uart    [simple_library] Calculating sum 100 + 1000
    Wait For Line On Uart    [simple_library] Call count: 2
    Wait For Line On Uart    [host][sum] Other sum is: 1100
    Wait For Line On Uart    [simple_library] A: other, B: test
    Wait For Line On Uart    [simple_library] Call count: 3
    Wait For Line On Uart    [host][process_str] Str is: aa

    Wait For Line On Uart    [simple_library] Got arguments using stack: 1, 10, 20, 30, 5
    Wait For Line On Uart    [simple_library] Call count: 4
    Wait For Line On Uart    [host][c_fun] Calculated: 1200

    Wait For Line On Uart    [simple_library] Got arguments using stack: 20, 123, 4, 80, 1000
    Wait For Line On Uart    [simple_library] Call count: 5
    Wait For Line On Uart    [host][c_fun] Calculated: 787

    Wait For Line On Uart    [host] TEST SUCCESS
