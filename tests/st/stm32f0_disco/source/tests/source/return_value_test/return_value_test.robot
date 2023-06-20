*** Settings ***
Suite Setup         Setup
Suite Teardown      Teardown
Test Setup          Reset Emulation


*** Variables ***
${TEST_FILE}    @renode_test_binary@


*** Test Cases ***
Test returing value from module on Cortex-M0
    Start Test

    Wait For Line On Uart    Hello from module!    timeout=1
    Start Emulation


*** Keywords ***
Start Test
    Execute Command    mach create
    Execute Command    machine LoadPlatformDescription @platforms/boards/stm32f072b_discovery.repl

    Execute Command    sysbus LoadBinary @${TEST_FILE} 0x08000000

    Execute Command    sysbus.cpu VectorTableOffset 0x08000000
    Execute Command    ShowAnalyzer sysbus.usart1
    Create Terminal Tester    sysbus.usart1
