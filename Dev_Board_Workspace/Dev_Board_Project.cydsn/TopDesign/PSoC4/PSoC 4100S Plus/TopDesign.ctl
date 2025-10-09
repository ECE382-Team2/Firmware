-- =============================================================================
-- The following directives assign pins to the locations specific for the
-- PSoC 4100 S Plus device.
-- =============================================================================

-- === CapSense ===
attribute port_location of \CapSense:Cmod(0)\ : label is "PORT(4,1)";
attribute port_location of \CapSense:Sns(0)\ : label is "PORT(2,7)";
attribute port_location of \CapSense:Sns(1)\ : label is "PORT(6,0)";
attribute port_location of \CapSense:Sns(2)\ : label is "PORT(6,1)";
attribute port_location of \CapSense:Sns(3)\ : label is "PORT(6,2)";
attribute port_location of \CapSense:Sns(4)\ : label is "PORT(6,4)";

-- === I2C ===
attribute port_location of \EZI2C:scl(0)\ : label is "PORT(3,0)";
attribute port_location of \EZI2C:sda(0)\ : label is "PORT(3,1)";

-- === USER LED ===
attribute port_location of LED_0(0) : label is "PORT(1,0)";
attribute port_location of LED_1(0) : label is "PORT(1,2)";
attribute port_location of LED_2(0) : label is "PORT(1,4)";
attribute port_location of LED_3(0) : label is "PORT(1,6)";
attribute port_location of LED_4(0) : label is "PORT(2,0)";