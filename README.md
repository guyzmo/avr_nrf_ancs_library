ANCS Library for Arduino and nrf8001
===

Apple Notification Center Services (ANCS) makes it easy to get notifications from an iOS device over Bluetooth Low Energy (BLE). The nrf8001 is a BLE chip from Nordic Semiconductor that makes BLE easy. There are Arduino libraries for that make it easy to integrate into your projects.

This library integrates this all together, and lets you easily make cool Arduino projects like, waving a flag or blinking lights whenever you get a new email.


Hardware
--
The library has been tested with the [RedBear Labs BLE shield](http://redbearlab.com/bleshield/) and should also work with the [Bluefruit LE Breakout Board](https://www.adafruit.com/products/1697).

Install
--
 - Download and follow the directions for installing the Nordic Arduino libraries: [https://github.com/NordicSemiconductor/ble-sdk-arduino](https://github.com/NordicSemiconductor/ble-sdk-arduino)
 - Download this library, unzip it, and copy it to the Arduino Library folder.

Examples
--
 - LCD - this example will display notification on an LCD display using the LiquidCrystal library. It should provide a good starting point for building your own awesomeness. **Make sure you update the example with the Pins you are using for the BLE Shield and the LCD Shield!**

Usage
--
A couple of additional libraries are needed in order to make things work. Start off by including the following:


    #include <lib_aci.h>
    #include <SPI.h>
    #include <EEPROM.h>

Next include this library:

    #include <notif.h>

The library handles setting up the BLE chip and you need to provide it with the pins you are using for `RDYN` & `REQN` - `Notif(uint8_t reqnPin, uint8_t rdynPin`:

    Notif notif(12,11);

In the setup() function of you program you need to call `notif.setup();` and also tell the library the callbacks to use when ANCS events occur. More on that in the Callbacks section below... and of course change the names of the functions you pass to match yours.

    void setup(void)
    {
          notif.setup();
          notif.set_notification_callback_handle(ancs_notifications);
          notif.set_connect_callback_handle(ancs_connected);
          notif.set_disconnect_callback_handle(ancs_disconnected);
    }

Finally you need to call the library in your loop() to read and process the BLE messages.

    void loop()
    {
      notif.ReadNotifications();
    }

Of course, if you run it now nothing will happen. You need to setup some Callback functions.... *read on*

Callbacks
--
When interesting things happen the library will call the functions you set. This is how you make things happen!

###Notification
This Callback is called everytime a new notification is sent from the bonded iOS device. Use the following template for the function in your sketch you want called: `void function_name(ancs_notification_t* notif)`

The callback is set by passing the library the function in your sketch you want called:

    notif.set_notification_callback_handle(ancs_notifications);

###Connect
This Callback is called when an iOS device connects to the BLE board. Use the following template for the function in your sketch you want called: `void function_name()`

The callback is set by passing the library the function in your sketch you want called:

    notif.set_connect_callback_handle(ancs_connected);

###Connect
This Callback is called when an iOS device disconnects from the BLE board. Use the following template for the function in your sketch you want called: `void function_name()`


The callback is set by passing the library the function in your sketch you want called:
    notif.set_disconnect_callback_handle(ancs_connected);


Defines
--
`#define DEBUG1`
Turns on debugging using a Macro. With this off, the serial.prints are ignored and a lot of memory is saved.

`#define ANCS_USE_APP`
`#define ANCS_USE_SUBTITLE`
Turns on the respective Notification fields. With this off, they will not be fetched and cached, which saves memory.


Pairing an iOS Device
--
Once you have a Sketch written and loaded on your Arduino, it is time to pair your iPhone or iPad with it. On the iOS device go into `Settings`, then `Bluetooth`. Under `Devices` look for `Notif` and tap it. It should ask you if you would like to Pair. After that, both the BLE board and the iOS device will be Bonded and remember the connection. If things are working right, both sides will automatically reconnect when they are in range and this information should be remembered even after the Arduino is reset.

Unpairing
--
You can break the Pairing on the iOS device go into `Settings`, then `Bluetooth` and then click on the `i` next to `Notif`. It will bring you to a screen with an option to `Forget This Device`. After doing this on the iOS side, you may need to reset the Arduino before it will forget its pairing information and allow for a new pairing.

BluefruitLE Breakout Board
--
If you are using the Breakout board from Adafruit you need to modify notif.cc and let it know what pins you are using for Reset and Interupt. Goto Line ~800 of notif.cc and edit appropriately:

    aci_state.aci_pins.reset_pin              = 9; // was UNUSED
    aci_state.aci_pins.active_pin             = UNUSED;
    aci_state.aci_pins.optional_chip_sel_pin  = UNUSED;
    
    aci_state.aci_pins.interface_is_interrupt = true; // was false
    aci_state.aci_pins.interrupt_number       = 1; // (on the Micro/Leo, pin2 is int1) was UNUSED

LICENSES
--

Below, you'll find the license notice for each software included in this source tree.

=== DISCLAIMER ===

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.


=== ANCS LIBRARY ===

(c)2013, 2014 Bernard Pratz, bernard@pratz.net, All rights reserved.

Entirely based on public Apple Specification for the ANCS

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
