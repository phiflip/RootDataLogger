# RootDataLogger – Data Logging System for Root Pull-Out Tests

RootDataLogger is a **data logging system** designed for **measuring root displacement under applied force**. It is used in **root pull-out experiments** to study root mechanical properties and their role in soil stabilization.

The system uses a **LoadCell (HX711)** to apply force and a **Wire Sensor** to measure root displacement. Measurement data is recorded on an **SD card**, and real-time transmission via **Bluetooth (HC-05)** enables remote monitoring. Additionally, the system includes **basic calibration and diagnostic tools** to improve measurement accuracy.

---

## **Project Structure**
```plaintext
RootDataLogger/
│── Docs/              # Documentation, manuals, schematics, sensor details
│   ├── Example/       # Sample files & test logs
│   ├── Manuals/       # User guide
│   ├── Schematics/    # Circuit diagram
│   ├── Images/        # Photos
│   ├── SensorInfo/    # Sensor-related documentation
│       ├── inventory.md         # List of sensors & hardware used
│       ├── wiring_diagram.pdf   # Sensor wiring & circuit diagram
│       ├── calibration_data/    # Calibration values for LoadCell & Wire Sensor
│       ├── sensor_specs/        # Sensor datasheets (HX711, Wire Sensor, RTC)
│       ├── maintenance_log.md   # Sensor maintenance records
│
│── Code/              # Main firmware for the logger
│   ├── Logger_I/      # Code for Logger I
│   ├── Logger_II/     # Code for Logger II
│
│── Scripts/           # Helper scripts for data analysis & processing
│   ├── log_converter.py      # Converts SD logs into CSV/JSON
│
│── Tests/             # Calibration & functional tests
│   ├── test_loadcell.ino      # LoadCell test & calibration
│   ├── test_rtc.ino           # RTC reset & verification
│   ├── test_wire_sensor.ino   # Wire Sensor functionality test
│
│── README.md          # Project description & usage guide
│── .gitignore         # Excludes unnecessary files
```

---

## **Features**
✅ **Root Pull-Out Testing** – Measures root displacement under applied force  
✅ **LoadCell (HX711) Integration** – Records applied force values  
✅ **Wire Sensor Support** – Captures displacement of roots  
✅ **SD Card Logging** – Saves measurements in `.csv` format  
✅ **Real-Time Clock (RTC)** – Adds timestamps to measurement data 
✅ **Bluetooth HC-05** – Enables remote data monitoring
✅ **Basic Calibration Tools** – Ensures accurate sensor readings

---

## 📊 **Data Processing & Analysis**
Recorded measurement logs can be processed using **Python**:
```bash
python scripts/log_converter.py log_YYYY-MM-DD.txt
```
Alternatively, the data can be opened in **Excel/Google Sheets** for analysis.

---

## **License & Contributions**
This project is **Open Source** and intended for **scientific research in root mechanics and soil stability**. Contributions, bug reports, and feature suggestions are welcome.  

📅 Created by PhiFlip|HAFL – © 2025  

---
