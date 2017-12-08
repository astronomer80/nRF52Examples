/* Author: Angelo Ferrante astronomer@tiscali.it

   This example shows how to update the manufacturer data in the advertising packet  with the
   Nordic nRF52 and the Arduino/Whatsnext nRF52 platform
   For more information read the free Nordic ebook:
   https://response.nordicsemi.com/download-getting-started-with-bluetooth-low-energy-development
*/

#include <BLEPeripheral.h>
#include <BLEUuid.h>

// create peripheral instance
BLEPeripheral blePeripheral = BLEPeripheral();

// uuid's can be:
//   16-bit: "ffff"
//  128-bit: "19b10010e8f2537e4f6cd104768a1214" (dashed format also supported)

// create one or more services
BLEService service = BLEService("fff0");

// create one or more characteristics
BLECharCharacteristic characteristic = BLECharCharacteristic("fff1", BLENotify | BLERead | BLEWrite);

// create one or more descriptors (optional)
BLEDescriptor descriptor = BLEDescriptor("2901", "value");
uint8_t val = 0;
void setup() {
  Serial.begin(115200);

  //initialize BLE led
  pinMode(BLE_LED, OUTPUT);

  //Initialize the BLE
  bleInit();
}

void bleInit() {
  //Set the advertising data that contains the device name and the manufacturer data
  handleManufacturerData();

  // add attributes (services, characteristics, descriptors) to peripheral
  blePeripheral.addAttribute(service);
  blePeripheral.addAttribute(characteristic);
  blePeripheral.addAttribute(descriptor);

  // set initial value
  characteristic.setValue(val++);

  // begin initialization
  blePeripheral.begin();
}

//Set the advertising data that contains the manufacturer data
void handleManufacturerData() {
  /*The advertising packet contains the device name and the manufacturer data
    0x0B = length 11 octets
    0x09 = AD type Complete Local Name
    0x41,0x73,0x74,0x72,0x6f,0x6e,0x6f,0x6d,0x65,0x72 = “Astronomer” in ASCII
    0x02 = length 2 octects
    0x01 = AD type flags
    0x04 = Flag BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED BR/EDR, ie classic bluetooth, is not supported.
    0x06 =length 5 octects
    0xFF = Manufacturer data
    0x66, 0x66= Undefined company name https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers
    0x00,0x00, val++ = The changing value*/

                         //local name                                                          //AD type               //Manufacturer
  uint8_t adv_data[22] = {0x0B, 0x09, 0x41, 0x73, 0x74, 0x72, 0x6F, 0x6E,0x6F, 0x6D, 0x65,0x72, 0x02, 0x01, 0x04, 0x06, 0xFF, 0x66, 0x66, 0x00, 0x00, val++};
  sd_ble_gap_adv_data_set(adv_data, sizeof(adv_data), NULL, 0);
}
long time1 = millis();

void loop() {
  if (millis() - time1 > 3000) {
    time1 = millis();

    //Stop advertising. Necessary before update
    sd_ble_gap_adv_stop();

    //Update advertising data
    handleManufacturerData();

    //Advertising Restart
    ble_gap_adv_params_t advertisingParameters;
    memset(&advertisingParameters, 0x00, sizeof(advertisingParameters));
    //See Nordic documentation here: https://cdn2.hubspot.net/hubfs/1961165/eBooks/NordicSemi_eBook_Bluetooth_Low_Energy_development.pdf?__hssc=8439722.2.1512759062399&__hstc=8439722.c16423bf00258698ac21e5ce693435a7.1512254883653.1512595083802.1512759062399.5&__hsfp=720596065&hsCtaTracking=0c610996-d8ee-40c9-82f4-b4db77681f9b%7Cfe75fcbd-83e6-4bfd-94b3-1c9084f36d86
    advertisingParameters.type        = BLE_GAP_ADV_TYPE_ADV_IND; // connectable undirected advertising
    advertisingParameters.p_peer_addr = NULL;
    advertisingParameters.fp          = BLE_GAP_ADV_FP_ANY;
    advertisingParameters.p_whitelist = NULL;  //If defines It will reject packets from scanners/centrals whose addresses are not in the list
    advertisingParameters.interval    = (150 * 16) / 10; // advertising interval (in units of 0.625 ms)
    advertisingParameters.timeout     = 0;
    sd_ble_gap_adv_start(&advertisingParameters);

    //Update the same value for the characteristic
    characteristic.setValue(val++);

  }
}
