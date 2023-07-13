*** Settings ***
Resource            ${RENODEKEYWORDS}

Suite Setup         Setup
Suite Teardown      Teardown
Test Teardown       Test Teardown
Test Timeout        10 seconds


*** Variables ***
${TEST_FILE}    @renode_test_binary@


*** Test Cases ***
Print hello world to USART from standalone executable
    Prepare Machine

    Wait For Line On Uart    [host] Float test on host: 1.245600    timeout=1
    Wait For Line On Uart    Executing module    timeout=1
    Wait For Line On Uart    [TEST] STM32F0 Floating Point Test     timeout=1
    Wait For Line On Uart    [TEST] Print test: 1.270000     timeout=1
    Wait For Line On Uart    [TEST] Sum of 1.50 and 2.70 is 4.20    timeout=1
    Wait For Line On Uart    [TEST] Floating point division: 1.80    timeout=1
    Wait For Line On Uart    [TEST] Errno is: Success     timeout=1


*** Keywords ***
Prepare Machine
    Execute Command    mach create
    Execute Command    machine LoadPlatformDescription @platforms/boards/stm32f072b_discovery.repl
    Execute Command    sysbus LoadBinary @${TEST_FILE} 0x08000000
    Execute Command    sysbus.cpu VectorTableOffset 0x08000000

    Create Terminal Tester    sysbus.usart1

    Start Emulation
