*** Settings ***
Resource            stm32f0_discovery_f072_common.robot

Suite Setup         Setup
Suite Teardown      Teardown
Test Teardown       Test Teardown
Test Timeout        10 seconds


*** Variables ***
${TEST_FILE}    @renode_test_binary@


*** Test Cases ***
Support for cpp classes and polymorphism together with imported constructors
    Prepare Machine

    Wait For Line On Uart    [host] STM32F0 Discovery Board started!    timeout=1
    Wait For Line On Uart    Test local scope
    Wait For Line On Uart    TestClassModule()
    Wait For Line On Uart    ~TestClassModule()
    Wait For Line On Uart    Sum from internal: -11
    Wait For Line On Uart    and the name is: its working :)
    Wait For Line On Uart    ~InternalImplementation()
    Wait For Line On Uart    ~InternalClassInterface()
    Wait For Line On Uart    ExternalImplementation(other implementation)
    Wait For Line On Uart    Sum from external: 4
    Wait For Line On Uart    and the name is: other implementation
    Wait For Line On Uart    Sum from internal unique_ptr: 58
    Wait For Line On Uart    and the name is: unique_ptr test
    Wait For Line On Uart    ~InternalImplementation()
    Wait For Line On Uart    ~InternalClassInterface()
    Wait For Line On Uart    ExternalImplementation(this is another external implementation)
    Wait For Line On Uart    Sum from external unique_ptr: -199280
    Wait For Line On Uart    and the name is: this is another external implementation
    Wait For Line On Uart    CleanUp
    Wait For Line On Uart    ~ExternalImplementation()
    Wait For Line On Uart    ~InternalClassInterface()
    Wait For Line On Uart    ~ExternalImplementation()
    Wait For Line On Uart    ~InternalClassInterface()
    Wait For Line On Uart    Errno is: Success
