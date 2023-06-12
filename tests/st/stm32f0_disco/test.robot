*** Settings ***
Suite Setup         Setup
Suite Teardown      Teardown
Test Setup          Reset Emulation
Test Teardown       Test Teardown


*** Variables ***
${UART}     sysbus.usart1


*** Test Cases ***
Should Run Hello World
    [Timeout]    2
    Create Machine
    Create Terminal Tester    ${UART}
    Start Emulation

    Wait For Line On Uart    Hello from testing module!


*** Keywords ***
Create Machine
    Execute Command    mach create
    Execute Command    machine LoadPlatformDescription @platforms/boards/stm32f072b_discovery.repl
    Execute Command    sysbus LoadELF @/home/mateusz/repos/Yasld/tests/st/stm32f0_disco/build/hello_world.elf
