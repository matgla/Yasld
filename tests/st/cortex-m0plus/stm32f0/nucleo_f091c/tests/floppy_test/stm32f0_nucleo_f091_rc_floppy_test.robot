*** Settings ***
Resource            stm32f0_nucleo_f091rc_common.robot

Suite Setup         Setup
Suite Teardown      Teardown
Test Teardown       Test Teardown
Test Timeout        10 seconds


*** Variables ***
${TEST_FILE}        @renode_test_binary@
${BOARD_FILE}       @renode_board_file@


*** Test Cases ***
Interoperability between executable and libraries
    Prepare Machine

    Wait For Line On Uart    [host] STM32F0 Nucleo Board started!    timeout=1
    Wait For Line On Uart    [floppy] starts execution    timeout=1
    Wait For Line On Uart    [floppy]: is doing stupid things    timeout=1
    Wait For Line On Uart    [floppy]: is greeting YouTube fans    timeout=1
    Wait For Line On Uart    [floppy] rules the world 0 time    timeout=1
    Wait For Line On Uart    [floppy] is spawning cat 0    timeout=1

    Wait For Line On Uart    [floppy]: is doing stupid things    timeout=1
    Wait For Line On Uart    [floppy]: is greeting YouTube fans    timeout=1
    Wait For Line On Uart    [floppy] rules the world 1 time    timeout=1
    Wait For Line On Uart    [floppy] is spawning cat 1    timeout=1

    Wait For Line On Uart    [floppy]: is doing stupid things    timeout=1
    Wait For Line On Uart    [floppy]: is greeting YouTube fans    timeout=1
    Wait For Line On Uart    [floppy] rules the world 2 time    timeout=1
    Wait For Line On Uart    [floppy] is spawning cat 2    timeout=1

    Wait For Line On Uart    [floppy]: is doing stupid things    timeout=1
    Wait For Line On Uart    [floppy]: is greeting YouTube fans    timeout=1
    Wait For Line On Uart    [floppy] rules the world 3 time    timeout=1
    Wait For Line On Uart    [floppy] is spawning cat 0    timeout=1

    Wait For Line On Uart    [floppy]: is doing stupid things    timeout=1
    Wait For Line On Uart    [floppy]: is greeting YouTube fans    timeout=1
    Wait For Line On Uart    [floppy] rules the world 4 time    timeout=1
    Wait For Line On Uart    [floppy] is spawning cat 1    timeout=1

    Wait For Line On Uart    [HOST rules the world 0 time    timeout=1
    Wait For Line On Uart    [HOST rules the world 1 time    timeout=1
    Wait For Line On Uart    [HOST rules the world 2 time    timeout=1

    Wait For Line On Uart    [floppy] starts execution    timeout=1
    Wait For Line On Uart    [floppy]: is doing stupid things    timeout=1
    Wait For Line On Uart    [floppy]: is greeting YouTube fans    timeout=1
    Wait For Line On Uart    [floppy] rules the world 5 time    timeout=1
    Wait For Line On Uart    [floppy] is spawning cat 0    timeout=1

    Wait For Line On Uart    [floppy]: is doing stupid things    timeout=1
    Wait For Line On Uart    [floppy]: is greeting YouTube fans    timeout=1
    Wait For Line On Uart    [floppy] rules the world 6 time    timeout=1
    Wait For Line On Uart    [floppy] is spawning cat 1    timeout=1

    Wait For Line On Uart    [HOST] rules the world 3 time    timeout=1
