*** Settings ***
Resource        ${RENODEKEYWORDS}

*** Keywords ***
Prepare Machine 
    Execute Command     mach create
    Execute Command     machine LoadPlatformDescription @platforms/boards/stm32f072b_discovery.repl
    Execute Command     sysbus LoadBinary @${TEST_FILE} 0x08000000
    Execute Command     sysbus.cpu VectorTableOffset 0x08000000

    Create Terminal Tester    sysbus.usart1

    Start Emulation
