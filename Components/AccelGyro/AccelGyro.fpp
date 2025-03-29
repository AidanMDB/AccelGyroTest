module Components {

    @ 3-tuple type used for telemetry
    array F32x3 = [3] F32

    @ Manager for the accelerometer and gyroscope
    passive component AccelGyro {

        #------------------------------------------------------------------------------
        # Commands
        #------------------------------------------------------------------------------

        @ Command to turn on or off the accelerometer and gyroscope
        guarded command POWER_ON_OFF(
            powerState: Fw.On   @< Indicates whether the device is on or off
        ) \ 
        opcode 0x01

        #------------------------------------------------------------------------------
        # Ports
        #------------------------------------------------------------------------------

        @ Port for sending telemtry to ground
        guarded input port Run: Svc.Sched

        @ Port for write data to device
        output port write: Drv.I2c

        @ Port for read data to device
        output port read: Drv.I2c

        #------------------------------------------------------------------------------
        # Events
        #------------------------------------------------------------------------------

        @ Reports errors when requesting telemetry
        event TelemetryError(
            status: Drv.I2cStatus @< the status value returned
        ) \
            severity warning high \
            format "Telemetry Failed with status: {}"  

        @ Configuration Failed
        event ConfigError(
            writeStatus: Drv.I2cStatus @< the status of writing data to device
        ) \
            severity warning high \
            format "Setup failed wth status: {}"

        @ Device not taken out of sleep mode 
        event PowerModeError(
            writeStatus: Drv.I2cStatus @< the status of writing data to device
        ) \
            severity warning high \
            format "{}"

        @ Report power state
        event PowerState(
            powerStatus: Fw.On
        ) \
            severity activity high \
            format "Device has been turned {}"

        #------------------------------------------------------------------------------
        # Telemetry
        #------------------------------------------------------------------------------

        @ Report X, Y, Z acceleration from accelerometer
        telemetry accelerometer: F32x3 \
        id 0x01 \
        update always \
        format "{}"

        @ Report X, Y, Z degrees from gyroscope
        telemetry gyroscope: F32x3 \
        id 0x02 \
        update always \
        format "{} deg/s"

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

    }
}