*** Settings ***
Resource    ${RENODEKEYWORDS}


*** Keywords ***
Prepare Machine
    Execute Command    mach create
    Execute Command    machine LoadPlatformDescription @${BOARD_FILE}
    Execute Command    sysbus LoadBinary @${TEST_FILE} 0x08000000
    Execute Command    sysbus.cpu VectorTableOffset 0x08000000

    Create Terminal Tester    sysbus.usart1

    Start Emulation
