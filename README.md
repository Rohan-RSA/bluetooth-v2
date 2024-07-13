Exploring: sys work queue, zbus, BLE and more
Project goal: BLE board that advertises a certain sensor type depending on the position of a rotary switch. Sensor types: PTO, Pressure, Flow.
              PTO and pressure sensor type will be non-connectable extended and coded advertising sets while flow sensor type is connectable and
              the flow sensor data has to be streamed over a serial BLE connection. This serial connection must be the Ublox SPS protocol since
              the device receiving the data can only understand SPS.
