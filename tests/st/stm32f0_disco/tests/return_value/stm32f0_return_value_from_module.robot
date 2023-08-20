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


*** Keywords ***
Prepare Machine
    Execute Command    mach create
    Execute Command    machine LoadPlatformDescription @platforms/boards/stm32f072b_discovery.repl
    Execute Command    sysbus LoadBinary @${TEST_FILE} 0x08000000
    Execute Command    sysbus.cpu VectorTableOffset 0x08000000

    Create Terminal Tester    sysbus.usart1

    Start Emulation
