ANCS Library for AVR/Arduino and nrf8001
===

This README file is a summary of how to build, use and of the licenses of the project.

Build
--

 * make commands
   * `flush`: deletes all compiled objets
   * `all`: compiles the firmware
   * `monitor`: show the output from the serial terminal (needs miniterm.py)
 * DEFINES options:
   * `-DNO_ANCS`: drops ANCS support
   * `-DDEBUG1 -DDEBUG2 -DDEBUG3`: adds levels of debugging

 * example:

To flash using the JTAG ICE 3 programmer the fully featured firmware:

    make DEFINES="" flush all flash2 monitor

To flash using the JTAG ICE 3 programmer the no-ancs firmware:

    make DEFINES="-DNO_ANCS" flush all flash2 monitor

To flash using the JTAG ICE 3 programmer with no ancs firmware:

    make DEFINES="-DNO_ANCS" flush all flash2 monitor

To flash using the JTAG ICE 3 programmer with debug output firmware:

    make DEFINES="-DDEBUG1 -DDEBUG2 -DDEBUG3" flush all flash2 monitor

To flash using the bootloader:

    make flush all flash

Libraries
--


    .
    |-- ancs_lib
    |   | Library that contains generic implementation of ANCS communication
    |   | following the Apple specifications
    |   `-- data_lib
    |         Library that contains utility tools in public domain
    |-- ble_lib
    |     Library that contains public domain source code mostly from Nordic
    |     Semiconductors implementing the nRF8001 specification. Only the PIPE
    |     definition is specific to the project.
    |-- Makefile
    |     File containing the build instructions
    `-- avr_nrf_ancs_library.ino
          Source file containing the entry point for the project. 
          Most of that file is generic source code.

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

=== BLE LIBRARY ===

(c)2014 Bernard Pratz , bernard@pratz.net. All rights reserved.
(c)2013-2014 Nordic Semiconductors ASA, All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or
    other materials provided with the distribution.

    Neither the name of Nordic Semiconductor ASA nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

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

